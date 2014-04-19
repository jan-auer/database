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

namespace lsql {

	// An internal type mapping for locks.
	typedef pthread_rwlock_t Lock;
	
	/**
	 * Acquires the given lock.
	 *
	 * If another thread currently holds a conflicting lock on the
	 * desired entry, this functions blocks until the other thread
	 * releases its lock. Conflicts are:
	 *  1. An exclusive lock, if @c exclusive is false.
	 *  2. Any lock, if @c exclusive is true.
	 *
	 * @param lock      The pointer to a lock object.
	 * @param exclusive Whether or not the lock should be exclusive.
	 *
	 * @return A return code as described in @c pthread_rwlock_rdlock().
	 */
	int lock(Lock* lock, bool exclusive);
	
	/**
	 * Releases a previously acquired lock. 
	 *
	 * If this thread holds no more locks on the specified object,
	 * another waiting thread can acquire the lock. For more 
	 * information, see @c pthread_rwlock_unlock().
	 *
	 * @todo This does not seem to work according to unit tests.
	 *
	 * @param lock The pointer to a lock object.
	 * @return A return code as described in @c pthread_rwlock_unlock().
	 */
	int unlock(Lock* lock);
	
}

#include "Locking.cpp"

#endif /* defined(__database__Locking__) */
