//
//  BufferQueue.h
//  database
//
//  Created by Felix Jankowski on 28.04.14.
//  Copyright (c) 2014 LightningSQL. All rights reserved.
//

#ifndef __database__BufferQueue__
#define __database__BufferQueue__

#include "BufferFrame.h"

namespace lsql {
	
	template<typename Value>
	class Queue {
		
		uint64_t count = 0;
		Item* first;
		Item* last;
		
		Mutex mutex;
		
	public:
		
		struct Item {
			Value* value;
			Item* prev;
			Item* next;
		};
		
		Queue();
		
		~Queue();
		
		BufferQueue::Item* enqueue(Value* value);
		
		Value* dequeue();
		
		Value* dequeueIf(bool (*filter)(Value*));
		
		Value* remove(Item* item);
		
		void moveFront(Item* item);
		
	private:
		
		void insertItem(Item* item);
		
		void removeItem(Item* item);
		
	};
	
}
#include "BufferQueue.cpp"

#endif /* defined(__database__BufferQueue__) */
