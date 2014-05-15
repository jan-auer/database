#pragma once

#include <cstdint>
#include <vector>
#include <string>

#include "utils/Serialize.h"
#include "common/IDs.h"
#include "Relation.h"

namespace lsql {

	/**
	 *
	 */
	struct Schema {

		uint16_t segmentCount = 0;
		std::vector<Relation> relations;

		std::string toString() const;

	};

	/**
	 * Here comes the extension for the serializer:
	 */
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
				serializer(obj.relations, res);
			}

		};

		template <>
		struct deserialize_helper<Schema, BufferContext> {

			static Schema apply(StreamType::const_iterator& begin,
													StreamType::const_iterator end,
													BufferContext* context = nullptr) {
				Schema schema;
				schema.relations = deserialize_helper<std::vector<Relation>, BufferContext>::apply(begin, end, context);
				return schema;
			}

		};

	}

}
