//
//  SlottedPage.h
//  database
//
//  Created by Jan Michael Auer on 12/05/14.
//  Copyright (c) 2014 LightningSQL. All rights reserved.
//

#ifndef __database__SlottedPage__
#define __database__SlottedPage__

#include <cstdint>
#include <string>

#include "buffer/BufferFrame.h"
#include "SPSegment.h"
#include "Record.h"

namespace lsql {

	class SlottedPage {

		struct Header {
			uint16_t count;
			int32_t dataStart;
			int32_t usedSpace;
		};

		enum SlotType {
			SLOT_EMPTY = 0,
			SLOT_USED,
			SLOT_REDIRECT
		};

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
		 *
		 */
		SlottedPage(SPSegment* segment, BufferFrame& frame);

		/**
		 *
		 */
		void reset();

		/**
		 *
		 */
		Record lookup(TID id) const;

		/**
		 *
		 */
		TID createSlot();

		/**
		 *
		 */
		bool insert(TID id, const Record& record);

		/**
		 *
		 */
		bool update(TID id, const Record& record, bool allowRedirect = true);

		/**
		 *
		 */
		bool remove(TID id);

		/**
		 *
		 */
		int32_t getFreeSpace() const;

	private:

		/**
		 *
		 */
		Slot& getSlot(TID id) const;

		/**
		 *
		 */
		template<typename DataType = char>
		DataType* getData(Slot& slot) const;

		/**
		 *
		 */
		void compressData();

		/**
		 *
		 */
		void replaceRecord(Slot& slot, const Record& record);

	};

}

#endif /* defined(__database__SlottedPage__) */
