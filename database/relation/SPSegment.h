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
#include "schema/Schema.h"
#include "Record.h"

namespace lsql {

	/**
	 *
	 */
	class SPSegment {

		struct Slot {
			TID id;
			size_t offset;
			size_t length;
		};

		struct Header {
			uint16_t count;
			size_t dataStart;
			Slot first;
		};

		BufferManager& bufferManager;
		Relation& relation;

	public:

		/**
		 *
		 */
		SPSegment(BufferManager& bufferManager, Relation& relation);

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
		bool update(TID id, const Record& record);

		/**
		 *
		 */
		bool remove(TID id);

	private:

		/**
		 *
		 */
		Header* getHeader(const BufferFrame& frame) const;

		/**
		 *
		 */
		Slot* getSlot(Header* header, uint16_t index) const;

		/**
		 *
		 */
		Slot* findSlot(const BufferFrame& frame, const TID& id) const;

		/**
		 *
		 */
		char* resolveData(const BufferFrame& frame, const Slot& slot) const;

		/**
		 *
		 */
		size_t computeFreeSpace(const BufferFrame& frame) const;

		/**
		 *
		 */
		BufferFrame& findFreeFrame(size_t requestedSize, uint32_t startPage = 0) const;

	};

}
