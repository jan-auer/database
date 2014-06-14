//
//  SelectionOperator.cpp
//  database
//
//  Created by Felix Jankowski on 14.06.14.
//  Copyright (c) 2014 LightningSQL. All rights reserved.
//

#include <cassert>
#include "SelectionOperator.h"

namespace lsql {

	SelectionOperator::SelectionOperator(IOperator& in, uint16_t index, Register& constant)
	: in(in), index(index), constant(constant) {}

	void SelectionOperator::open() {
		assert(!isOpen);

		in.open();
		isOpen = true;
	}

	bool SelectionOperator::next() {
		assert(isOpen);

		while (in.next()) {
			Row input = in.getOutput();
			
			if (*(input.at(index)) == constant)
				return true;
		}

		return false;
	}

	Row SelectionOperator::getOutput() const {
		assert(isOpen);
		return in.getOutput();
	}

	void SelectionOperator::rewind() {
		assert(isOpen);
		in.rewind();
	}

	void SelectionOperator::close() {
		assert(isOpen);

		in.close();
		isOpen = false;
	}

}