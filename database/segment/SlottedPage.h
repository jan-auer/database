//
//  SlottedPage.h
//  database
//
//  Created by Jan Michael Auer on 12/05/14.
//  Copyright (c) 2014 LightningSQL. All rights reserved.
//

#pragma once

#include <cstdint>

#include "buffer/BufferFrame.h"
#include "SPSegment.h"
#include "Record.h"

namespace lsql {

	class SlottedPage {

		/** Structure of the slotted page header. */
		struct Header {
			uint16_t count;
			int32_t dataStart;
			int32_t usedSpace;
		};

		/** States for page slots. */
		enum SlotType {
			SLOT_EMPTY = 0,
			SLOT_USED,
			SLOT_REDIRECT
		};

		/** Structure of a slot within the slotted page. */
		struct Slot {
			SlotType type;
			int32_t offset;
			int32_t size;
		};

		SPSegment* segment;

		PID pid;
		char* data;
		Header* header;
		Slot* slots;

	public:

		/**
		 * Creates a new slotted page.
		 *
		 * @param segment The owning segment of this page.
		 * @param frame   The corresponding buffer frame containing page data.
		 */
		SlottedPage(SPSegment* segment, BufferFrame& frame);

		/**
		 * Resets this page to an empty state.
		 *
		 * All counters are set to zero and all slots are removed. Due to performance
		 * reasons, this method might however leave data on the disk.
		 */
		void reset();

		/**
		 * Searches an existing record within this page.
		 *
		 * @param id The tuple identifier of the record to find.
		 */
		Record lookup(TID id) const;

		/**
		 * Creates a new slot within this page.
		 *
		 * Internally, this method will try to reuse previously freed slots as good
		 * as possible. If there are no free slots to recycle, a new slot is appended
		 * to the page.
		 *
		 * @return The tuple identifier of the new slot.
		 */
		TID createSlot();

		/**
		 * Inserts a new record into this page.
		 *
		 * The insertion is first tried by prepending the data to the current
		 * dataStart. If there is not enough space to prepend the data, will first
		 * be reorganized which moves all existing data to the very back of the page.
		 *
		 * If the slot specified by @c id is already used, it's entry is overwritten.
		 *
		 * @param id     The tuple identifier of the new record.
		 * @param record A record containing data to insert.
		 */
		void insert(TID id, const Record& record);

		/**
		 * Updates the specified record with new data.
		 *
		 * This method tries to update the specified record inplace first. If this
		 * is not possible but there is enough cumulated free space in this page,
		 * all data (excluding the old record) is compressed to the end of the page.
		 *
		 * Otherwise, the new record data must be redirected to a new page and the
		 * old record data is replaced with the redirected tuple identifier. If the
		 * record already has been redirected, this method fails with return value
		 * @c false. In this case, this redirected instance is removed and a new
		 * one is created in another page.
		 *
		 * @param id            The tuple identifier of the record to update.
		 * @param record        New data for the record.
		 * @param allowRedirect Whether or not this method might redirect in case
		 *                      there is not enough space in this page.
		 *
		 * @return True if the record could be updated, otherwise false.
		 */
		bool update(TID id, const Record& record, bool allowRedirect = true);

		/**
		 * Removes the specified record from this page.
		 *
		 * The corresponding slot is set to SLOT_EMPTY. If possible, this method
		 * tries to clean up unused slots and thus shrink the header. Furthermore,
		 * the dataStart pointer is updated, if necessary. In case the record entry
		 * is a redirect, the redirected record is removed recursively.
		 *
		 * This method fails if the slot specified by @c does not exist.
		 *
		 * @param id The tuple identifier of the record to remove.
		 */
		void remove(TID id);

		/**
		 * Resolves the amount of cumulated free space in this page.
		 */
		int32_t getFreeSpace() const;

	private:

		/**
		 * Returns a typed pointer to the data of a slot.
		 *
		 * @param DataType The type of data in the slot. Defaults to @c char.
		 * @param slot     The slot to get data for.
		 *
		 * @return A casted pointer to the slot data.
		 */
		template<typename DataType = char>
		DataType* getData(Slot& slot) const;

		/**
		 * Moves all data to the back of the page to maximize consecutive free space.
		 */
		void compressData();

		/**
		 * Updates the specified slot with the new record data inplace.
		 *
		 * This only works, if the new data is less or equal than the current data.
		 * Furthermore, slot and header counters are updated.
		 *
		 * @param slot   The slot to replace.
		 * @param record New data to replace the old one.
		 */
		void replaceRecord(Slot& slot, const Record& record);

	};

}
