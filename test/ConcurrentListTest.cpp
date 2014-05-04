//
//  ConcurrentListTest.cpp
//  database
//
//  Created by Jan Michael Auer on 04/05/14.
//  Copyright (c) 2014 LightningSQL. All rights reserved.
//

#include "utils/ConcurrentList.h"

namespace lsql {
namespace test {

	struct TestItem {
		TestItem* prev;
		TestItem* next;
	};

	struct TestPolicy {
		TestItem*& next(TestItem* item) const {
			return item->next;
		}
		TestItem*& prev(TestItem* item) const {
			return item->prev;
		}
	};

	typedef ConcurrentList<TestItem, TestPolicy> TestList;

	struct ConcurrentListTest : public testing::Test {
		TestList* list;

		virtual void SetUp() {
			list = new TestList();
		}

		virtual void TearDown() {
			list->cleanup();
			delete list;
		}
	};

	TEST_F(ConcurrentListTest, InitializesEmpty) {
		EXPECT_EQ(nullptr, list->getFirst());
		EXPECT_EQ(nullptr, list->getLast());
	}

	TEST_F(ConcurrentListTest, AppendsInCorrectOrder) {
		TestItem* item1 = new TestItem();
		TestItem* item2 = new TestItem();
		list->append(item1);
		list->append(item2);

		EXPECT_EQ(item1, list->getFirst());
		EXPECT_EQ(item2, list->getLast());

		EXPECT_EQ(nullptr, item1->prev);
		EXPECT_EQ(item2, item1->next);

		EXPECT_EQ(item1, item2->prev);
		EXPECT_EQ(nullptr, item2->next);
	}

	TEST_F(ConcurrentListTest, PrependsInCorrectOrder) {
		TestItem* item1 = new TestItem();
		TestItem* item2 = new TestItem();
		list->prepend(item2);
		list->prepend(item1);

		EXPECT_EQ(item1, list->getFirst());
		EXPECT_EQ(item2, list->getLast());

		EXPECT_EQ(nullptr, item1->prev);
		EXPECT_EQ(item2, item1->next);

		EXPECT_EQ(item1, item2->prev);
		EXPECT_EQ(nullptr, item2->next);
	}

	TEST_F(ConcurrentListTest, MovesFirstItem) {
		TestItem* item1 = new TestItem();
		TestItem* item2 = new TestItem();
		list->append(item1);
		list->append(item2);

		list->bringFront(item1);

		EXPECT_EQ(item1, list->getFirst());
		EXPECT_EQ(item2, list->getLast());

		EXPECT_EQ(nullptr, item1->prev);
		EXPECT_EQ(item2, item1->next);

		EXPECT_EQ(item1, item2->prev);
		EXPECT_EQ(nullptr, item2->next);
	}
	
	TEST_F(ConcurrentListTest, MovesMiddleItem) {

	}
	
	TEST_F(ConcurrentListTest, MovesLastItem) {
		TestItem* item1 = new TestItem();
		TestItem* item2 = new TestItem();
		list->append(item1);
		list->append(item2);

		list->bringFront(item2);

		EXPECT_EQ(item2, list->getFirst());
		EXPECT_EQ(item1, list->getLast());

		EXPECT_EQ(nullptr, item2->prev);
		EXPECT_EQ(item1, item2->next);

		EXPECT_EQ(item2, item1->prev);
		EXPECT_EQ(nullptr, item1->next);
	}
	
	TEST_F(ConcurrentListTest, RemovesMiddleItem) {
		TestItem* item1 = new TestItem();
		TestItem* item2 = new TestItem();
		TestItem* item3 = new TestItem();
		list->append(item1);
		list->append(item2);
		list->append(item3);

		list->remove(item2);

		EXPECT_EQ(item1, list->getFirst());
		EXPECT_EQ(item3, list->getLast());

		EXPECT_EQ(item3, item1->next);
		EXPECT_EQ(item1, item3->prev);

		EXPECT_EQ(nullptr, item2->prev);
		EXPECT_EQ(nullptr, item2->next);
	}

	TEST_F(ConcurrentListTest, RemovesFirstItem) {
		TestItem* item1 = new TestItem();
		TestItem* item2 = new TestItem();
		list->append(item1);
		list->append(item2);

		list->remove(item1);

		EXPECT_EQ(item2, list->getFirst());
		EXPECT_EQ(item2, list->getLast());

		EXPECT_EQ(nullptr, item2->prev);

		EXPECT_EQ(nullptr, item1->prev);
		EXPECT_EQ(nullptr, item1->next);
	}
	
	TEST_F(ConcurrentListTest, RemovesLastItem) {
		TestItem* item1 = new TestItem();
		TestItem* item2 = new TestItem();
		list->append(item1);
		list->append(item2);

		list->remove(item2);

		EXPECT_EQ(item1, list->getFirst());
		EXPECT_EQ(item1, list->getLast());

		EXPECT_EQ(nullptr, item1->next);

		EXPECT_EQ(nullptr, item2->prev);
		EXPECT_EQ(nullptr, item2->next);
	}
	
	TEST_F(ConcurrentListTest, RemovesOnlyItem) {
		TestItem* item = new TestItem();
		list->append(item);

		list->remove(item);

		EXPECT_EQ(nullptr, list->getFirst());
		EXPECT_EQ(nullptr, list->getLast());

		EXPECT_EQ(nullptr, item->prev);
		EXPECT_EQ(nullptr, item->next);
	}

	TEST_F(ConcurrentListTest, Empties) {
		TestItem* item = new TestItem();
		list->append(item);

		list->empty();

		EXPECT_EQ(nullptr, list->getFirst());
		EXPECT_EQ(nullptr, list->getLast());
	}

	TEST_F(ConcurrentListTest, CleansPointers) {
		TestItem* item = new TestItem();
		list->append(item);

		list->cleanup();

		EXPECT_EQ(nullptr, list->getFirst());
		EXPECT_EQ(nullptr, list->getLast());
	}

}
}
