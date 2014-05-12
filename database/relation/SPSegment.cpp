//
//  SPSegment.cpp
//  database
//
//  Created by Jan Michael Auer on 10/05/14.
//  Copyright (c) 2014 LightningSQL. All rights reserved.
//

#include <cassert>
#include <string>

#include "SPSegment.h"

namespace lsql {

	SPSegment::SPSegment(BufferManager& bufferManager, Relation& relation)
	: bufferManager(bufferManager), relation(relation) {}

	Record SPSegment::lookup(TID id) const {
		BufferFrame& frame = bufferManager.fixPage(id, false);

		Slot* slot = findSlot(frame, id);
		assert(slot != nullptr);

		char* data = resolveData(frame, *slot);
		Record record(slot->length, data);

		bufferManager.unfixPage(frame, false);
		return record;
	}

	TID SPSegment::insert(const Record& record) {
		BufferFrame& frame = findFreeFrame(record.getLen());
		Header* header = getHeader(frame);

		// Find a free TID
		Slot* lastSlot = getSlot(header, header->count - 1);

		// Create a slot entry
		Slot* slot = getSlot(header, header->count);
		slot->id = TID(frame.getId().segment(), frame.getId().page(), lastSlot->id.tuple() + 1);
		slot->length = record.getLen();
		slot->offset = header->dataStart - slot->length;

		// Copy data and update dataStart
		++header->count;
		header->dataStart = slot->offset;
		std::memcpy(resolveData(frame, *slot), record.getData(), record.getLen());

		return slot->id;
	}

	bool SPSegment::update(TID id, const Record& record) {
		// TODO: implement
		return false;
	}

	bool SPSegment::remove(TID id) {
		// TODO: implement
		return false;
	}

	SPSegment::Header* SPSegment::getHeader(const BufferFrame& frame) const {
		return static_cast<Header*>(frame.getData());
	}

	SPSegment::Slot* SPSegment::getSlot(Header* header, uint16_t index) const {
		return &header->first + index * sizeof(Slot);
	}

	SPSegment::Slot* SPSegment::findSlot(const BufferFrame& frame, const TID& id) const {
		Header* header = getHeader(frame);

		Slot* slot = getSlot(header, header->count - 1);
		while (slot >= &header->first && slot->id != id)
			slot -= sizeof(Slot);

		return (slot >= &header->first) ? slot : nullptr;
	}

	char* SPSegment::resolveData(const BufferFrame& frame, const Slot& slot) const {
		return static_cast<char*>(frame.getData()) + slot.offset;
	}

	size_t SPSegment::computeFreeSpace(const BufferFrame& frame) const {
		Header* header = getHeader(frame);
		size_t headerSize = sizeof(Header) + header->count * sizeof(Slot);
		return header->dataStart - headerSize;
	}

	BufferFrame& SPSegment::findFreeFrame(size_t requestedSize, uint32_t startPage) const {
		// Try to find a frame with enough space
		for (uint32_t page = startPage; page <= relation.pageCount; ++page) {
			BufferFrame& frame = bufferManager.fixPage(PID(relation.segment, page), true);
			if (computeFreeSpace(frame) >= requestedSize)
				return frame;

			bufferManager.unfixPage(frame, false);
		}

		// No frame found, create a new one.
		PID id(relation.segment, relation.pageCount);
		BufferFrame& frame = bufferManager.fixPage(id, true);

		Header* header = getHeader(frame);
		header->count = 0;
		header->dataStart = BufferFrame::SIZE;

		return frame;
	}

}