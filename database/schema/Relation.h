//
//  Relation.h
//  database
//
//  Created by Felix Jankowski on 14.05.14.
//  Copyright (c) 2014 LightningSQL. All rights reserved.
//

#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "utils/Serialize.h"
#include "buffer/BufferManager.h"
#include "segment/SPSegment.h"
#include "Attribute.h"

namespace lsql {

	/**
	 *
	 */
	struct Relation : public SPSegment {

		std::string name;
		std::vector<unsigned> primaryKey;
		std::vector<Attribute> attributes;

		Relation(BufferManager& bufferManager, uint16_t segmentId, uint32_t pageCount = 0)
		: SPSegment(bufferManager, segmentId, pageCount) {}

		const uint16_t segmentId() const {
			return Segment::id;
		}

		const uint32_t pageCount() const {
			return Segment::pageCount;
		}

	};

	/**
	 * Here comes the extension for the serializer:
	 */
	namespace serialization {

		/**
		 *
		 */
		struct BufferContext {
			BufferManager& bufferManager;
			BufferContext(BufferManager& bm) : bufferManager(bm) {}
		};

		template <>
		struct get_size_helper<Relation> {

			static size_t value(const Relation& obj) {
				size_t size = get_size(obj.name);
				size += get_size(obj.segmentId());
				size += get_size(obj.pageCount());
				size += get_size(obj.attributes);
				size += get_size(obj.primaryKey);
				return size;
			}

		};

		template <>
		struct serialize_helper<Relation> {

			static void apply(const Relation& obj, StreamType::iterator& res) {
				serializer(obj.segmentId(), res);
				serializer(obj.pageCount(), res);
				serializer(obj.name,        res);
				serializer(obj.primaryKey,  res);
				serializer(obj.attributes,  res);
			}

		};

		template <>
		struct deserialize_helper<Relation, BufferContext> {

			static Relation apply(StreamType::const_iterator& begin,
														StreamType::const_iterator end,
														BufferContext* context = nullptr) {
				auto segment    = deserialize_helper<uint16_t>::apply(begin,end);
				auto pageCount  = deserialize_helper<uint32_t>::apply(begin,end);

				Relation rel(context->bufferManager, segment, pageCount);
				rel.name       = deserialize_helper<std::string>::apply(begin,end);
				rel.primaryKey = deserialize_helper<std::vector<unsigned>>::apply(begin,end);
				rel.attributes = deserialize_helper<std::vector<Attribute>>::apply(begin,end);

				return rel;
			}

		};
		
	}
	
}