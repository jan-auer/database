//
//  SchemaManager.cpp
//  database
//
//  Created by Jan Michael Auer on 10/05/14.
//  Copyright (c) 2014 LightningSQL. All rights reserved.
//

#include <cstdio>
#include <sstream>

#include "Serialize.h"
#include "SchemaManager.h"

namespace lsql {

	SchemaManager::SchemaManager(BufferManager& bufferManager) :
	bufferManager(bufferManager) {
		BufferFrame& frame = bufferManager.fixPage(0, false);
		char* frameData = reinterpret_cast<char*>(frame.getData());

		detail::RelationContext context(bufferManager);
		std::vector<uint8_t> serializedData;
		serializedData.assign(frameData, frameData + BufferFrame::SIZE - 1);
		schema = deserialize<Schema, detail::RelationContext>(serializedData, &context);

		bufferManager.unfixPage(frame, false);

	}

	SchemaManager::~SchemaManager() {
		BufferFrame& frame = bufferManager.fixPage(0, true);

		std::vector<uint8_t> serializedVector;
		serialize(schema, serializedVector);

		std::memcpy(frame.getData(), serializedVector.data(), serializedVector.size());
		bufferManager.unfixPage(frame, true);
	}

	Relation& SchemaManager::lookup(const std::string& name) {
		for (auto& relation : schema.relations) {
			if (relation.name == name)
				return relation;
		}

		// ToDo: Throw Exception?
		// Relation not found, create a new one
		return create(name, std::vector<Attribute>(), std::vector<unsigned>());
	}

	Relation& SchemaManager::create(const std::string& name, const std::vector<Attribute>& attributes, const std::vector<unsigned>& primaryKey) {
		uint16_t segmentId = schema.segmentCount++;

		schema.relations.emplace_back(name, segmentId, attributes, primaryKey);

		Relation& rel = schema.relations.back();
		return rel;
	}

	bool SchemaManager::drop(const std::string& name) {
		for (auto it = schema.relations.begin(); it != schema.relations.end(); ++it) {
			if (it->name != name)
				continue;

			schema.relations.erase(it);
			return true;
		}

		return false;
	}

}
