//
//  BufferQueue.cpp
//  database
//
//  Created by Felix Jankowski on 28.04.14.
//  Copyright (c) 2014 LightningSQL. All rights reserved.
//

#include <cassert>
#include "BufferQueue.h"

namespace lsql {
	

	template<typename Value>
	typename BufferQueue<Value>::Item* BufferQueue<Value>::createItemAndEnqueue(Value* value) {
		Item* item = new Item();
		item->value = value;
		
		insertItem(item);
		return item;
	}
	

	template<typename Value>
	void BufferQueue<Value>::enqueue(Item* item) {

		insertItem(item);
		return;
	}

	template<typename Value>
	typename BufferQueue<Value>::Item* BufferQueue<Value>::dequeue() {
		return remove(last);
	}

	template<typename Value>
	typename BufferQueue<Value>::Item* BufferQueue<Value>::dequeueIf(bool (*filter)(Value*)) {
		assert(filter != nullptr);

		mutex.lock();

		Item* found = nullptr;
		for (Item* item = last; item != nullptr; item = item->prev) {
			if (filter(item->value)) {
				found = item;
				break;
			}
		}
		
		if (found == nullptr) {
			mutex.unlock();
			return nullptr;
		}
		
		removeItem(found);
		mutex.unlock();
			
		return found;
	}
	
	template<typename Value>
	Value* BufferQueue<Value>::remove(Item* item) {
		assert(item != nullptr);
		
		mutex.lock();
		removeItem(item);
		mutex.unlock();
		
		Value* value = item->value;
		delete item;
		return value;
	}
	
	template<typename Value>
	void BufferQueue<Value>::moveFront(Item* item) {
		assert(item != nullptr);
		
		mutex.lock();
		removeItem(item);
		insertItem(item);
		mutex.unlock();
	}
	
	template<typename Value>
	void BufferQueue<Value>::insertItem(Item* item) {
		assert(item != nullptr);
		mutex.lock();
		
		item->prev = nullptr;
		item->next = first;
		
		first->prev = item;
		first = item;
		
		count++;
		mutex.unlock();
	}
	
	template<typename Value>
	void BufferQueue<Value>::removeItem(Item* item) {

		if (item->prev) {
			item->prev->next = item->next;
		} else {
			first = item->next;
		}
		
		if (item->next) {
			item->next->prev = item->prev;
		} else {
			last = item->prev;
		}
		
		count--;
	}

}
