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
	class BufferQueue {

	public:
		
		struct Item {
			Value* value;
			Item* prev;
			Item* next;
		};
		
		/* BufferQueue();
		
		~BufferQueue(); */


		Item* createItemAndEnqueue(Value* value);
		
		void enqueue(Item* item);
		
		Item* dequeue();
		
		Item* dequeueIf(bool (*filter)(Value*));
		
		Value* remove(Item* item);
		
		void moveFront(Item* item);
		
	private:

		uint64_t count = 0;
		Item* first;
		Item* last;

		Mutex mutex;

		void insertItem(Item* item);
		
		void removeItem(Item* item);
		
	};
	
}
#include "BufferQueue.cpp"

#endif /* defined(__database__BufferQueue__) */
