//
//  Segment.cpp
//  database
//
//  Created by Jan Michael Auer on 14/05/14.
//  Copyright (c) 2014 LightningSQL. All rights reserved.
//

#include "Segment.h"

namespace lsql {

	Segment::Segment(uint16_t id, uint32_t pageCount)
	: id(id), pageCount(pageCount) {}

	uint16_t Segment::getID() const {
		return id;
	}

	uint32_t Segment::getPageCount() const {
		return pageCount;
	}

	PID Segment::addPage() {
		return PID(id, pageCount++);
	}

}