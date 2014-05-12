//
//  Record.h
//  database
//
//  Created by Jan Michael Auer on 10/05/14.
//  Copyright (c) 2014 LightningSQL. All rights reserved.
//

#pragma once

#include <cstdint>

namespace lsql {

	/**
	 * A simple record implementation.
	 */
	class Record {

		uint16_t len;
		char* data;

	public:

		// Assignment Operator: deleted
		Record& operator=(Record& rhs) = delete;

		// Copy Constructor: deleted
		Record(Record& t) = delete;

		// Move Constructor
		Record(Record&& t);

		// Constructor
		Record(uint16_t len, const char* ptr);

		// Destructor
		~Record();

		// Get pointer to data
		const char* getData() const;

		// Get data size in bytes
		uint16_t getLen() const;

	};

}
