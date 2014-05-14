//
//  SPSegment.h
//  database
//
//  Created by Jan Michael Auer on 10/05/14.
//  Copyright (c) 2014 LightningSQL. All rights reserved.
//

#pragma once

#include <vector>

#include "common/IDs.h"
#include "buffer/BufferManager.h"
#include "Segment.h"
#include "Record.h"

namespace lsql {

	/**
	 * A wrapper class for segments containing slotted pages.
	 *
	 * SPSegment handles CRUD for records in slotted pages. All operations within
	 * a page are delegated to @c SlottedPage.
	 */
	class SPSegment {

		BufferManager& bufferManager;
		Segment& segment;

	public:

		/**
		 * Creates a wrapper for segments containing slotted pages.
		 *
		 * @param bufferManager The buffer manager instance.
		 * @param segment       The internal segment.
		 */
		SPSegment(BufferManager& bufferManager, Segment& segment);

		/**
		 * Searches an existing record within the segment.
		 *
		 * @param id The tuple identifier of the record to find.
		 */
		Record lookup(TID id);

		/**
		 * Inserts a new record into the segment.
		 * Once inserted, the tuple identifier of the record will never change.
		 *
		 * @param record A record containing data to insert.
		 * @return The tuple identifier of the new record.
		 */
		TID insert(const Record& record);

		/**
		 * Updates the specified record with new data.
		 * The tuple identifier is guaranteed to be constant, i.e. not change.
		 *
		 * @param id            The tuple identifier of the record to update.
		 * @param record        New data for the record.
		 * @param allowRedirect For internal use only.
		 *
		 * @return True if the record could be updated, otherwise false.
		 */
		bool update(TID id, const Record& record, bool allowRedirect = true);

		/**
		 * Removes the specified record from this page.
		 *
		 * @return True if the record could be updated, otherwise false.
		 */
		bool remove(TID id);

	private:

		/**
		 * Searches for a page which will fit the given size.
		 *
		 * @param requestedSize The minimum of free space in the page.
		 * @param startPage     The page to start searching from.
		 */
		BufferFrame& findFreeFrame(int32_t requestedSize, uint32_t startPage = 0);

	};

}
