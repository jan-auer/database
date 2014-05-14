//
//  SchemaManager.cpp
//  database
//
//  Created by Jan Michael Auer on 10/05/14.
//  Copyright (c) 2014 LightningSQL. All rights reserved.
//

#include "SchemaManager.h"
#include "Serialize.h"

#include <sstream>      //std::stringstream
#include <stdio.h>			//memcpy


namespace lsql {

	SchemaManager::SchemaManager(BufferManager& bufferManager) :
	bufferManager(bufferManager) {
		BufferFrame& frame = bufferManager.fixPage(0, false);
		char* frameData = reinterpret_cast<char*>(frame.getData());

		std::vector<uint8_t> serializedData;
		serializedData.assign(frameData, frameData + BufferFrame::SIZE - 1);
		schema = deserialize<Schema> (serializedData);

		bufferManager.unfixPage(frame, false);

	}


	SchemaManager::~SchemaManager() {

		BufferFrame& frame = bufferManager.fixPage(0, true);

		std::vector<uint8_t> serializedVector;
		serialize(schema, serializedVector);

		std::memcpy(frame.getData(), serializedVector.data(), serializedVector.size() * sizeof(char));

		bufferManager.unfixPage(frame, true);

	}


	Schema& SchemaManager::getSchema() {
		return schema;
	}


	Relation& SchemaManager::lookupRelation(const std::string& name) {

		std::vector<Relation> rels = schema.relations;

		typename std::vector<Relation>::iterator it = rels.begin();
		for( ; it != rels.end(); ++it) {

			if (it->name == name)
				return *it;

		}

		// relation not found, create new one.
		// ToDo: Throw Exception
		return createRelation(name, std::vector<Attribute>(), std::vector<unsigned>());
	}

	Relation& SchemaManager::createRelation(const std::string name, std::vector<Attribute> attributes, std::vector<unsigned int> primaryKey) {

		schema.relations.emplace_back(name, schema.segmentCount++, attributes, primaryKey);

		Relation& rel = schema.relations.back();
		return rel;
	}

	bool SchemaManager::dropRelation(const std::string name) {

		typename std::vector<Relation>::iterator it = schema.relations.begin();

		for( ; it != schema.relations.end(); ++it) {
			if (it->name != name)
				continue;

			schema.relations.erase(it);
			return true;
		}
		
		return false;
	}

}
