//
//  SlottedPageIterator.h
//  database
//
//  Created by Jan Michael Auer on 14/06/14.
//  Copyright (c) 2014 LightningSQL. All rights reserved.
//

#pragma once

#include "Record.h"
#include "SlottedPage.h"

namespace lsql {

	/**
	 *
	 */
	class SlottedPage::Iterator {

		SlottedPage* page;
		Slot* slot;
		Record* record;

	public:

		/**
		 *
		 */
		explicit Iterator(SlottedPage* page = nullptr, uint16_t start = 0);

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
		Record& operator*() const;

		/**
		 *
		 */
		Record* operator->() const;

	};

}
