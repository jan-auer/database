//
//  SelectionOperator.h
//  database
//
//  Created by Felix Jankowski on 14.06.14.
//  Copyright (c) 2014 LightningSQL. All rights reserved.
//

#pragma once

#include <vector>
#include <ostream>
#include <iostream>

#include "Register.h"
#include "IOperator.h"


namespace lsql {

	/**
	 * The Selection operator is initialized with an input operator,
	 * a register ID and a constant
	 */
	class SelectionOperator : public IOperator {

		IOperator& in;
		uint32_t index;
		Register& constant;

		bool isOpen = false;

	public:

		/**
		 * Creates a new Operator:
		 * Selection: Implements predicates of the form ^i ai = ci where
		 *             ai are attributes and ci are constants.
		 *
		 * @param in The next operator, from which the tuples shall be fetched
		 * @param index The index of the value that shall be compared to
		 *							the constant
		 * @param constant The comparison value
		 */
		SelectionOperator(IOperator& in, uint16_t index, Register& constant);

		// IOperator interface implementation.

		void open();
		bool next();
		Row getOutput() const;
		void rewind();
		void close();
		
	};
	
}
