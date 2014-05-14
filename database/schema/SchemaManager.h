//
//  SchemaManager.h
//  database
//
//  Created by Jan Michael Auer on 10/05/14.
//  Copyright (c) 2014 LightningSQL. All rights reserved.
//

#pragma once

#include "Schema.h"
#include "BufferManager.h"
#include "BufferFrame.h"

namespace lsql {

	/**
	 *
	 */
	class SchemaManager {

	private:
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
		Schema& getSchema();

		/**
		 *  throw exception??
		 */
		Relation& lookupRelation(const std::string& name);

		/**
		 *
		 */
		Relation& createRelation(const std::string name, std::vector<Attribute> attributes, std::vector<unsigned> primaryKey);

		/**
		 *
		 */
		bool dropRelation(const std::string name);

	};

}
