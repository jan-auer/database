//
//  PageIdTest.cpp
//  database
//
//  Created by Jan Michael Auer on 21/04/14.
//  Copyright (c) 2014 LightningSQL. All rights reserved.
//

#include "common/IDs.h"

namespace lsql {
namespace test {

	TEST(PageIdTest, StoresSegmentId) {
		PID id(1, 2);
		EXPECT_EQ(1, id.segment());
	}

	TEST(PageIdTest, StoresPageId) {
		PID id(1, 2);
		EXPECT_EQ(2, id.page());
	}

}
}