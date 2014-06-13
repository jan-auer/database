//
//  Register.cpp
//  database
//
//  Created by Jan Michael Auer on 13/06/14.
//  Copyright (c) 2014 LightningSQL. All rights reserved.
//

#include <cassert>
#include <utility>
#include <string>
#include "Register.h"

namespace lsql {

	Type Register::getType() const {
		return type;
	}

	uint64_t Register::hash() const {
		switch (type) {
			case Type::Char:
				std::hash<Char> hashChar;
				return hashChar(data.c);

			case Type::Integer:
				std::hash<Integer> hashInt;
				return hashInt(data.i);

			default:
				return 0;
		}
	}

	Register& Register::operator=(const Char& c) {
		type = Type::Char;
		data.c = c;
		return *this;
	}

	Register& Register::operator=(const Integer& i) {
		type = Type::Integer;
		data.i = i;
		return *this;
	}

	std::ostream& operator<<(std::ostream& os, const Register& r) {
		switch (r.type) {
			case Type::Char:
				os << r.data.c;
				break;

			case Type::Integer:
				os << r.data.i;
				break;
		}

		return os;
	}

	bool operator<(const Register& a, const Register& b) {
		return compare(a, b) < 0;
	}
	
	bool operator==(const Register& a, const Register& b) {
		return compare(a, b) == 0;
	}

	int compare(const Register& a, const Register& b) {
		assert(a.type == b.type);

		if (a.type == Type::Char)
			return a.data.c.compare(b.data.c);

		if (a.type == Type::Integer)
			return (a.data.i < b.data.i) ? -1 : (a.data.i > b.data.i);

		assert(false);
	}
	
}