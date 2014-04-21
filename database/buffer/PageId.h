//
//  PageId.h
//  database
//
//  Created by Jan Michael Auer on 21/04/14.
//  Copyright (c) 2014 LightningSQL. All rights reserved.
//

#ifndef database_PageId_h
#define database_PageId_h

/**
 * A page id containing the unique segment identifier and the page identifier
 * which is unique within the segment.
 *
 * The memory layout allows the PageId to be casted from and to uint64_t. 
 * Additionally, an implicit constructor from uint64_t allows quick conversions.
 */
struct PageId {

	uint64_t id;

	PageId(uint64_t id) : id(id) {}

	PageId(uint32_t segment = 0, uint32_t page = 0) {
		id = uint64_t(segment) << 32 | page;
	}

	const uint32_t segment() const {
		return uint32_t(id >> 32);
	}

	const uint32_t page() const {
		return uint32_t(id);
	}

};

#endif
