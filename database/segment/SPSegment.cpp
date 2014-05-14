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

	SPSegment::SPSegment(BufferManager& bufferManager, uint16_t id, uint32_t pageCount)
	: Segment(bufferManager, id, pageCount) {}

	Record SPSegment::lookup(TID id) {
		BufferFrame& frame = fixPage(id, false);

		SlottedPage sp(this, frame);
		Record record = sp.lookup(id);

		unfixPage(frame, false);
		return record;
	}

	TID SPSegment::insert(const Record& record) {
		BufferFrame& frame = findFreeFrame(record.getSize());

		SlottedPage sp(this, frame);
		TID id = sp.createSlot();
		sp.insert(id, record);

		unfixPage(frame, true);
		return id;
	}

	bool SPSegment::update(TID id, const Record& record, bool allowRedirect) {
		BufferFrame& frame = fixPage(id, true);

		SlottedPage sp(this, frame);
		bool success = sp.update(id, record, allowRedirect);

		unfixPage(frame, true);
		return success;
	}

	bool SPSegment::remove(TID id) {
		BufferFrame& frame = fixPage(id, true);

		SlottedPage sp(this, frame);
		sp.remove(id);

		unfixPage(frame, true);
		return true;
	}

	BufferFrame& SPSegment::findFreeFrame(int32_t requestedSize, uint32_t startPage) {
		// Try to find a frame with enough space
		for (uint32_t page = startPage; page < getPageCount(); ++page) {
			BufferFrame& frame = fixPage(PID(getID(), page), true);

			if (SlottedPage(this, frame).getFreeSpace() >= requestedSize)
				return frame;

			unfixPage(frame, false);
		}

		// No free page found, create and initialize a new one.
		PID id = addPage();
		BufferFrame& frame = fixPage(id, true);
		SlottedPage(this, frame).reset();
		return frame;
	}

}