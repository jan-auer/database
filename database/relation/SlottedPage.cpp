//
//  SlottedPage.cpp
//  database
//
//  Created by Jan Michael Auer on 12/05/14.
//  Copyright (c) 2014 LightningSQL. All rights reserved.
//

#include <cassert>

#include "SlottedPage.h"

namespace lsql {

	SlottedPage::SlottedPage(SPSegment* segment, BufferFrame& frame)
	: segment(segment), pid(frame.getId()) {
		data = static_cast<char*>(frame.getData());
		header = static_cast<Header*>(frame.getData());
		slots = reinterpret_cast<Slot*>(data + sizeof(Header));
	}

	void SlottedPage::reset() {
		header->count = 0;
		header->dataStart = BufferFrame::SIZE;
	}

	Record SlottedPage::lookup(TID id) const {
		Slot* slot = findSlot(id);
		assert(slot != nullptr);

		if (slot->redirect) {
			TID* redirectTID = reinterpret_cast<TID*>(data + slot->offset);
			return segment->lookup(*redirectTID);
		} else {
			return Record(slot->length, data + slot->offset);
		}
	}

	TID SlottedPage::insert(const Record& record) {
		// Find a free TID
		uint16_t tid = (header->count == 0) ? 0 : slots[header->count - 1].id.tuple() + 1;

		// Create a slot entry
		Slot* slot = &slots[header->count];
		slot->id = TID(pid.segment(), pid.page(), tid);
		slot->length = record.getLen();
		slot->offset = header->dataStart - slot->length;
		slot->redirect = false;

		// Copy data and update dataStart
		++header->count;
		header->dataStart = slot->offset;
		std::memcpy(data + slot->offset, record.getData(), record.getLen());

		return slot->id;
	}

	bool SlottedPage::update(TID id, const Record& record, bool allowRedirect) {
		Slot* slot = findSlot(id);
		assert(slot != nullptr);

		if (slot->redirect) {
			TID* redirectTID = reinterpret_cast<TID*>(data + slot->offset);
			bool updated = segment->update(*redirectTID, record, false);

			if (!updated) {
				segment->remove(*redirectTID);
				*redirectTID = segment->insert(record);
			}

		} else if (record.getLen() - slot->length <= getFreeSpace()) {
			replaceRecord(slot, record);

		} else if (allowRedirect) {
			slot->redirect = true;
			TID redirectTID = insert(record);
			Record redirectRecord(sizeof(TID), reinterpret_cast<char*>(&redirectTID));
			replaceRecord(slot, redirectRecord);

		} else {
			return false;
		}

		return true;
	}

	bool SlottedPage::remove(TID id) {
		// Find the desired slot within the page.
		Slot* slot = findSlot(id);
		assert(slot != nullptr);

		if (slot->redirect) {
			TID* redirectTID = reinterpret_cast<TID*>(data + slot->offset);
			return segment->remove(*redirectTID);
		}

		// Remove the data gap
		size_t removedSize = slot->length;
		moveData(header->dataStart, slot->offset - header->dataStart, removedSize);

		// Remove the slot
		Slot* next = slot + sizeof(Slot);
		Slot* last = &slots[header->count - 1];
		if (next <= last)
			std::memmove(slot, next, last - next);

		header->count--;
		return true;
	}

	size_t SlottedPage::getFreeSpace() const {
		return header->dataStart - sizeof(Header) - header->count * sizeof(Slot);
	}

	SlottedPage::Slot* SlottedPage::findSlot(TID id) const {
		Slot* slot = &slots[header->count - 1];
		while (slot >= slots && slot->id != id)
			slot -= sizeof(Slot);

		return (slot >= slots) ? slot : nullptr;
	}

	void SlottedPage::replaceRecord(Slot* slot, const Record& record) {
		// Move subsequent data
		ssize_t distance = slot->length - record.getLen();
		moveData(slot->offset, slot->offset - header->dataStart, distance);

		// Update the slot and data
		slot->length = record.getLen();
		std::memcpy(data + slot->offset, record.getData(), record.getLen());
	}

	void SlottedPage::moveData(size_t start, size_t length, ssize_t offset) {
		char* dataStart = data + header->dataStart;
		std::memmove(dataStart + offset, dataStart, length);
		header->dataStart += offset;

		for (Slot* slot = &slots[header->count - 1]; slot->offset <= start; slot -= sizeof(Slot))
			slot->offset += offset;
	}

}