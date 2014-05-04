//
//  BufferManagerTest.cpp
//  database
//
//  Created by Jan Michael Auer on 21/04/14.
//  Copyright (c) 2014 LightningSQL. All rights reserved.
//

#include "buffer/BufferManager.h"

namespace lsql {
namespace test {

	const size_t FILE_SIZE = 2 * BufferFrame::SIZE;

	struct BufferManagerTest : public testing::Test {
		BufferManager* manager;

		virtual void SetUp() {
			File<uint8_t> file("1", true);
			file.allocate(FILE_SIZE);
			manager = new BufferManager("1", 2);
		}

		virtual void TearDown() {
			delete manager;
		}
	};

	TEST_F(BufferManagerTest, InitializesSize) {
		manager->fixPage(PageId(1, 0), false);
		BufferFrame& frame0 = manager->fixPage(PageId(1, 0), false);

		manager->fixPage(PageId(1, 1), false);
		BufferFrame& frame1 = manager->fixPage(PageId(1, 1), false);

		manager->unfixPage(frame0, false);
		manager->unfixPage(frame1, false);
	}

}
}
