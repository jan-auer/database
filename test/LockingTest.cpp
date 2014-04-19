//
//  LockingTest.cpp
//  database
//
//  Created by Jan Michael Auer on 19/04/14.
//  Copyright (c) 2014 LightningSQL. All rights reserved.
//

#include <iostream>
#include "gtest/gtest.h"
#include "helpers/ThreadHelpers.cpp"
#include "Locking.h"

using namespace lsql;

namespace lsql {
namespace test {

	TEST(LockingTest, LocksShared) {
		Lock l = PTHREAD_RWLOCK_INITIALIZER;
		
		lock(&l, false);
		ssize_t ret = testThreadded(&testSharedLock, (void*) &l);
		
		pthread_rwlock_destroy(&l);
		EXPECT_EQ(0, ret);
	}

	TEST(LockingTest, LocksExclusive) {
		Lock l = PTHREAD_RWLOCK_INITIALIZER;
		
		lock(&l, true);
		ssize_t ret = testThreadded(&testExclusiveLock, (void*) &l);
		
		pthread_rwlock_destroy(&l);
		EXPECT_EQ(0, ret);
	}

	TEST(LockingTest, UnlocksShared) {
		Lock l = PTHREAD_RWLOCK_INITIALIZER;
		pthread_rwlock_rdlock(&l);
		
		unlock(&l);
		ssize_t ret = testThreadded(&testUnlocked, (void*) &l);

		EXPECT_EQ(0, ret);
	}

	TEST(LockingTest, UnlocksExclusive) {
		Lock l = PTHREAD_RWLOCK_INITIALIZER;
		pthread_rwlock_wrlock(&l);
		
		unlock(&l);
		ssize_t ret = testThreadded(&testUnlocked, (void*) &l);

		EXPECT_EQ(0, ret);
	}

}
}
