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

	/**
	 * A page id containing the unique segment identifier and the page identifier
	 * which is unique within the segment.
	 *
	 * The memory layout allows the PageId to be casted from and to uint64_t.
	 * Additionally, an implicit constructor from uint64_t allows quick conversions.
	 */
	struct PID {

		uint64_t id;

		/** Creates a new PID from the given 64 bit number. */
		PID(uint64_t id);

		/** Creates a new PID. */
		PID(uint16_t segment, uint32_t page);

		/** Returns the segment number. */
		uint16_t segment() const;

		/** Returns the page number. */
		uint32_t page() const;

	};

	/** Compares two PIDs for equality. */
	bool operator==(const PID& a, const PID& b);

	/** Compares two PIDs for inequality. */
	bool operator!=(const PID& a, const PID& b);

	/**
	 * A tuple identifier containing the unique segment and page identifiers, as 
	 * well as the tuple number within the page.
	 *
	 * The memory layout allows the TID to be casted from and to uint64_t. 
	 * Additionally, the implicit constructor from uint64_t allows quick conversions.
	 */
	struct TID : public PID {

		/** Creates a new TID from the given 64 bit number. */
		TID(uint64_t id);

		/** Creates a new TID. */
		TID(uint16_t segment, uint32_t page, uint16_t tuple);

		/** Returns the tuple number. */
		uint16_t tuple() const;

	};

	/** Compares two TIDs for equality. */
	bool operator==(const TID& a, const TID& b);

	/** Compares two TIDs for inequality. */
	bool operator!=(const TID& a, const TID& b);

	const TID NULL_TID(0,0,0);
	const PID NULL_PID(0,0);
}
