//
//  SPSegmentIterator.h
//  database
//
//  Created by Jan Michael Auer on 14/06/14.
//  Copyright (c) 2014 LightningSQL. All rights reserved.
//

#pragma once

#include "SlottedPage.h"
#include "SPSegment.h"

namespace lsql {

	class SlottedPage;

	class SPSegment::Iterator {

		SPSegment* segment;
		uint32_t pageId;
		BufferFrame* frame;
		SlottedPage* page;

	public:

		/**
		 *
		 */
		explicit Iterator(SPSegment* segment = nullptr, uint32_t start = 0);

		/**
		 *
		 */
		Iterator(const Iterator& iterator);

		/**
		 *
		 */
		~Iterator();

		/**
		 *
		 */
		Iterator& operator=(const Iterator& iterator);

		/**
		 *
		 */
		bool operator==(const Iterator& other) const;

		/**
		 *
		 */
		bool operator!=(const Iterator& other) const;

		/**
		 *
		 */
		Iterator& operator++();

		/**
		 *
		 */
		SlottedPage& operator*() const;

		/**
		 *
		 */
		SlottedPage* operator->() const;

	};

}