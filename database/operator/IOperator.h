//
//  IOperator.h
//  database
//
//  Created by Jan Michael Auer on 13/06/14.
//  Copyright (c) 2014 LightningSQL. All rights reserved.
//

#pragma once

#include <vector>
#include <Register.h>

namespace lsql {

	/**
	 *
	 */
	class IOperator {
	public:

		/**
		 *
		 */
		virtual void open() = 0;

		/**
		 *
		 */
		virtual bool next() = 0;

		/**
		 *
		 */
		virtual std::vector<Register*> getOutput() = 0;

		/**
		 *
		 */
		virtual void rewind() = 0;

		/**
		 *
		 */
		virtual void close() = 0;

	};

}