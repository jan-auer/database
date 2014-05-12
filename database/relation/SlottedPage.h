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
			size_t dataStart;
		};

		struct Slot {
			TID id;
			bool redirect;
			size_t offset;
			size_t length;
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
		TID insert(const Record& record);

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
		size_t getFreeSpace() const;

	private:

		/**
		 *
		 */
		Slot* findSlot(TID id) const;

		/**
		 *
		 */
		void replaceRecord(Slot* slot, const Record& record);

		/**
		 *
		 */
		void moveData(size_t start, size_t length, ssize_t offset);

	};

}

#endif /* defined(__database__SlottedPage__) */
