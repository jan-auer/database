//
//  SelectionOperator.cpp
//  database
//
//  Created by Felix Jankowski on 14.06.14.
//  Copyright (c) 2014 LightningSQL. All rights reserved.
//

#include "SelectionOperator.h"

#include <cassert>

using namespace lsql;

SelectionOperator::SelectionOperator(IOperator& in, uint32_t index, Register& constant) :
	in(in), index(index), constant(constant) {}


void SelectionOperator::open() {
	isOpen = true;
	in.open();
}


bool SelectionOperator::next() {
	assert(isOpen);

	while (in.next()) {
		std::vector<Register*> input = in.getOutput();
		
		if (*(input.at(index)) == constant)
			return true;
	}

	return false;
}


std::vector<Register*> SelectionOperator::getOutput() {
	return in.getOutput();
}


void SelectionOperator::rewind() {
	in.rewind();
}


void SelectionOperator::close() {
	isOpen = false;
	in.close();
}
