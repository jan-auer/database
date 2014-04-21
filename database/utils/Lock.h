//
//  Locking.h
//  database
//
//  Created by Jan Michael Auer on 19/04/14.
//  Copyright (c) 2014 LightningSQL. All rights reserved.
//

#ifndef __database__Locking__
#define __database__Locking__

#include <pthread.h>
#include <algorithm>

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
		 * @return A return code as described in @c pthread_rwlock_rdlock().
		 */
		inline int lock(bool exclusive);

		/**
		 * Releases the previously acquired lock.
		 *
		 * If this thread holds no more locks on the specified object, another 
		 * waiting thread can acquire the lock. For more information, see @c 
		 * pthread_rwlock_unlock().
		 *
		 * @return A return code as described in @c pthread_rwlock_unlock().
		 */
		inline int unlock();

		/**
		 * Retrieves a handle to the internal lock object.
		 *
		 * @return A pointer to the initialized pthread_rwlock.
		 */
		pthread_rwlock_t* object();

	};

}

#include "Lock.cpp"

#endif /* defined(__database__Locking__) */
