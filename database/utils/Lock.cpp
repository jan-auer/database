//
//  Locking.cpp
//  database
//
//  Created by Jan Michael Auer on 19/04/14.
//  Copyright (c) 2014 LightningSQL. All rights reserved.
//

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

	int Lock::lock(bool exclusive) {
		if (exclusive) {
			return pthread_rwlock_wrlock(&l);
		} else {
			return pthread_rwlock_rdlock(&l);
		}
	}
	
	int Lock::unlock() {
		return pthread_rwlock_unlock(&l);
	}

	pthread_rwlock_t* Lock::object() {
		return &l;
	}

}
