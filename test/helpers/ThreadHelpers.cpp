//
//  ThreadHelpers.cpp
//  database
//
//  Created by Jan Michael Auer on 19/04/14.
//  Copyright (c) 2014 LightningSQL. All rights reserved.
//

#ifndef __unit_test__ThreadHelpers__
#define __unit_test__ThreadHelpers__

#include <pthread.h>

namespace lsql {
	namespace test {

		void* testSharedLock(void* arg) {
			auto lock = (pthread_rwlock_t*) arg;
			ssize_t rc = 0;

			if (pthread_rwlock_trywrlock(lock) == 0)
				rc = 1;
			if (pthread_rwlock_tryrdlock(lock) != 0)
				rc = 2;

			pthread_rwlock_unlock(lock);
			return (void*) rc;
		}

		void* testUnlocked(void* arg) {
			auto lock = (pthread_rwlock_t*) arg;
			ssize_t rc = pthread_rwlock_trywrlock(lock);

			pthread_rwlock_unlock(lock);
			return (void*) rc;
		}

		void* testExclusiveLock(void* arg) {
			auto lock = (pthread_rwlock_t*) arg;
			ssize_t rc = (pthread_rwlock_tryrdlock(lock) == 0);

			pthread_rwlock_unlock(lock);
			return (void*) rc;
		}

		ssize_t testThreadded(void* (*callback)(void *), void* arg) {
			pthread_t thread;
			void* ret;

			if (pthread_create(&thread, nullptr, callback, arg) != 0)
				return -1;
			if (pthread_join(thread, &ret) != 0)
				return -1;

			return (ssize_t)ret;
		}

	}
}

#endif /* defined(__unit_test__ThreadHelpers__) */
