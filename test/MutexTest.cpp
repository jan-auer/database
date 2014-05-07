//
//  MutexTest.cpp
//  database
//
//  Created by Felix Jankowski on 07.05.14.
//  Copyright (c) 2014 LightningSQL. All rights reserved.
//

#include "helpers/ThreadHelpers.cpp"
#include "utils/Mutex.h"

namespace lsql {
namespace test {

	struct MutexTest : public testing::Test {
		Mutex* m;

		virtual void SetUp() {
			m = new Mutex();
		}

		virtual void TearDown() {
			pthread_mutex_unlock(m->object());
			delete m;
		}
	};


	TEST_F(MutexTest, Locks) {
		m->lock();
		EXPECT_EQ(0, testThreadded(&testMutexLocked, (void*) m->object()));
	}


	TEST_F(MutexTest, Unlocks) {
		pthread_mutex_lock(m->object());

		m->unlock();
		EXPECT_EQ(0, testThreadded(&testMutexUnlocked, (void*) m->object()));
	}

}
}
