//
//  ConcurrentList.cpp
//  database
//
//  Created by Jan Michael Auer on 04/05/14.
//  Copyright (c) 2014 LightningSQL. All rights reserved.
//

#include "ConcurrentList.h"

namespace lsql {

	template<typename Item, typename AccessPolicy>
	ConcurrentList<Item, AccessPolicy>::ConcurrentList()
	: first(nullptr), last(nullptr) {
	}

	template<typename Item, typename AccessPolicy>
	ConcurrentList<Item, AccessPolicy>::~ConcurrentList() {
	}

	template<typename Item, typename AccessPolicy>
	Item* ConcurrentList<Item, AccessPolicy>::getFirst() const {
		return first;
	}

	template<typename Item, typename AccessPolicy>
	Item* ConcurrentList<Item, AccessPolicy>::getLast() const {
		return last;
	}

	template<typename Item, typename AccessPolicy>
	void ConcurrentList<Item, AccessPolicy>::append(Item* item) {
		assert(item != nullptr);
		lock(true);

		prev(item) = last;
		next(item) = nullptr;

		if (last) {
			next(last) = item;
		} else {
			first = item;
		}

		last = item;

		unlock();
	}
	
	template<typename Item, typename AccessPolicy>
	void ConcurrentList<Item, AccessPolicy>::prepend(Item* item) {
		assert(item != nullptr);
		lock(true);

		prev(item) = nullptr;
		next(item) = first;

		if (first) {
			prev(first) = item;
		} else {
			last = item;
		}

		first = item;

		unlock();
	}
	
	template<typename Item, typename AccessPolicy>
	void ConcurrentList<Item, AccessPolicy>::bringFront(Item* item) {
		assert(item != nullptr);

		if (item == first)
			return;

		lock(true);

		if (item == last)
			last = prev(item);

		if (prev(item))
			next(prev(item)) = next(item);
		if (next(item))
			prev(next(item)) = prev(item);

		next(item) = first;
		prev(item) = nullptr;

		prev(first) = item;
		first = item;

		unlock();
	}
	
	template<typename Item, typename AccessPolicy>
	void ConcurrentList<Item, AccessPolicy>::remove(Item* item) {
		assert(item != nullptr);
		lock(true);

		if (item == first)
			first = next(item);
		if (item == last)
			last = prev(item);

		if (prev(item))
			next(prev(item)) = next(item);
		if (next(item))
			prev(next(item)) = prev(item);

		next(item) = nullptr;
		prev(item) = nullptr;

		unlock();
	}

	template<typename Item, typename AccessPolicy>
	void ConcurrentList<Item, AccessPolicy>::empty() {
		lock(true);
		first = last = nullptr;
		unlock();
	}

	template<typename Item, typename AccessPolicy>
	void ConcurrentList<Item, AccessPolicy>::cleanup() {
		lock(true);

		Item* item = first;
		while (item != nullptr) {
			Item* old = item;
			item = next(item);
			delete old;
		}

		first = last = nullptr;
		unlock();
	}

	template<typename Item, typename AccessPolicy>
	bool ConcurrentList<Item, AccessPolicy>::lock(bool exclusive) {
		return l.lock(exclusive);
	}

	template<typename Item, typename AccessPolicy>
	bool ConcurrentList<Item, AccessPolicy>::unlock() {
		return l.unlock();
	}

}