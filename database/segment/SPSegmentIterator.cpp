//
//  SPSegmentIterator.cpp
//  database
//
//  Created by Jan Michael Auer on 14/06/14.
//  Copyright (c) 2014 LightningSQL. All rights reserved.
//

#include "SlottedPage.h"
#include "SPSegmentIterator.h"

namespace lsql {

	typedef SPSegment::Iterator SSI;

	SSI::Iterator(SPSegment* segment, uint32_t start)
	: segment(segment), pageId(start), page(nullptr), frame(nullptr) {
	}

	SSI::Iterator(const SSI& iterator) {
		*this = iterator;
	}

	SSI::~Iterator() {
		if (page != nullptr)
			segment->unfixPage(*frame, false);
	}

	SSI& SSI::operator=(const SSI& iterator) {
		segment = iterator.segment;
		pageId  = iterator.pageId;

		return *this;
	}

	bool SSI::operator==(const SSI& other) const {
		return pageId == other.pageId;
	}

	bool SSI::operator!=(const SSI& other) const {
		return !(*this == other);
	}

	SSI& SSI::operator++() {
		assert(segment != nullptr);

		if (page != nullptr) {
			segment->unfixPage(*frame, false);
			frame = nullptr;
			page = nullptr;
		}

		++pageId;
		return *this;
	}

	SlottedPage& SSI::operator*() const {
		return *operator->();
	}

	SlottedPage* SSI::operator->() const {
		if (page == nullptr) {
			SSI* that = const_cast<SSI*>(this);
			that->frame = &segment->fixPage(PID(segment->getID(), pageId), false);
			that->page = new SlottedPage(segment, *frame);
		}

		return page;
	}

}