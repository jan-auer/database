//
//  Attribute.h
//  database
//
//  Created by Felix Jankowski on 14.05.14.
//  Copyright (c) 2014 LightningSQL. All rights reserved.
//

#pragma once

namespace lsql {
	struct Attribute {

		std::string name;
		Types::Tag type;
		uint32_t len;
		bool notNull;

		Attribute() : len(~0), notNull(true) {}

	};

	static bool operator== (Attribute& a1, Attribute& a2) {
		if (a1.name == a2.name &&
				a1.type == a2.type &&
				a1.len == a2.len &&
				a1.notNull == a2.notNull
				)
			return true;
		return false;
	}

	static bool operator!= (Attribute& a1, Attribute& a2) {
		return !(a1 == a2);
	}

	/**
	 * Here comes the extension for the serializer:
	 */
	namespace detail {

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
														 StreamType::const_iterator end)
			{
				Attribute attr = Attribute();

				attr.name			= deserialize_helper<std::string>::apply(begin,end);
				attr.type			= deserialize_helper<Types::Tag>::apply(begin,end);
				attr.len			= deserialize_helper<uint32_t>::apply(begin,end);
				attr.notNull	= deserialize_helper<bool>::apply(begin,end);
				return attr;
			}
		};
	}

}