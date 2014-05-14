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

namespace lsql {

	/**
	 *
	 */
	class SchemaManager {

	private:
		Schema schema;
		uint64_t segmentCount;
		BufferManager& bufferManager;

		/**
		 * Creates a new schema manager instance and reads the schema from
		 * disk, stored in segment 0.
		 */
		SchemaManager(BufferManager& bufferManager);


		/**
		 * Destroys the schema manager and writes the schema to disk.
		 */
		~SchemaManager();

	public:
		/**
		 *
		 */
		Schema& getSchema() const;

		/**
		 *
		 */
		Relation& findRelation(const std::string& name) const;

		/**
		 *
		 */
		Relation& createRelation(const std::string name, std::vector<Attribute> attributes, std::vector<unsigned> primaryKey);


		bool alterRelationAddAttribute(Relation& relation, Attribute attribute);
		bool alterRelationAlterAttribute(Relation& relation, std::string attributeName, Attribute attribute);
		bool alterRelationDropAttribute(Relation& relation, std::string attributeName);

		bool alterRelationPrimaryKey(Relation& relation, std::vector<unsigned>);


		/**
		 *
		 */
		bool dropRelation(const std::string name);

	};

}
