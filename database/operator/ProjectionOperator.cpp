//
//  ProjectionOperator.cpp
//  database
//
//  Created by Felix Jankowski on 14.06.14.
//  Copyright (c) 2014 LightningSQL. All rights reserved.
//

#include "ProjectionOperator.h"

#include <cassert>


using namespace lsql;


ProjectionOperator::ProjectionOperator(IOperator& in, std::vector<uint32_t>& indices) :
in(in), indices(indices) {}


void ProjectionOperator::open() {
	in.open();
}


bool ProjectionOperator::next() {
	assert(isOpen);

	if (!in.next())
		return false;

	return true;
}


std::vector<Register*> ProjectionOperator::getOutput() {
	assert(isOpen);

	std::vector<Register*> output;
	std::vector<Register*> input = in.getOutput();

	for (auto &it : indices) {
		assert(it < input.size());
		output.push_back(input.at(it));
	}

	return output;
}


void ProjectionOperator::rewind() {
	in.rewind();
}


void ProjectionOperator::close() {
	isOpen = false;
	in.close();
}
