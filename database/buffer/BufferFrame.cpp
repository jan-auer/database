//
//  BufferFrame.cpp
//  database
//
//  Created by Jan Michael Auer on 17/04/14.
//  Copyright (c) 2014 LightningSQL. All rights reserved.
//

#include <stdlib.h>
#include <unistd.h>
#include <sys/sysctl.h>
#include "BufferFrame.h"

namespace lsql {

	bool operator==(const PageId& a, const PageId& b) {
		return a.id == b.id;
	}
	
	size_t BufferFrame::SIZE = (size_t) sysconf(_SC_PAGESIZE);
	
	BufferFrame::BufferFrame(const PageId& id) : id(id), dirty(false) {
		data = valloc(SIZE);
	}
	
	BufferFrame::~BufferFrame() {
		free(data);
	}
	
	const PageId& BufferFrame::getId() const {
		return id;
	}

	void* BufferFrame::getData() {
		return data;
	}
	
	bool BufferFrame::isDirty() const {
		return dirty;
	}
	
	void BufferFrame::setDirty() {
		dirty = true;
	}

	bool BufferFrame::lock(bool exclusive) {
		return l.lock(exclusive) == 0;
	}
	
	bool BufferFrame::unlock() {
		return l.unlock() == 0;
	}
	
}
