//
//  Record.cpp
//  database
//
//  Created by Jan Michael Auer on 10/05/14.
//  Copyright (c) 2014 LightningSQL. All rights reserved.
//

#include <cassert>
#include <cstdlib>
#include <string>

#include "Record.h"

namespace lsql {

	Record::Record(Record&& other)
	: size(other.size), data(other.data) {
		other.data = nullptr;
		other.size = 0;
	}

	Record::Record(uint32_t size, const char* ptr) : size(size) {
		data = static_cast<char*>(malloc(size));
		if (data)
			std::memcpy(data, ptr, size);
	}

	Record::~Record() {
		free(data);
	}

	const char* Record::getData() const {
		return data;
	}

	uint32_t Record::getSize() const {
		return size;
	}

}