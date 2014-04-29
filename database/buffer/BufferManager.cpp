//
//  BufferManager.cpp
//  database
//
//  Created by Jan Michael Auer on 17/04/14.
//  Copyright (c) 2014 LightningSQL. All rights reserved.
//

#include <cmath>
#include <sstream>
#include <chrono>
#include <thread>
#include "BufferManager.h"

template <class T>
inline std::string toString (const T& t)
{
	std::stringstream ss;
	ss << t;
	return ss.str();
}

namespace lsql {

	bool isUnfixed(BufferFrame* frame) {
		return frame->tryLock(true);
	}

	BufferManager::BufferManager(uint64_t size) : freePages(size) {
		assert(freePages > 0);

		// TODO: Come up with something better than log2(float)
		slotCount = uint64_t(1) << int(ceil(log2(size)));

		slots = new BufferSlot[slotCount];
		slotLocks = new Lock[slotCount];
	}

	BufferManager::~BufferManager() {
		/*
		for (int i = 0; i < slotCount; i++) {
			for (BufferEntry& entry : slots[i]) {
				// TODO: Flush page contents to disc
				delete entry.queueElement;
			}
		}

		delete[] slots;
		delete[] slotLocks;
		*/
	}

	BufferFrame& BufferManager::fixPage(const PageId& id, bool exclusive) {


		cerr << "Fixing page " << id.id << endl;

		BufferFrame* frame;
		BufferSlot& slot = slots[hash(id)];

		// Get a slot lock to prevent it from being changed
		Lock& slotLock = slotLocks[hash(id)];
		slotLock.lock(false);

		// Search for the frame
		frame = acquirePage(slot, id, exclusive);
		if (frame != nullptr) {
			slotLock.unlock();
			return *frame;
		}

		// Upgrade the slot lock to an exclusive lock
		slotLock.unlock();
		slotLock.lock(true);

		// Research for the frame, in case someone was faster than us
		frame = acquirePage(slot, id, exclusive);
		if (frame != nullptr) {
			slotLock.unlock();
			return *frame;
		}

		// Create and insert a new frame
		frame = allocatePage(slot, id);
		frame->lock(true);
		slotLock.unlock();

		// Load data into the new frame
		readPage(frame);

		// Downgrade the lock, if only a shared lock is desired
		if (!exclusive) {
			// Downgrading the lock requires to release it first. It is very
			// unlikely, that the page gets paged out so quickly.
			frame->unlock();
			frame->lock(false);
		}

		return *frame;
	}

	void BufferManager::unfixPage(BufferFrame& frame, bool isDirty) {

		cout << "Unfixing page " << frame.getId().id << endl;

		if (isDirty)
			frame.setDirty();

		frame.unlock();
	}

	uint64_t BufferManager::hash(const PageId& id) const {
		// TODO: try out crc, murmur, fnv1a instead of identity
		return id.page() & (slotCount - 1);
	}

	BufferFrame* BufferManager::acquirePage(BufferSlot& slot, const PageId& id, bool exclusive) {
		for (BufferEntry& entry : slot) {
			if (id != entry.id)
				continue;

			auto queueItem = entry.queueItem;
			BufferFrame* frame = queueItem->value;

			frame->lock(exclusive);
			accessQueueItem(queueItem, entry.queueType);

			return frame;
		}

		return nullptr;
	}

	BufferFrame* BufferManager::allocatePage(BufferSlot& slot, const PageId& id) {
		if (freePages.fetch_sub(1) < 1) {
			bool success = pageOut();
			if (!success) exit(1);
		}

		BufferFrame* frame = new BufferFrame(id);
		BufferQueue<BufferFrame>::Item* queueItem = queueA1.createItemAndEnqueue(frame);
		slot.emplace_back(id, queueItem, A1);

		return frame;
	}

	bool BufferManager::pageOut() {

		//Let's try for 3 times at most
		for (int i = 0; i < 3; ++i) {

			//FixME: Select correct Queue
			BufferQueue<BufferFrame>::Item* item = queueA1.dequeueIf(isUnfixed);

			if (item == nullptr)
				exit(2);

			
			BufferFrame* frame = item->value;

			BufferSlot& slot = slots[hash(frame->getId())];
			Lock& slotLock = slotLocks[hash(frame->getId())];

			// Try to get a lock for the slot containing the
			// frame chosen by the queue to be paged out.
			// If this fails, wait for 1 ms and try again. If it
			// fails again, we probably have a deadlock with
			// another thread who has locked the slot and
			// is waiting for exactly this frame.
			// In this case we unlock the frame. The following
			// access changes the queue, so we start from the
			// beginning again.

			bool locked = (slotLock.tryLock(true) == 0);

			if (!locked) {
				cerr << "Warning! Slot Locked" << endl;
				
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
				locked = (slotLock.tryLock(true) == 0);
			}

			if (!locked) {
				frame->unlock();
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
				continue;
			} else {

				for (auto it = slot.begin(); it != slot.end(); ++it) {
					if ((it->id) == frame->getId()) {

						cout << "erasing " << it->queueItem->value->getId().id << " from slot " << &slot << endl;


						slot.erase(it);
						break;
					}
				}

			}
			
			slotLock.unlock();

			writePage(frame);

			//clean up the memory
			delete item;

			frame->unlock();
			delete frame;

			freePages++;
			return true;
		}

		// after trying for 3 times, the for-loop has exited.
		// that means, we have not been able to successfully
		// page out and return true. Signal failure
		return false;

	}

	void BufferManager::accessQueueItem(BufferQueue<BufferFrame>::Item* queueItem, QueueType& type) {
		switch (type) {
			case Am:
				queueAm.moveFront(queueItem);
				break;

			case A1:

				queueA1.remove(queueItem);
				type = Am;
				queueAm.enqueue(queueItem);
				break;

			default:
				break;
		}
	}

	void BufferManager::readPage(BufferFrame* frame) {

		const PageId& p_id = frame->getId();

		long seg = p_id.segment();
		long pg = p_id.page();

		cout << "reading frame page " << pg << " of segment " << seg << endl;

		const char* fileName = toString(frame->getId().segment()).c_str();
		File<void> file(fileName, false);

		file.read(frame->getData(), BufferFrame::SIZE, frame->getId().page() * BufferFrame::SIZE);
	}

	void BufferManager::writePage(BufferFrame* frame) {
		if (!frame->isDirty())
			return;

		const char* fileName = toString(frame->getId().segment()).c_str();
		File<void> file(fileName, true);

		file.write(frame->getData(), BufferFrame::SIZE, frame->getId().page() * BufferFrame::SIZE);
	}
	
}

