//
//  PrintOperator.cpp
//  database
//
//  Created by Felix Jankowski on 14.06.14.
//  Copyright (c) 2014 LightningSQL. All rights reserved.
//

#include "PrintOperator.h"

#include <cassert>

using namespace lsql;

PrintOperator::PrintOperator(IOperator& in, std::ostream& os) : in(in), os(os) {}


void PrintOperator::open() {
	in.open();
	isOpen = true;
}


bool PrintOperator::next() {
	assert(isOpen);

	if (!in.next())
		return false;

	std::vector<Register*> v = in.getOutput();


	for(std::vector<Register*>::iterator it = v.begin(); it != v.end(); ++it) {
		os << *(*it) << " ";
	}

	os << std::endl;
	return true;
}


void PrintOperator::rewind() {
	in.rewind();
}


void PrintOperator::close() {
	isOpen = false;
	in.close();
}