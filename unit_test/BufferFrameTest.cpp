//
//  BufferManager.cpp
//  database
//
//  Created by Jan Michael Auer on 17/04/14.
//  Copyright (c) 2014 LightningSQL. All rights reserved.
//

#include "buffer/BufferFrame.h"

namespace lsql {
namespace test {

	struct BufferFrameTest : public testing::Test {
		BufferFrame* frame;
		
		virtual void SetUp() {
			PID p(1,2);
			frame = new BufferFrame(p);
		}
		
		virtual void TearDown() {
			delete frame;
		}
	};

	TEST_F(BufferFrameTest, InitializesSize) {
		EXPECT_GT(BufferFrame::SIZE, 0) << "BufferFrame::SIZE has not been initialized";
	}

	TEST_F(BufferFrameTest, InitializesDirty) {
		EXPECT_FALSE(frame->isDirty()) << "";
	}

}
}
