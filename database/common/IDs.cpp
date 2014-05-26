//
//  IDs.cpp
//  database
//
//  Created by Jan Michael Auer on 11/05/14.
//  Copyright (c) 2014 LightningSQL. All rights reserved.
//

#include <climits>
#include "IDs.h"

namespace lsql {

	const int TUPLE_POS   = 0;
	const int PAGE_POS    = TUPLE_POS + CHAR_BIT * sizeof(uint16_t);
	const int SEGMENT_POS = PAGE_POS + CHAR_BIT * sizeof(uint32_t);

	PID::PID(uint64_t id) : id(id) {}

	PID::PID(uint16_t segment, uint32_t page) {
		id = 0 | (uint64_t(segment) << SEGMENT_POS) | (uint64_t(page) << PAGE_POS);
	}

	uint16_t PID::segment() const {
		return uint16_t(id >> SEGMENT_POS);
	}

	uint32_t PID::page() const {
		return uint32_t(id >> PAGE_POS);
	}

	std::ostream& operator<<(std::ostream& os, const PID& pid) {
//		os << "PID";
		os << pid.id;
		return os;
	}

	bool operator==(const PID& a, const PID& b) {
		return (a.id >> PAGE_POS) == (b.id >> PAGE_POS);
	}

	bool operator!=(const PID& a, const PID& b) {
		return !(a == b);
	}

	TID::TID(uint64_t id) : PID(id) {}

	TID::TID(uint16_t segment, uint32_t page, uint16_t tuple)
	: PID(segment, page) {
		id |= uint64_t(tuple) << TUPLE_POS;
	}

	uint16_t TID::tuple() const {
		return uint16_t(id >> TUPLE_POS);
	}

	TID TID::operator=(const PID& pid) {
		id = pid.id;
		return *this;
	}

	/* std::ostream& operator<<(std::ostream& os, const TID& tid) {
		os << tid.id;
		//os << "p" << tid.page() << "s" << tid.segment() << "t" << tid.tuple();
		return os;
	} */

	bool operator==(const TID& a, const TID& b) {
		return a.id == b.id;
	}

	bool operator!=(const TID& a, const TID& b) {
		return !(a == b);
	}

}
