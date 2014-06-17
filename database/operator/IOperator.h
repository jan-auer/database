//
//  IOperator.h
//  database
//
//  Created by Jan Michael Auer on 13/06/14.
//  Copyright (c) 2014 LightningSQL. All rights reserved.
//

#pragma once

#include <vector>

#include "Register.h"

namespace lsql {


	/** 
	 * A database tuple (row) is a vector of pointers to register objects.
	 * For simplification, this type is defined.
	 */
	typedef std::vector<Register*> Row;

	
	/**
	 * This is an abstract class, defining the interface of
	 * the database operator classes.
	 *
	 * Description of the methods is based on assignment 5.
	 */
	class IOperator {
	public:

		/**
		 * Open the operator
		 */
		virtual void open() = 0;

		/**
		 * Produce the next tuple
		 */
		virtual bool next() = 0;

		/**
		 * Get all produced values
		 */
		virtual Row getOutput() const = 0;

		/**
		 * Rewind is an addition, p.ex. to be used for a nested loop join
		 */
		virtual void rewind() = 0;

		/**
		 * Close the operator
		 */
		virtual void close() = 0;

	};

}