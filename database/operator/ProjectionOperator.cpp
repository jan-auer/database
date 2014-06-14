//
//  ProjectionOperator.cpp
//  database
//
//  Created by Felix Jankowski on 14.06.14.
//  Copyright (c) 2014 LightningSQL. All rights reserved.
//

#include <cassert>
#include "ProjectionOperator.h"

namespace lsql {

	ProjectionOperator::ProjectionOperator(IOperator& in, std::vector<uint16_t>& indices)
	: in(in), indices(indices) {}

	void ProjectionOperator::open() {
		assert(!isOpen);
		in.open();
	}

	bool ProjectionOperator::next() {
		assert(isOpen);
		return in.next();
	}

	Row ProjectionOperator::getOutput() const {
		assert(isOpen);

		Row output;
		Row input = in.getOutput();

		for (auto index : indices) {
			assert(index < input.size());
			output.push_back(input[index]);
		}

		return output;
	}

	void ProjectionOperator::rewind() {
		assert(isOpen);
		in.rewind();
	}

	void ProjectionOperator::close() {
		assert(isOpen);

		in.close();
		isOpen = false;
	}

}