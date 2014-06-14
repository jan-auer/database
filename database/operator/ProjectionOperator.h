//
//  ProjectionOperator.h
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
	 * The Projection operator is initialized with an input operator
	 * and a list of register IDs (indexes into the register vector)
	 * it should project to
	 */

	class ProjectionOperator : public IOperator {

		IOperator& in;
		bool isOpen = false;
		std::vector<uint16_t>& indices;

	public:

		/**
		 * Creates a new Operator:
		 * Projection: Projects to a subset of the input schema.
		 *
		 * @param op The next operator, from which the tuples shall be fetched
		 */
		ProjectionOperator(IOperator& in, std::vector<uint16_t>& indices);

		// IOperator interface implementation.

		void open();
		bool next();
		Row getOutput() const;
		void rewind();
		void close();
		
	};
	
}