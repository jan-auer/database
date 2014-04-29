//
//  Mutex.h
//  database
//
//  Created by Felix Jankowski on 28.04.14.
//  Copyright (c) 2014 LightningSQL. All rights reserved.
//

#ifndef __database__Mutex__
#define __database__Mutex__

#include <pthread.h>

namespace lsql {
  
	
	class Mutex {
		pthread_mutex_t m;
		
	public:
		Mutex();
		
		~Mutex();
		
		int lock();
		
		int unlock();
		
	};
	

}

#include "Mutex.cpp"

#endif /* defined(__database__Mutex__) */
