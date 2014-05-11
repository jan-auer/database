//
//  IDs.cpp
//  database
//
//  Created by Jan Michael Auer on 11/05/14.
//  Copyright (c) 2014 LightningSQL. All rights reserved.
//

#include "IDs.h"

namespace lsql {

	PID::PID(uint64_t id) : id(id) {}

	PID::PID(SID segment, uint32_t page) {
		id = uint64_t(segment) << 32 | page;
	}

	const SID PID::segment() const {
		return SID(id >> 32);
	}

	const uint32_t PID::page() const {
		return uint32_t(id);
	}

	bool operator==(const PID& a, const PID& b) {
		return a.id == b.id;
	}

	bool operator!=(const PID& a, const PID& b) {
		return !(a == b);
	}

}
