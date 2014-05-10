//
//  PageId.h
//  database
//
//  Created by Jan Michael Auer on 04/05/14.
//  Copyright (c) 2014 LightningSQL. All rights reserved.
//

#pragma once

namespace lsql {

	typedef uint32_t SID;

	/**
	 * A page id containing the unique segment identifier and the page identifier
	 * which is unique within the segment.
	 *
	 * The memory layout allows the PageId to be casted from and to uint64_t.
	 * Additionally, an implicit constructor from uint64_t allows quick conversions.
	 */
	struct PID {

		uint64_t id;

		PID(uint64_t id) : id(id) {}

		PID(SID segment, uint32_t page) {
			id = uint64_t(segment) << 32 | page;
		}

		const SID segment() const {
			return SID(id >> 32);
		}

		const uint32_t page() const {
			return uint32_t(id);
		}

	};

	bool operator==(const PID& a, const PID& b) {
		return a.id == b.id;
	}

	bool operator!=(const PID& a, const PID& b) {
		return !(a == b);
	}

	/**
	 *
	 */
	struct TID {

	};

}
