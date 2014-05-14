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
	 * Represents a record in the database containing data.
	 *
	 * Unlike in the database, this record is immutable. To change the value of
	 * a record in the database, create a new record and call @c SPSegment::update
	 * with the corresponding TID. See @c SPSegment for mor information.
	 */
	class Record {

		uint32_t size;
		char* data;

	public:

		/** Assignment Operator: deleted */
		Record& operator=(Record& rhs) = delete;

		/** Copy Constructor: deleted */
		Record(Record& t) = delete;

		/**
		 * Creates a new record.
		 *
		 * @param size The size of the record in bytes.
		 * @param data A pointer to the data of @c size bytes.
		 */
		Record(uint32_t size, const char* data);

		/**
		 * Takes over the data of the specified @c old record.
		 * The internal is detached from the old record and not copied.
		 *
		 * @param old An old record containing data.
		 */
		Record(Record&& old);

		/**
		 * Destroys this record and deletes the internal data.
		 * If this record was moved, no data is deleted.
		 */
		~Record();

		/**
		 * Returns a pointer to the record data or @c nullptr if moved.
		 */
		const char* getData() const;

		/**
		 * Returns the size of this record in bytes.
		 */
		uint32_t getSize() const;

	};

}
