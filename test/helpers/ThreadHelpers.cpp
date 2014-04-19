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
		if (pthread_rwlock_trywrlock((pthread_rwlock_t*) arg) == 0)
			return (void*) 2;
		if (pthread_rwlock_tryrdlock((pthread_rwlock_t*) arg) != 0)
			return (void*) 1;

		return (void*) 0;
	}

	void* testUnlocked(void* arg) {
		return (void*) pthread_rwlock_trywrlock((pthread_rwlock_t*) arg);
	}

	void* testExclusiveLock(void* arg) {
		return (void*) (pthread_rwlock_tryrdlock((pthread_rwlock_t*) arg) == 0);
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
