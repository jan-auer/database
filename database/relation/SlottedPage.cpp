//
//  SlottedPage.cpp
//  database
//
//  Created by Jan Michael Auer on 12/05/14.
//  Copyright (c) 2014 LightningSQL. All rights reserved.
//

#include <cassert>
#include <algorithm>
#include <vector>

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
		header->dataStart = int32_t(BufferFrame::SIZE);
		header->usedSpace = 0;
	}

	Record SlottedPage::lookup(TID id) const {
		Slot& slot = getSlot(id);
		assert(slot.type != SLOT_EMPTY);

		if (slot.type == SLOT_REDIRECT) {
			TID* redirectTID = getData<TID>(slot);
			return segment->lookup(*redirectTID);
		} else {
			return Record(slot.size, getData(slot));
		}
	}

	TID SlottedPage::createSlot() {
		for (uint16_t i = 0; i < header->count; ++i)
			if (slots[i].type == SLOT_EMPTY)
				return TID(pid.segment(), pid.page(), i);

		uint16_t id = header->count++;
		return TID(pid.segment(), pid.page(), id);
	}

	bool SlottedPage::insert(TID id, const Record& record) {
		// Reset the slot for compression
		Slot& slot = getSlot(id);
		slot.type = SLOT_EMPTY;

		// Check if we need to compress data
		int32_t freeSpace = header->dataStart - sizeof(Header) - header->count * sizeof(Slot);
		if (int32_t(record.getSize()) > freeSpace)
			compressData();

		// Initialize the slot entry
		slot.type = SLOT_USED;
		slot.size = record.getSize();
		slot.offset = header->dataStart - slot.size;

		// Update header information
		header->dataStart = slot.offset;
		header->usedSpace += slot.size;

		// Copy data to the page
		std::memcpy(getData(slot), record.getData(), slot.size);
		return true;
	}

	bool SlottedPage::update(TID id, const Record& record, bool allowRedirect) {
		Slot& slot = getSlot(id);
		assert(slot.type != SLOT_EMPTY);

		// Delegate update to the redirected page.
		if (slot.type == SLOT_REDIRECT) {
			TID* redirectTID = getData<TID>(slot);
			bool updated = segment->update(*redirectTID, record, false);

			if (!updated) {
				// There was no space in the redirected page, so move the redirection
				// to a new page and remove the old record.
				segment->remove(*redirectTID);
				*redirectTID = segment->insert(record);
			}

		// Downsize the data slot
		} else if (int32_t(record.getSize()) <= slot.size) {
			replaceRecord(slot, record);

		// The current data slot might not fit, so reinsert with the same id
		} else if (int32_t(record.getSize()) <= getFreeSpace()) {
			insert(id, record);

		// There is no space in this page, so redirect to a new page
		} else if (allowRedirect) {
			slot.type = SLOT_REDIRECT;
			TID redirectTID = segment->insert(record);
			Record redirectRecord(sizeof(TID), reinterpret_cast<char*>(&redirectTID));
			replaceRecord(slot, redirectRecord);

		// There is no space but we must not redirect, so fail
		} else {
			return false;
		}

		return true;
	}

	bool SlottedPage::remove(TID id) {
		Slot& slot = getSlot(id);
		assert(slot.type != SLOT_EMPTY);

		// Also remove the redirected tuple.
		if (slot.type == SLOT_REDIRECT) {
			TID* redirectTID = getData<TID>(slot);
			return segment->remove(*redirectTID);
		}

		// Reset this slot
		slot.type = SLOT_EMPTY;
		header->usedSpace -= slot.size;

		// Remove all trailing empty slots
		int16_t i = header->count - 1;
		for (; i >= 0 && slots[i].type == SLOT_EMPTY; --i)
			header->count--;

		// Update dataStart according to the remaining slots
		for (; i >= 0; --i)
			if (slots[i].type != SLOT_EMPTY)
				header->dataStart = std::min(header->dataStart, slots[i].offset);

		return true;
	}

	int32_t SlottedPage::getFreeSpace() const {
		int32_t headerSize = sizeof(Header) + (header->count + 1) * sizeof(Slot);
		return int32_t(BufferFrame::SIZE) - header->usedSpace - headerSize;
	}

	SlottedPage::Slot& SlottedPage::getSlot(TID id) const {
		return slots[id.tuple()];
	}

	template<typename DataType>
	DataType* SlottedPage::getData(Slot& slot) const {
		return reinterpret_cast<DataType*>(data + slot.offset);
	}

	void SlottedPage::compressData() {
		std::vector<Slot*> orderedSlots;
		for (int16_t i = header->count - 1; i > 0; --i)
			if (slots[i].type != SLOT_EMPTY)
				orderedSlots.push_back(&slots[i]);

		// Order the slots after decreasing offsets
		std::sort(orderedSlots.begin(), orderedSlots.end(), [] (Slot* a, Slot* b) {
			return a->offset > b->offset;
		});

		// Move all data to the back of the page.
		header->dataStart = int32_t(BufferFrame::SIZE);
		for (Slot* slot : orderedSlots) {
			header->dataStart -= slot->size;
			std::memmove(data + header->dataStart, data + slot->offset, slot->size);
			slot->offset = header->dataStart;
		}
	}

	void SlottedPage::replaceRecord(Slot& slot, const Record& record) {
		header->usedSpace += int32_t(record.getSize()) - slot.size;
		std::memcpy(getData(slot), record.getData(), record.getSize());
		slot.size = record.getSize();
	}

}