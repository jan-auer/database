//
//  BufferManager.cpp
//  database
//
//  Created by Jan Michael Auer on 04/05/14.
//  Copyright (c) 2014 LightningSQL. All rights reserved.
//

#include <utility>
#include <cmath>

#include "utils/Logger.h"
#include "utils/File.h"
#include "BufferManager.h"

namespace lsql {

	BufferManager::BufferManager(uint64_t size) : freePages(size) {
		assert(freePages > 0);

		queueThreshold = BUFFER_MANAGER_QUEUE_THRESHOLD * size;
		// TODO: Come up with something better than log2(float)
		slotCount = uint64_t(1) << int(ceil(log2(size)));
		pageTable = new Slot[slotCount];
	}

	BufferManager::~BufferManager() {
		for (; slotCount > 0; --slotCount)
			pageTable[slotCount - 1].cleanup();

		delete[] pageTable;
	}

	BufferFrame& BufferManager::fixPage(const PID& id, bool exclusive) {
		mutex.lock();

		BufferFrame* frame;
		Slot& slot = getSlot(id);

		// Search for the frame
		frame = acquirePage(slot, id);
		if (frame != nullptr) {
			frame->lock(exclusive);
			mutex.unlock();
			return *frame;
		}

		// Create and insert a new frame
		frame = allocatePage(slot, id);
		mutex.unlock();

		// Load data into the new frame
		frame->load();

		// Downgrade the lock, if only a shared lock is desired
		if (!exclusive) {
			frame->unlock();
			frame->lock(false);
		}

		return *frame;
	}

	void BufferManager::unfixPage(BufferFrame& frame, bool isDirty) {
		if (isDirty)
			frame.setDirty();

		frame.unlock();
	}

	uint64_t BufferManager::hash(const PID& id) const {
		// TODO: try out crc, murmur, fnv1a instead of identity
		return id.page() & (slotCount - 1);
	}

	BufferManager::Slot& BufferManager::getSlot(const PID& id) const {
		return pageTable[hash(id)];
	}

	BufferFrame* BufferManager::acquirePage(Slot& slot, const PID& id) {
		BufferFrame* frame = slot.getFirst();
		while (frame != nullptr && id != frame->getId())
			frame = frame->tableNext;

		if (frame != nullptr)
			registerPageAccess(frame);

		return frame;
	}

	void BufferManager::registerPageAccess(BufferFrame* frame) {
		if (frame->queue == QUEUE_AM) {
			queueAm.bringFront(frame);
		} else {
			queueA1.remove(frame);
			frame->queue = QUEUE_AM;
			queueAm.prepend(frame);
		}
	}

	BufferFrame* BufferManager::allocatePage(Slot& slot, const PID& id) {
		BufferFrame* frame;

		if (freePages > 0) {
			frame = new BufferFrame(id);
			freePages--;
		} else {
			BufferFrame* unused = findUnusedPage();
			unused->save();

			frame = new BufferFrame(id, std::move(*unused));

			unused->unlock();
			delete unused;
		}

		frame->lock(true);
		slot.prepend(frame);

		frame->queue = QUEUE_A1;
		queueA1.prepend(frame);

		return frame;
	}

	BufferFrame* BufferManager::findUnusedPage() {
		BufferFrame* unused;

		if (queueA1.getSize() > queueThreshold) {
			unused = getLastUnusedPage(queueA1);
			if (unused == nullptr)
				unused = getLastUnusedPage(queueAm);
		} else {
			unused = getLastUnusedPage(queueAm);
			if (unused == nullptr)
				unused = getLastUnusedPage(queueA1);
		}

		if (unused == nullptr) {
			std::cerr << "ERROR: Out of memory." << std::endl;
			exit(EXIT_CODE_NO_MORE_MEMORY);
		}

		return unused;
	}

	BufferFrame* BufferManager::getLastUnusedPage(Queue& queue) {
		BufferFrame* frame = queue.getLast();
		while (frame != nullptr && !frame->tryLock(true))
			frame = frame->queuePrev;

		if (frame == nullptr)
			return nullptr;

		getSlot(frame->getId()).remove(frame);
		queue.remove(frame);
		return frame;
	}

}