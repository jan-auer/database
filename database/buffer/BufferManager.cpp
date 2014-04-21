//
//  BufferManager.cpp
//  database
//
//  Created by Jan Michael Auer on 17/04/14.
//  Copyright (c) 2014 LightningSQL. All rights reserved.
//

#include <math.h>
#include "BufferManager.h"

using namespace std;

namespace lsql {

	BufferManager::BufferManager(const string& fileName, uint64_t size)
	: file(fileName, true), maxPages(size) {
		assert(maxPages > 0);

		// TODO: Come up with something better than log2(float)
		slotCount = uint64_t(1) << int(ceil(log2(maxPages)));

		slots = new BufferSlot[slotCount];
		slotLocks = new Lock[slotCount];
	}

	BufferManager::~BufferManager() {
		for (int i = 0; i < slotCount; i++) {
			for (BufferEntry& entry : slots[i]) {
				// TODO: Flush page contents to disc
				delete entry.second;
			}
		}

		delete[] slots;
		delete[] slotLocks;
	}

	BufferFrame& BufferManager::fixPage(const PageId& id, bool exclusive) {
		BufferFrame* frame;
		BufferSlot& slot = slots[hash(id)];

		// Get a slot lock to prevent it from being changed
		Lock slotLock = slotLocks[hash(id)];
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
		file.read(frame->getData(), BufferFrame::SIZE);
		frame->unlock();

		return *frame;
	}

	void BufferManager::unfixPage(BufferFrame& frame, bool isDirty) {
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
			if (id == entry.first) {
				entry.second->lock(exclusive);
				return entry.second;
			}
		}

		return nullptr;
	}

	BufferFrame* BufferManager::allocatePage(BufferSlot& slot, const PageId& id) {
		// TODO: Page out old pages, if necessary.

		BufferFrame* frame = new BufferFrame(id);
		slot.emplace_back(id, frame);
		return frame;
	}

}

