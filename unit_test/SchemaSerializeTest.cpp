//
//  SchemaSerializeTest.h
//  database
//
//  Created by Felix Jankowski on 12.05.14.
//  Copyright (c) 2014 LightningSQL. All rights reserved.
//

#include "gtest/gtest.h"
#include "utils/Serialize.h"

#include "schema/Schema.h"

TEST(Serialization, Number) {

	long t1;

	for (size_t i=0; i<500000; ++i) {

		t1 = random();
		StreamType res;
		serialize(t1,res);
		auto t2 = deserialize<decltype(t1)>(res);
		if (t1!=t2) { std::cerr << "PROBLEMS!" << std::endl; }
	}
}
