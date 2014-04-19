//
//  ConcurrentMap.cpp
//  database
//
//  Created by Jan Michael Auer on 17/04/14.
//  Copyright (c) 2014 LightningSQL. All rights reserved.
//

#include <assert.h>
#include <math.h>
#include "ConcurrentMap.h"

#define __cast(T, v) (*(T*)&(v))

namespace lsql {

	template<typename Key, typename Value>
	ConcurrentMap<Key, Value>::ConcurrentMap(off_t size) {
		assert(size > 0);
		// TODO: look for a better solution than log2(float)
		slotCount = off_t(1) << int(ceil(log2(size)));
		slots = new Slot[slotCount];
	}
	
	template<typename Key, typename Value>
	ConcurrentMap<Key, Value>::~ConcurrentMap() {
		delete[] slots;
	}
	
	template<typename Key, typename Value>
	bool ConcurrentMap<Key, Value>::contains(const Key& key, Lock** l) {
		Slot& slot = getSlot(key);
		
		lock(&slot.lock, false);
		Entry* found = *findRef(slot, key, l, false);
		unlock(&slot.lock);
		
		return found != nullptr;
	}
	
	template<typename Key, typename Value>
	Value* ConcurrentMap<Key, Value>::get(const Key& key, Lock** l, bool exclusive) {
		assert(!exclusive || lock != nullptr);
		
		Slot& slot = getSlot(key);

		lock(&slot.lock, false);
		Entry* entry = *findRef(slot, key, l, exclusive);
		unlock(&slot.lock);
		
		return (entry == nullptr) ? nullptr : &entry->value;
	}
	
	template<typename Key, typename Value>
	void ConcurrentMap<Key, Value>::insert(const Key& key, const Value& value, Lock** l) {
		assert(key == __cast(Key, value));
		
		Slot& slot = getSlot(key);
		lock(&slot.lock, true);
		
		Entry** ref = findRef(slot, key, l, true);
		assert(*ref == nullptr);
		
		*ref = new Entry(value, *ref);
		if (l != nullptr)
			lock(*l = &(*ref)->lock, true);
		
		unlock(&slot.lock);
	}
	
	template<typename Key, typename Value>
	void ConcurrentMap<Key, Value>::remove(const Key& key) {
		Slot& slot = getSlot(key);
		lock(&slot.lock, true);
		
		Lock* unused;
		Entry** ref = findRef(slot, key, &unused, true);
		Entry* entry = *ref;
		
		*ref = entry->next;
		delete entry;
		
		unlock(&slot.lock);
	}
	
	template<typename Key, typename Value>
	off_t ConcurrentMap<Key, Value>::hash(const Key& key) const {
		// TODO: try out crc, murmur, fnv1a instead of identity
		return key & (slotCount - 1);
	}
	
	template<typename Key, typename Value>
	typename ConcurrentMap<Key, Value>::Slot& ConcurrentMap<Key, Value>::getSlot(const Key& key) {
		return slots[hash(key)];
	}
	
	template<typename Key, typename Value>
	typename ConcurrentMap<Key, Value>::Entry**
	ConcurrentMap<Key, Value>::findRef(Slot& slot, const Key& key, Lock** l, bool exclusive) {
		Entry** ref = &slot.next;
		while (*ref != nullptr && key != __cast(Key, (*ref)->value))
			ref = &(*ref)->next;
		
		if (*ref != nullptr && l != 0)
			lock(*l = &(*ref)->lock, exclusive);
		
		return ref;
	}

}
