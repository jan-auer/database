//
//  Register.h
//  database
//
//  Created by Jan Michael Auer on 13/06/14.
//  Copyright (c) 2014 LightningSQL. All rights reserved.
//

#pragma once

#include <ostream>
#include <iostream>
#include <utility>

#include "schema/Types.h"

namespace lsql {

	/**
	 * Operator results are passed as register objects. Each contains one
	 * value - a tuple is a vector of pointers to a register object.
	 */
	
	class Register {

		/**
		 * SQL type of the value, as defined in schema/Types.h
		 */
		Type type;


		/**
		 * The result value. 
		 * A union is used, as the value may be of different types, but
		 * can only be one type at the same time per object.
		 */
		union Value {
			Char c;
			Integer i;

			Value() {}
			Value(Char c) : c(c) {}
			Value(Integer i) : i(i) {}
			~Value() {}
		} data;

	public:

		/**
		 * Copy constructor.
		 * Constructs a new register object, with identical values 
		 * as to the provided one.
		 */
		Register(const Register& r);

		/**
		 * Constructs a new register object with a value of type Char
		 */
		Register(Char c);

		/**
		 * Constructs a new register object with a value of type Integer
		 */
		Register(Integer i);

		/**
		 * Returns the type of the register object, as defined in schema/Types.h
		 *
		 * @return The type of the value contained in the object
		 */
		Type getType() const;

		/**
		 * Creates a hash of the contained value
		 *
		 * @return The type of the value contained in the object
		 */
		uint64_t hash() const;

		/**
		 * Assignment operator, also used in the constructors
		 */
		Register& operator=(const Register& r);

		/**
		 * Assignment operator for assigning Char values*
		 * to the register object
		 */
		Register& operator=(const Char& c);

		/**
		 * Assignment operator for assigning Integer values 
		 *  to the register object
		 */
		Register& operator=(const Integer& i);

		/**
		 * Pipe operator for printing the values of the register object
		 * to a stream.
		 */
		friend std::ostream& operator<<(std::ostream& os, const Register& r);

		/**
		 * Compare operator, used for selections.
		 * Compares the values of two register objects.
		 */
		friend bool operator<(const Register& a, const Register& b);

		/**
		 * Compare operator, used for selections.
		 * Checks the values of two register objects for identity.
		 */
		friend bool operator==(const Register& a, const Register& b);

		/**
		 * Compare function for comparing the values of two register objects.
		 *
		 * @return -1 if a < b, 0 if a = b and 1 if a > b
		 */
		friend int compare(const Register& a, const Register& b);

	};

}

namespace std {

	/**
	 * Specialization of std::hash() for register objects.
	 * hash(Register) now calls register.hash()
	 */

	template<>
	struct hash<lsql::Register> {

		size_t operator()(const lsql::Register& r) const {
			return r.hash();
		}

	};

}
