//
//  BufferFrame.cpp
//  database
//
//  Created by Jan Michael Auer on 04/05/14.
//  Copyright (c) 2014 LightningSQL. All rights reserved.
//

#include <cstdlib>
#include <unistd.h>
#include <sys/sysctl.h>

#include "BufferFrame.h"

namespace lsql {

	size_t BufferFrame::SIZE = (size_t) sysconf(_SC_PAGESIZE);

	BufferFrame::BufferFrame(const PageId& id) : id(id), dirty(false), queue(QUEUE_NONE) {
		data = valloc(SIZE);
		assert(data != nullptr);

		tableNext = tablePrev = nullptr;
		queueNext = queuePrev = nullptr;
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
		return l.lock(exclusive);
	}

	bool BufferFrame::tryLock(bool exclusive) {
		return l.tryLock(exclusive);
	}

	bool BufferFrame::unlock() {
		return l.unlock();
	}

}
