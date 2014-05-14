#pragma once

#include <vector>
#include <string>
#include <iostream>

#include "Types.h"
#include "common/IDs.h"
#include "utils/Serialize.h"

namespace lsql {

#pragma mark Attribute
	/**
	 *
	 */
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


#pragma mark - Relation
	/**
	 *
	 */
	struct Relation {

		std::string name;
		uint16_t segment;
		uint32_t pageCount;

		std::vector<Attribute> attributes;
		std::vector<unsigned> primaryKey;


		Relation(const std::string& name) : name(name) {}
		Relation() : name(std::string()) {}

	};

	/**
	 * Here comes the extension for the serializer:
	 */
	namespace detail {

		template <>
		struct get_size_helper<Relation> {
			static size_t value(const Relation& obj) {
				size_t size = get_size(obj.name);
				size += get_size(obj.segment);
				size += get_size(obj.pageCount);
				size += get_size(obj.attributes);
				size += get_size(obj.primaryKey);
				return size;
			}
		};

		template <>
		struct serialize_helper<Relation> {

			static void apply(const Relation& obj, StreamType::iterator& res) {
				serializer(obj.name, res);
				serializer(obj.segment, res);
				serializer(obj.pageCount, res);
				serializer(obj.attributes, res);
				serializer(obj.primaryKey, res);
			}

		};

		template <>
		struct deserialize_helper<Relation> {

			static Relation apply(StreamType::const_iterator& begin,
														 StreamType::const_iterator end)
			{
				//Relation rel = Relation(deserialize_helper<std::string>::apply(begin,end));

				std::string name = deserialize_helper<std::string>::apply(begin,end);
				Relation rel = Relation(name);

				rel.segment		= deserialize_helper<uint16_t>::apply(begin,end);
				rel.pageCount	= deserialize_helper<uint32_t>::apply(begin,end);
				rel.attributes	= deserialize_helper<std::vector<Attribute>>::apply(begin,end);
				rel.primaryKey	= deserialize_helper<std::vector<unsigned>>::apply(begin,end);
				return rel;
			}
		};
	}


#pragma mark - Schema


	/**
	 *
	 */
	struct Schema {

		std::vector<Relation> relations;

		std::string toString() const;

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
