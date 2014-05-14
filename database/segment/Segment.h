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
#include "buffer/BufferManager.h"

namespace lsql {

	/**
	 * A wrapper class for segments.
	 */
	class Segment {

		BufferManager& bufferManager;

		uint16_t id;
		uint32_t pageCount;

	public:

		/**
		 * Creates a new segment.
		 * If this segment is new, pageCount should be left empty.
		 *
		 * @param bufferManager The buffer manager instance.
		 * @param id            The segment identifier.
		 * @param pageCount     The number of pages in this segment.
		 */
		Segment(BufferManager& bufferManager, uint16_t id, uint32_t pageCount = 0);

		/**
		 * Returns the identifier of this segment.
		 */
		uint16_t getID() const;

		/**
		 * Returns the number of pages in this segment.
		 */
		uint32_t getPageCount() const;

		/**
		 * Fixes the specified page in the underlying @c BufferManager instance. 
		 * This method might fail, if the requested page does not belong to this
		 * segment.
		 *
		 * @param id        An identifier for the page to load.
		 * @param exclusive Whether or not this frame is exclusive to the caller of 
		 *                  this method.
		 *
		 * @return A reference to the buffer frame. See @c BufferFrame for more details.
		 */
		BufferFrame& fixPage(PID id, bool exclusive);

		/**
		 * Releases the given frame. This method might fail, if the page does not
		 * belong to this segment.
		 */
		void unfixPage(BufferFrame& frame, bool isDirty);

		/**
		 * Adds a new page to this segment.
		 * This also increments the page count.
		 *
		 * @return The page id of the new page.
		 */
		PID addPage();

	};

}
