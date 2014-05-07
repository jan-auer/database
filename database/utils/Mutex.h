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

	/**
	 * Wrapper class for mutex locks.
	 */
	class Mutex {

		pthread_mutex_t m;

	public:

		/**
		 * Creates and initializes a new mutex.
		 */
		Mutex();

		/**
		 * Destorys this mutex.
		 *
		 * This method might fail, if there are still threads holding this mutex.
		 */
		~Mutex();

		/**
		 * Acquires the mutex.
		 *
		 * If another thread currently holds a lock on the desired mutex, this 
		 * function blocks until the other thread releases its lock.
		 *
		 * @return True if the mutex could be acquired; otherwise false.
		 */
		bool lock();

		/**
		 * Tries to acquires the mutex.
		 *
		 * If another thread currently holds a lock on the desired mutex, this
		 * function returns false.
		 *
		 * @return True if the mutex could be acquired; otherwise false.
		 */
		bool tryLock();

		/**
		 * Releases the previously acquired mutex.
		 *
		 * If this thread holds no more locks on the specified object, another
		 * waiting thread can acquire the mutex. For more information, see @c
		 * pthread_mutex_unlock().
		 *
		 * @return True if the mutex could be released; otherwise false.
		 */
		bool unlock();

		/**
		 * Retrieves a handle to the internal mutex object.
		 *
		 * @return A pointer to the initialized pthread_rwlock.
		 */
		pthread_mutex_t* object();

	};

}

#include "Mutex.cpp"


#endif /* defined(__database__Mutex__) */
