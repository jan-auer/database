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
#include <sstream>

#include "utils/File.h"
#include "BufferFrame.h"

namespace lsql {

	size_t BufferFrame::SIZE = BUFFER_FRAME_SIZE * (size_t) sysconf(_SC_PAGESIZE);

	BufferFrame::BufferFrame(const PageId& id)
	: id(id), dirty(false), queue(QUEUE_NONE) {
		tableNext = tablePrev = nullptr;
		queueNext = queuePrev = nullptr;

		// use valloc over malloc for aligned memory pages.
		data = valloc(SIZE);
		assert(data != nullptr);
	}

	BufferFrame::BufferFrame(const PageId& id, BufferFrame&& unused)
	: id(id), dirty(false), queue(QUEUE_NONE) {
		tableNext = tablePrev = nullptr;
		queueNext = queuePrev = nullptr;

		data = unused.getData();
		unused.invalidate();
	}

	BufferFrame::~BufferFrame() {
		if (data != nullptr) {
			save();
			free(data);
		}
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

	bool BufferFrame::load() {
		std::string fileName = std::to_string(id.segment());
		File<void> file(fileName, false);

		return file.read(data, SIZE, id.page() * SIZE);
	}

	bool BufferFrame::save() {
		if (!dirty)
			return true;

		std::string fileName = std::to_string(id.segment());
		File<void> file(fileName, true);

		return file.write(data, SIZE, id.page() * SIZE);
		dirty = false;
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

	void BufferFrame::invalidate() {
		this->data = nullptr;
	}

}
