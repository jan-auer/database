//
//  Locking.h
//  database
//
//  Created by Jan Michael Auer on 19/04/14.
//  Copyright (c) 2014 LightningSQL. All rights reserved.
//

#pragma once

#include <pthread.h>
#include <algorithm>
#include <cassert>

namespace lsql {

	/**
	 * Wrapper class for read/write locks.
	 */
	class Lock {

		pthread_rwlock_t l;

	public:

		/**
		 * Creates and initializes a new lock.
		 */
		Lock();

		/**
		 * Destorys this lock.
		 *
		 * This method might fail, if there are still threads holding this lock.
		 */
		~Lock();

		/**
		 * Acquires the lock.
		 *
		 * If another thread currently holds a conflicting lock on the desired 
		 * entry, this functions blocks until the other thread releases its 
		 * lock. Conflicts are:
		 *  1. An exclusive lock, if @c exclusive is false.
		 *  2. Any lock mode, if @c exclusive is true.
		 *
		 * @param exclusive Whether or not the lock should be exclusive.
		 * @return True if the lock could be acquired; otherwise false.
		 */
		bool lock(bool exclusive);

		/**
		 * Tries to acquire the lock.
		 *
		 * If another thread currently holds a conflicting lock on the desired
		 * entry, this function returns false. Conflicts are:
		 *  1. An exclusive lock, if @c exclusive is false.
		 *  2. Any lock mode, if @c exclusive is true.
		 *
		 * @param exclusive Whether or not the lock should be exclusive.
		 * @return True if the lock could be acquired; otherwise false.
		 */
		bool tryLock(bool exclusive);

		/**
		 * Releases the previously acquired lock.
		 *
		 * If this thread holds no more locks on the specified object, another 
		 * waiting thread can acquire the lock. For more information, see @c 
		 * pthread_rwlock_unlock().
		 *
		 * @return True if the lock could be released; otherwise false.
		 */
		bool unlock();

		/**
		 * Retrieves a handle to the internal lock object.
		 *
		 * @return A pointer to the initialized pthread_rwlock.
		 */
		pthread_rwlock_t* object();

	};

}
