//
//  PrintOperator.cpp
//  database
//
//  Created by Felix Jankowski on 14.06.14.
//  Copyright (c) 2014 LightningSQL. All rights reserved.
//

#include <cassert>
#include "PrintOperator.h"

namespace lsql {

	PrintOperator::PrintOperator(IOperator& in, std::ostream& os) : in(in), os(os) {}

	void PrintOperator::open() {
		assert(!isOpen);

		in.open();
		isOpen = true;
	}

	bool PrintOperator::next() {
		assert(isOpen);

		if (!in.next())
			return false;

		Row v = in.getOutput();


		for(Row::iterator it = v.begin(); it != v.end(); ++it) {
			os << *(*it) << " ";
		}

		os << std::endl;
		return true;
	}

	void PrintOperator::rewind() {
		assert(isOpen);
		in.rewind();
	}

	void PrintOperator::close() {
		assert(isOpen);

		in.close();
		isOpen = false;
	}

}