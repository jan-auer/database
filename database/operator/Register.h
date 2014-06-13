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
#include "schema/Types.h"

namespace lsql {

	/**
	 *
	 */
	class Register {

		Type type;

		union {
			Char c;
			Integer i;
		} data;

	public:

		/**
		 *
		 */
		Type getType() const;

		/**
		 *
		 */
		uint64_t hash() const;

		/**
		 *
		 */
		Register& operator=(const Char& c);

		/**
		 *
		 */
		Register& operator=(const Integer& i);

		/**
		 *
		 */
		friend std::ostream& operator<<(std::ostream& os, const Register& r);

		/**
		 *
		 */
		friend bool operator<(const Register& a, const Register& b);

		/**
		 *
		 */
		friend bool operator==(const Register& a, const Register& b);

		/**
		 *
		 */
		friend int compare(const Register& a, const Register& b);

	};

}