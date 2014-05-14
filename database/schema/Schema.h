#pragma once

#include <vector>
#include <string>
#include <iostream>

#include "Types.h"
#include "common/IDs.h"
#include "utils/Serialize.h"

#include "SPSegment.h"

#include "Attribute.h"
#include "Relation.h"

namespace lsql {

	/**
	 *
	 */
	struct Schema {

		std::vector<Relation> relations;

		std::string toString() const;

		uint16_t segmentCount = 0;

	};

	/**
	 * Here comes the extension for the serializer:
	 */
	namespace detail {

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
		struct deserialize_helper<Schema> {

			static Schema apply(StreamType::const_iterator& begin,
														StreamType::const_iterator end)
			{
				Schema schema = Schema();

				schema.relations		= deserialize_helper<std::vector<Relation>>::apply(begin,end);
				return schema;
			}
		};
	}
}
