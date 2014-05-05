//
//  BufferManager.cpp
//  database
//
//  Created by Jan Michael Auer on 04/05/14.
//  Copyright (c) 2014 LightningSQL. All rights reserved.
//

#include <cmath>
#include <sstream>

#include "utils/File.h"
#include "BufferManager.h"

#define log(__msg__) \
	pthread_mutex_lock(&m); \
	std::cout << "[" << pthread_self() << "] " << __msg__ << std::endl; \
	pthread_mutex_unlock(&m);
//#define log(__msg__) ;

namespace lsql {

	pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;

	BufferManager::BufferManager(uint64_t size) : freePages(size) {
		assert(freePages > 0);

		queueThreshold = QUEUE_A1_THRESHOLD * size;
		// TODO: Come up with something better than log2(float)
		slotCount = uint64_t(1) << int(ceil(log2(size)));
		pageTable = new Slot[slotCount];
	}

	BufferManager::~BufferManager() {
		for (; slotCount > 0; --slotCount)
			pageTable[slotCount - 1].cleanup();

		delete[] pageTable;
	}

	BufferFrame& BufferManager::fixPage(const PageId& id, bool exclusive) {
		log( id.page() << ": Searching page (slot: " << hash(id) << ")" );

		BufferFrame* frame;
		Slot& slot = pageTable[hash(id)];

		// Get a slot lock to prevent it from being changed
		slot.lock(false);

		// Search for the frame
		frame = acquirePage(slot, id, exclusive);
		if (frame != nullptr) {
			slot.unlock();
			return *frame;
		}

		// Upgrade to an exclusive slot lock
		slot.unlock();
		slot.lock(true);

		// Research for the frame, in case someone was faster than us
		frame = acquirePage(slot, id, exclusive);
		if (frame != nullptr) {
			slot.unlock();
			return *frame;
		}

		// Create and insert a new frame
		frame = allocatePage(slot, id);

		// Load data into the new frame
		readPage(frame);

		// Downgrade the lock, if only a shared lock is desired
		if (!exclusive) {
			log( id.page() << ": Downgrading lock (1 > 0)" );
			// Downgrading the lock requires to release it first. It is very
			// unlikely, that the page gets paged out so quickly.
			frame->unlock();
			frame->lock(false);
		}

		return *frame;
	}

	void BufferManager::unfixPage(BufferFrame& frame, bool isDirty) {
		if (isDirty)
			frame.setDirty();

		log( frame.getId().page() << ": Unlocking." );
		frame.unlock();
	}

	uint64_t BufferManager::hash(const PageId& id) const {
		// TODO: try out crc, murmur, fnv1a instead of identity
		return id.page() & (slotCount - 1);
	}

	BufferFrame* BufferManager::acquirePage(Slot& slot, const PageId& id, bool exclusive) {
		BufferFrame* frame;
		for (frame = slot.getFirst(); frame != nullptr; frame = frame->tableNext) {
			if (id != frame->getId())
				continue;

			log( frame->getId().page() << ": Locking (" << exclusive << ")" );
			frame->lock(exclusive);
			registerPageAccess(frame);
			return frame;
		}

		return nullptr;
	}

	void BufferManager::registerPageAccess(BufferFrame* frame) {
		if (frame->queue == QUEUE_AM) {
			queueAm.bringFront(frame, true);
		} else {
			queueA1.remove(frame, true);
			frame->queue = QUEUE_AM;
			queueAm.prepend(frame, true);
		}
	}

	BufferFrame* BufferManager::allocatePage(Slot& slot, const PageId& id) {
		// TODO: Allocate space AFTER unlocking the slot BEFORE loading.
		log( id.page() << ": Allocating space" );
		if (freePages.fetch_sub(1) < 1) {
			if (!freePage()) {
				std::cerr << "ERROR: Out of memory." << std::endl;
				exit(77);
			}
		}

		log( id.page() << ": Creating frame (1)" );
		BufferFrame* frame = new BufferFrame(id);
		frame->lock(true);
		slot.prepend(frame);
		slot.unlock();

		queueA1.prepend(frame, true);
		return frame;
	}

	bool BufferManager::freePage() {
		if (queueA1.getSize() > queueThreshold) {
			log( "    Pulling page from A1, Am" );
			return freeLastPage(queueA1) || freeLastPage(queueAm);
		} else {
			log( "    Pulling page from Am, A1" );
			return freeLastPage(queueAm) || freeLastPage(queueA1);
		}
	}

	bool BufferManager::freeLastPage(Queue& queue) {
		queue.lock(false);

		for (BufferFrame* frame = queue.getLast(); frame != nullptr; frame = frame->queuePrev) {
			Slot& slot = pageTable[hash(frame->getId())];
			slot.lock(true);

			// TODO: Lock the frame first, then try the slot
			// TODO: Try to lock the slot multiple times with a small timeout
			if (!frame->tryLock(true))
				continue;

//			if (!slot.tryLock(true)) {
//				frame->unlock();
//				continue;
//			}

			log( frame->id.page() << ": Removing page (slot: " << hash(frame->id) << ")" );
			slot.remove(frame);
			slot.unlock();

			queue.remove(frame);
			queue.unlock();

			if (frame->isDirty())
				writePage(frame);

			log( frame->id.page() << ": Unlocking and deleting frame." );
			frame->unlock();
			delete frame;

			freePages++;
			return true;
		}

		queue.unlock();
		return false;
	}

	void BufferManager::readPage(BufferFrame* frame) {
		string fileName = std::to_string(frame->getId().segment());
		File<void> file(fileName, false);

//		log( frame->getId().page() << ": Reading data from disk" );
		file.read(frame->getData(), BufferFrame::SIZE,
				   frame->getId().page() * BufferFrame::SIZE);
		log( frame->getId().page() << ": Read '" << reinterpret_cast<unsigned*>(frame->getData())[0] << "' from file '" << fileName << "' at offset " << frame->getId().page() * BufferFrame::SIZE);
	}
	
	void BufferManager::writePage(BufferFrame* frame) {
		string fileName = to_string(frame->getId().segment());
		File<void> file(fileName, true);

		log( frame->getId().page() << ": Writing '" << reinterpret_cast<unsigned*>(frame->getData())[0] << "' to file '" << fileName << "' at offset " << frame->getId().page() * BufferFrame::SIZE);
		file.write(frame->getData(), BufferFrame::SIZE,
				   frame->getId().page() * BufferFrame::SIZE);
	}

}