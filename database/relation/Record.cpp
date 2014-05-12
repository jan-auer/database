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

	Record::Record(Record&& t) : len(t.len), data(t.data) {
		t.data = nullptr;
		t.len = 0;
	}

	Record::Record(uint16_t len, const char* ptr) : len(len) {
		data = static_cast<char*>(malloc(len));
		if (data)
			std::memcpy(data, ptr, len);
	}

	Record::~Record() {
		free(data);
	}

	const char* Record::getData() const {
		return data;
	}

	uint16_t Record::getLen() const {
		return len;
	}

}