//
//  SlottedPageIterator.cpp
//  database
//
//  Created by Jan Michael Auer on 14/06/14.
//  Copyright (c) 2014 LightningSQL. All rights reserved.
//

#include <cassert>
#include "SlottedPageIterator.h"

namespace lsql {

	typedef SlottedPage::Iterator SPI;

	SPI::Iterator(SlottedPage* page, uint16_t start)
	: page(page), slot(&page->slots[start]), record(nullptr) {
	}

	SPI::Iterator(const SPI& iterator) {
		*this = iterator;
	}

	SPI::~Iterator() {
		if (record != nullptr)
			delete record;
	}

	SPI& SPI::operator=(const SPI& iterator) {
		page = iterator.page;
		slot = iterator.slot;
		record = nullptr;

		return *this;
	}

	bool SPI::operator==(const SPI& other) const {
		return slot == other.slot;
	}

	bool SPI::operator!=(const SPI& other) const {
		return !(*this == other);
	}

	SPI& SPI::operator++() {
		if (record != nullptr)
			delete record;

		++slot;
		return *this;
	}

	Record& SPI::operator*() const {
		return *this->operator->();
	}

	Record* SPI::operator->() const {
		assert(slot - page->slots < page->header->count);

		if (record == nullptr) {
			const_cast<SPI*>(this)->record = new Record(slot->size, page->getData(*slot));
		}

		return record;
	}

}