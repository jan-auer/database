//
//  PrintOperator.h
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
	 * The Print operator is initialized with an input operator
	 * and an output stream to which its next method writes the
	 * next tuple (if any) in a humanreadable format
	 */

	class PrintOperator : public IOperator {

		IOperator& in;
		std::ostream& os;

		bool isOpen = false;
		
	public:

		/**
		 * Creates a new Operator:
		 * Print: Prints out all input tuples in a human-readable format.
		 *
		 * @param op The next operator, from which the tuples shall be fetched.
		 * @param os The output stream the tuples are printed to in human read
		 *					 readable form. Default is std::cout.
		 */
		PrintOperator(IOperator& in, std::ostream& os = std::cout);

		// IOperator interface implementation.

		void open();
		bool next();
		Row getOutput() const = 0;
		void rewind();
		void close();
		
	};
	
}