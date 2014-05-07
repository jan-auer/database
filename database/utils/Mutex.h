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

		bool tryLock();

		bool lock();

		bool unlock();

		/**
		 * Retrieves a handle to the internal lock object.
		 *
		 * @return A pointer to the initialized pthread_rwlock.
		 */
		pthread_mutex_t* object();

	};


}

#include "Mutex.cpp"


#endif /* defined(__database__Mutex__) */
