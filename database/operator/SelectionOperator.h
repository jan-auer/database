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

	class SelectionOperator : protected IOperator {

		IOperator& in;
		bool isOpen = false;
		uint32_t index;
		Register& constant;

	public:

		/**
		 * Creates a new Operator:
		 * Selection: Implements predicates of the form 􏰀i ai = ci where
		 *             ai are attributes and ci are constants.
		 *
		 * @param in The next operator, from which the tuples shall be fetched
		 */
		SelectionOperator(IOperator& in, uint32_t index, Register& constant);

		/**
		 *
		 */
		~SelectionOperator();

		/**
		 *
		 */
		virtual void open();

		/**
		 *
		 */
		virtual bool next();

		/**
		 *
		 */
		virtual std::vector<Register*> getOutput();

		/**
		 *
		 */
		virtual void rewind();

		/**
		 *
		 */
		virtual void close();
		
	};
	
}