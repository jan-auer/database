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
	 * 
	 */
	class SPSegment {

		BufferManager& bufferManager;
		Segment& segment;

	public:

		/**
		 *
		 */
		SPSegment(BufferManager& bufferManager, Segment& segment);

		/**
		 *
		 */
		Record lookup(TID id);

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

	private:

		/**
		 *
		 */
		BufferFrame& findFreeFrame(int32_t requestedSize, uint32_t startPage = 0);

	};

}
