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
#include "Lock.h"

using namespace lsql;

namespace lsql {
namespace test {

	struct LockTest : public testing::Test {
		Lock* l;

		virtual void SetUp() {
			l = new Lock();
		}

		virtual void TearDown() {
			pthread_rwlock_unlock(l->object());
			delete l;
		}
	};


	TEST_F(LockTest, LocksShared) {
		l->lock(false);
		EXPECT_EQ(0, testThreadded(&testSharedLock, (void*) l->object()));
	}

	TEST_F(LockTest, LocksExclusive) {
		l->lock(true);
		EXPECT_EQ(0, testThreadded(&testExclusiveLock, (void*) l->object()));
	}

	TEST_F(LockTest, UnlocksShared) {
		pthread_rwlock_rdlock(l->object());
		
		l->unlock();
		EXPECT_EQ(0, testThreadded(&testUnlocked, (void*) l->object()));
	}

	TEST_F(LockTest, UnlocksExclusive) {
		pthread_rwlock_wrlock(l->object());
		
		l->unlock();
		EXPECT_EQ(0, testThreadded(&testUnlocked, (void*) l->object()));
	}

}
}
