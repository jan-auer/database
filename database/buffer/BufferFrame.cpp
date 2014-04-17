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
	
	size_t BufferFrame::SIZE = (size_t) sysconf(_SC_PAGESIZE);
	
	BufferFrame::BufferFrame(PID& id) : id(id), dirty(false) {
		data = malloc(SIZE);
	}
	
	BufferFrame::~BufferFrame() {
		free(data);
	}
	
	const PID& BufferFrame::getId() const {
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
	
}