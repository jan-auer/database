//
//  Segment.h
//  database
//
//  Created by Jan Michael Auer on 14/05/14.
//  Copyright (c) 2014 LightningSQL. All rights reserved.
//

#pragma once

#include <cstdint>
#include "common/IDs.h"

namespace lsql {

	/**
	 * A simple class for segments.
	 *
	 * TODO: Review whether this should also handle fixPage by wrapping BufferManager.
	 */
	class Segment {

		uint16_t id;
		uint32_t pageCount;

	public:

		/**
		 * Creates a new segment.
		 * If this segment is new, pageCount should be left empty.
		 *
		 * @param id        The segment identifier.
		 * @param pageCount The number of pages in this segment.
		 */
		Segment(uint16_t id, uint32_t pageCount = 0);

		/**
		 * Returns the identifier of this segment.
		 */
		uint16_t getID() const;

		/**
		 * Returns the number of pages in this segment.
		 */
		uint32_t getPageCount() const;

		/**
		 * Adds a new page to this segment.
		 * This also increments the page count.
		 *
		 * @return The page id of the new page.
		 */
		PID addPage();

	};

}
