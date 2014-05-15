//
//  Attribute.h
//  database
//
//  Created by Felix Jankowski on 14.05.14.
//  Copyright (c) 2014 LightningSQL. All rights reserved.
//

#pragma once

#include <cstdint>
#include <string>

#include "utils/Serialize.h"
#include "Types.h"

namespace lsql {

	/**
	 * Represents database attributes.
	 */
	struct Attribute {

		std::string name;
		Type type;
		uint32_t len;
		bool notNull;

		/** Creates a new attribute */
		Attribute() : len(~0), notNull(true) {}

		/** Serialization constructor for attributes. */
		Attribute(std::string&& name, Type&& type, uint32_t len, bool notNull)
		: len(len), notNull(notNull) {
			std::swap(this->name, name);
			std::swap(this->type, type);
		};

	};

	namespace serialization {

		template <>
		struct get_size_helper<Attribute> {

			static size_t value(const Attribute& obj) {
				size_t size = get_size(obj.name);
				size += get_size(obj.type);
				size += get_size(obj.len);
				size += get_size(obj.notNull);
				return size;
			}

		};

		template <>
		struct serialize_helper<Attribute> {

			static void apply(const Attribute& obj, StreamType::iterator& res) {
				serializer(obj.name, res);
				serializer(obj.type, res);
				serializer(obj.len, res);
				serializer(obj.notNull, res);
			}

		};

		template <>
		struct deserialize_helper<Attribute> {

			static Attribute apply(StreamType::const_iterator& begin,
														 StreamType::const_iterator end,
														 void* context = nullptr) {
				return Attribute(
					std::move(deserialize_helper<std::string>::apply(begin,end)),
					std::move(deserialize_helper<Type>::apply(begin,end)),
					deserialize_helper<uint32_t>::apply(begin,end),
					deserialize_helper<bool>::apply(begin,end));
			}

		};

	}

}