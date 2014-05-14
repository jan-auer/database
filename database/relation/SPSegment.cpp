//
//  SPSegment.cpp
//  database
//
//  Created by Jan Michael Auer on 10/05/14.
//  Copyright (c) 2014 LightningSQL. All rights reserved.
//

#include <cassert>
#include <algorithm>
#include <string>

#include "SlottedPage.h"
#include "SPSegment.h"

namespace lsql {

	SPSegment::SPSegment(BufferManager& bufferManager, Relation& relation)
	: bufferManager(bufferManager), relation(relation) {}

	Record SPSegment::lookup(TID id) {
		BufferFrame& frame = bufferManager.fixPage(id, false);

		SlottedPage sp(this, frame);
		Record record = sp.lookup(id);

		bufferManager.unfixPage(frame, false);
		return record;
	}

	TID SPSegment::insert(const Record& record) {
		BufferFrame& frame = findFreeFrame(record.getSize());

		SlottedPage sp(this, frame);
		TID id = sp.createSlot();
		sp.insert(id, record);

		bufferManager.unfixPage(frame, true);
		return id;
	}

	bool SPSegment::update(TID id, const Record& record, bool allowRedirect) {
		BufferFrame& frame = bufferManager.fixPage(id, true);

		SlottedPage sp(this, frame);
		bool success = sp.update(id, record, allowRedirect);

		bufferManager.unfixPage(frame, true);
		return success;
	}

	bool SPSegment::remove(TID id) {
		BufferFrame& frame = bufferManager.fixPage(id, true);

		SlottedPage sp(this, frame);
		bool success = sp.remove(id);

		bufferManager.unfixPage(frame, true);
		return success;
	}

	BufferFrame& SPSegment::findFreeFrame(int32_t requestedSize, uint32_t startPage) {
		// Try to find a frame with enough space
		for (uint32_t page = startPage; page < relation.pageCount; ++page) {
			BufferFrame& frame = bufferManager.fixPage(PID(relation.segment, page), true);

			if (SlottedPage(this, frame).getFreeSpace() >= requestedSize)
				return frame;

			bufferManager.unfixPage(frame, false);
		}

		// No frame found, create a new one.
		uint32_t pageId = relation.pageCount++;
		PID id(relation.segment, pageId);
		BufferFrame& frame = bufferManager.fixPage(id, true);

		SlottedPage(this, frame).reset();
		return frame;
	}

}