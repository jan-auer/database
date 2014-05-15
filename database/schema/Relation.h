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
	 * Represents database relations (TABLE).
	 */
	struct Relation : public SPSegment {

		std::string name;
		std::vector<unsigned> primaryKey;
		std::vector<Attribute> attributes;

		/** Creates a new relation */
		Relation(BufferManager& bufferManager, uint16_t segmentId, uint32_t pageCount = 0)
		: SPSegment(bufferManager, segmentId, pageCount) {}

		/** Serialization constructor for relations. */
		Relation(BufferManager& bufferManager, uint16_t segmentId, uint32_t pageCount,
						 std::string&& name, std::vector<unsigned>&& primaryKey,
						 std::vector<Attribute>&& attributes)
		: Relation(bufferManager, segmentId, pageCount) {
			std::swap(this->name, name);
			std::swap(this->primaryKey, primaryKey);
			std::swap(this->attributes, attributes);
		}

		/** Returns the segment id of the relation. */
		const uint16_t segmentId() const {
			return Segment::id;
		}

		/** Returns the number of pages, this relations uses. */
		const uint32_t pageCount() const {
			return Segment::pageCount;
		}

	};

	namespace serialization {

		/**
		 * A serialization context to inject a buffer manager during deserialization.
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
				return Relation(context->bufferManager,
					deserialize_helper<uint16_t>::apply(begin,end),
					deserialize_helper<uint32_t>::apply(begin,end),
					std::move(deserialize_helper<std::string>::apply(begin,end)),
					std::move(deserialize_helper<std::vector<unsigned>>::apply(begin,end)),
					std::move(deserialize_helper<std::vector<Attribute>>::apply(begin,end)));
			}

		};
		
	}
	
}