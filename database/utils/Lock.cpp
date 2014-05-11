//
//  Locking.cpp
//  database
//
//  Created by Jan Michael Auer on 19/04/14.
//  Copyright (c) 2014 LightningSQL. All rights reserved.
//

#include <cassert>
#include "Lock.h"

namespace lsql {

	Lock::Lock() {
		int rc = pthread_rwlock_init(&l, nullptr);
		assert(rc == 0);
	}

	Lock::~Lock() {
		int rc = pthread_rwlock_destroy(&l);
		assert(rc == 0);
	}

	bool Lock::lock(bool exclusive) {
		if (exclusive) {
			return pthread_rwlock_wrlock(&l) == 0;
		} else {
			return pthread_rwlock_rdlock(&l) == 0;
		}
	}

	bool Lock::tryLock(bool exclusive) {
		if (exclusive) {
			return pthread_rwlock_trywrlock(&l) == 0;
		} else {
			return pthread_rwlock_tryrdlock(&l) == 0;
		}
	}

	bool Lock::unlock() {
		return pthread_rwlock_unlock(&l) == 0;
	}

	pthread_rwlock_t* Lock::object() {
		return &l;
	}

}
