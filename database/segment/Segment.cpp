//
//  Segment.cpp
//  database
//
//  Created by Jan Michael Auer on 14/05/14.
//  Copyright (c) 2014 LightningSQL. All rights reserved.
//

#include <cassert>
#include "Segment.h"

namespace lsql {

	Segment::Segment(BufferManager& bufferManager, uint16_t id, uint32_t pageCount)
	: bufferManager(bufferManager), id(id), pageCount(pageCount) {}

	uint16_t Segment::getID() const {
		return id;
	}

	uint32_t Segment::getPageCount() const {
		return pageCount;
	}

	BufferFrame& Segment::fixPage(PID id, bool exclusive) {
		assert(id.segment() == this->id);
		return bufferManager.fixPage(id, exclusive);
	}

	void Segment::unfixPage(BufferFrame& frame, bool isDirty) {
		assert(frame.getId().segment() == id);
		bufferManager.unfixPage(frame, isDirty);
	}

	PID Segment::addPage() {
		return PID(id, pageCount++);
	}

}