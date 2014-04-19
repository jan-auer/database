//
//  ConcurrentMap.cpp
//  database
//
//  Created by Jan Michael Auer on 18/04/14.
//  Copyright (c) 2014 LightningSQL. All rights reserved.
//

#include <iostream>
#include "gtest/gtest.h"
#include "helpers/ThreadHelpers.cpp"
#include "ConcurrentMap.h"

namespace lsql {
namespace test {

	struct ConcurrentMapTest : public testing::Test {
		ConcurrentMap<int, int>* map;
		Lock* lock;
		
		virtual void SetUp() {
			map = new ConcurrentMap<int, int>(2);
			lock = nullptr;
		}
		
		virtual void TearDown() {
			delete map;
		}
	};

	//
	// ENTRIES   -----------------------------------------------------
	//

	TEST_F(ConcurrentMapTest, ContainsInsertedElements) {
		map->insert(42, 42);
		EXPECT_TRUE(map->contains(42));
	}

	TEST_F(ConcurrentMapTest, ContainsNoMissingElements) {
		EXPECT_FALSE(map->contains(42));
	}

	TEST_F(ConcurrentMapTest, GetsInsertedElements) {
		map->insert(42, 42);
		EXPECT_EQ(42, *map->get(42));
	}

	TEST_F(ConcurrentMapTest, GetsNoMissingElements) {
		EXPECT_EQ(nullptr, map->get(42));
	}

	TEST_F(ConcurrentMapTest, RemovesElements) {
		map->insert(42, 42);
		map->remove(42);
		EXPECT_FALSE(map->contains(42));
	}

	TEST_F(ConcurrentMapTest, FindsElementsInTheSameSlot) {
		map->insert(1, 1);            // slot 0
		map->insert(2, 2);            // slot 1
		map->insert(3, 3);            // slot 0
		map->insert(4, 4);            // slot 1
		
		EXPECT_EQ(1, *map->get(1));   // slot 0
		EXPECT_EQ(2, *map->get(2));   // slot 1
		EXPECT_EQ(3, *map->get(3));   // slot 0
		EXPECT_EQ(4, *map->get(4));   // slot 1
	}

	//
	// CONCURRENCY   -----------------------------------------------------
	//

	TEST_F(ConcurrentMapTest, LocksEntrySharedOnContains) {
		map->insert(42, 42, &lock);
		unlock(lock);
		
		map->contains(42, &lock);
		ssize_t ret = testThreadded(&testSharedLock, (void*) lock);
		EXPECT_EQ(0, ret);
	}

	TEST_F(ConcurrentMapTest, LocksEntrySharedOnGet) {
		map->insert(42, 42, &lock);
		unlock(lock);
		
		map->get(42, &lock, false);
		ssize_t ret = testThreadded(&testSharedLock, (void*) lock);
		EXPECT_EQ(0, ret);
	}

	TEST_F(ConcurrentMapTest, LocksEntryExclusiveOnGet) {
		map->insert(42, 42, &lock);
		unlock(lock);
		
		map->get(42, &lock, true);
		ssize_t ret = testThreadded(&testExclusiveLock, (void*) lock);
		EXPECT_EQ(0, ret);
	}

	TEST_F(ConcurrentMapTest, LocksEntrySharedOnInsert) {
		map->insert(42, 42, &lock);
		ssize_t ret = testThreadded(&testExclusiveLock, (void*) lock);
		EXPECT_EQ(0, ret);
	}

}
}
