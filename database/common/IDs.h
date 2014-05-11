//
//  PageId.h
//  database
//
//  Created by Jan Michael Auer on 04/05/14.
//  Copyright (c) 2014 LightningSQL. All rights reserved.
//

#pragma once

#include <cstdint>

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

		/** */
		PID(uint64_t id);

		/** */
		PID(SID segment, uint32_t page);

		/** */
		const SID segment() const;

		/** */
		const uint32_t page() const;

	};

	/** */
	bool operator==(const PID& a, const PID& b);

	/** */
	bool operator!=(const PID& a, const PID& b);

	/**
	 *
	 */
	struct TID {

	};

}
