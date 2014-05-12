#pragma once

#include <vector>
#include <string>

#include "Types.h"

namespace lsql {

	/**
	 *
	 */
	struct Attribute {

		std::string name;
		Types::Tag type;
		unsigned len;
		bool notNull;

		Attribute() : len(~0), notNull(true) {}

	};

	/**
	 *
	 */
	struct Relation {

		std::string name;
		std::vector<Attribute> attributes;
		std::vector<unsigned> primaryKey;

		uint16_t segment;
		uint32_t pageCount;

		Relation(const std::string& name) : name(name) {}

	};

	/**
	 *
	 */
	struct Schema {

		std::vector<Relation> relations;

		std::string toString() const;

	};

}
