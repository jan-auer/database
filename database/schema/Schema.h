//
//  Schema.h
//  database
//
//  Created by Felix Jankowski on 14.05.14.
//  Copyright (c) 2014 LightningSQL. All rights reserved.
//

#pragma once

#include <cstdint>
#include <vector>
#include <string>

#include "utils/Serialize.h"
#include "common/IDs.h"
#include "Relation.h"

namespace lsql {

	/**
	 * Represents a database schema.
	 */
	struct Schema {

		uint16_t segmentCount = 0;
		std::vector<Relation> relations;

		/** Creates a new Schema */
		Schema() {}

		/** Serialization constructor for schemata */
		Schema(uint16_t segmentCount, std::vector<Relation>&& r)
		: segmentCount(segmentCount) {
			std::swap(relations, r);
		}

	};

	namespace serialization {

		template <>
		struct get_size_helper<Schema> {

			static size_t value(const Schema& obj) {
				size_t size = get_size(obj.relations);
				return size;
			}

		};

		template <>
		struct serialize_helper<Schema> {

			static void apply(const Schema& obj, StreamType::iterator& res) {
				serializer(obj.segmentCount, res);
				serializer(obj.relations, res);
			}

		};

		template <>
		struct deserialize_helper<Schema, BufferContext> {

			static Schema apply(StreamType::const_iterator& begin,
													StreamType::const_iterator end,
													BufferContext* context = nullptr) {
				return Schema(deserialize_helper<uint16_t>::apply(begin, end),
					std::move(deserialize_helper<std::vector<Relation>, BufferContext>::apply(begin, end, context)));
			}

		};

	}

}
