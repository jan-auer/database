//
//  SchemaManager.h
//  database
//
//  Created by Jan Michael Auer on 10/05/14.
//  Copyright (c) 2014 LightningSQL. All rights reserved.
//

#pragma once

#include "buffer/BufferFrame.h"
#include "buffer/BufferManager.h"
#include "Schema.h"

namespace lsql {

	/**
	 *
	 */
	class SchemaManager {

		Schema schema;
		BufferManager& bufferManager;

	public:

		/**
		 * Creates a new schema manager instance and reads the schema from
		 * disk, stored in segment 0.
		 */
		SchemaManager(BufferManager& bufferManager);

		/**
		 * Destroys the schema manager and writes the schema to disk.
		 */
		~SchemaManager();

		/**
		 *
		 */
		Relation& lookup(const std::string& name);

		/**
		 *
		 */
		Relation& create(const std::string& name,
										 const std::vector<Attribute>& attributes,
										 const std::vector<unsigned>& primaryKey);

		/**
		 *
		 */
		bool drop(const std::string& name);

	};

}
