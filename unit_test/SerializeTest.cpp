//
//  Serialize.cpp
//  database
//
//  Created by Felix Jankowski on 12.05.14.
//  Copyright (c) 2014 LightningSQL. All rights reserved.
//

/**
 * Copyright (c) 2013, Simone Pellegrini All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  - Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 *  - Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#include "gtest/gtest.h"
#include "utils/Serialize.h"

#include <sstream>
#include <chrono>

TEST(get_size, Ints) {
	uint16_t v1 = 2u;
	EXPECT_EQ(get_size(v1), sizeof(decltype(v1)));
	EXPECT_EQ(get_size(v1), 2u);

	uint32_t v2 = 10u;
	EXPECT_EQ(get_size(v2), sizeof(decltype(v2)));
	EXPECT_EQ(get_size(v2), 4u);
}

TEST(get_size, Vector) {
	std::vector<uint8_t> v1 = { 0u, 1u, 2u };
	EXPECT_EQ(get_size(v1), sizeof(size_t)+v1.size());

	std::vector<uint64_t> v2 = { 0u, 8u };
	EXPECT_EQ(get_size(v2), sizeof(size_t)+v2.size()*sizeof(uint64_t));
}

TEST(get_size, String) {
	std::string s1 = "hello";
	EXPECT_EQ(get_size(s1), sizeof(size_t)+s1.length());

	std::string s2;
	EXPECT_EQ(get_size(s2), sizeof(size_t));
}

TEST(get_size, Tuple) {
	auto t1 = std::make_tuple(0ul, std::string("hi"));
	EXPECT_EQ(get_size(t1), sizeof(unsigned long)+sizeof(size_t)+2u);

	auto t2 = std::make_tuple(std::vector<uint16_t>{'a','b','c'}, 10, 20);
	EXPECT_EQ(get_size(t2), sizeof(size_t)+3*2+sizeof(int)*2);
}

TEST(Serialize, Ints) {

	uint32_t v1 = 10;
	StreamType res;
	serialize(v1,res);
	EXPECT_EQ(sizeof(uint32_t), res.size());
	EXPECT_EQ(res, std::vector<uint8_t>({0xA, 0, 0, 0}));

	res.clear();

	uint64_t v2 = 64;
	serialize(v2,res);
	EXPECT_EQ(sizeof(uint64_t), res.size());
	EXPECT_EQ(res, std::vector<uint8_t>({0x40, 0, 0, 0, 0, 0, 0, 0}));

	res.clear();

	int v3 = -1;
	serialize(v3,res);
	EXPECT_EQ(sizeof(int), res.size());
	EXPECT_EQ(res, std::vector<uint8_t>({0xFF, 0xFF, 0xFF, 0xFF}));

}

TEST(Serialize, MultipleInvocations) {

	StreamType res;
	serialize(std::vector<char>{1,2}, res);
	serialize(std::vector<char>{3,4}, res);
	EXPECT_EQ(4u+sizeof(size_t)*2, res.size());
	EXPECT_EQ(res, std::vector<uint8_t>({0x2, 0, 0, 0, 0, 0, 0, 0,
		1, 2,
		0x2, 0, 0, 0, 0, 0, 0, 0,
		3, 4}));
}

TEST(Deerialize, MultipleInvocations) {

	std::vector<uint8_t> res{0x2, 0, 0, 0, 0, 0, 0, 0, 1, 2, 0x2, 0, 0, 0, 0, 0, 0, 0, 3, 4};

	auto it = res.cbegin();
	auto v1 = deserialize<std::vector<char>>(it, res.cend());
	auto v2 = deserialize<std::vector<char>>(it, res.end());

	EXPECT_EQ(v1, std::vector<char>({1,2}));
	EXPECT_EQ(v2, std::vector<char>({3,4}));

}

TEST(Deserialize, Ints) {

	int v1 = deserialize<uint32_t>({0xA, 0, 0, 0});
	EXPECT_EQ(v1, 10);

	auto v2 = deserialize<uint64_t>({0x40, 0, 0, 0, 0, 0, 0, 0});
	EXPECT_EQ(v2, 64u);

	auto v3 = deserialize<int>({0xFF, 0xFF, 0xFF, 0xFF});
	EXPECT_EQ(v3, -1);

}

TEST(Serialize, Vector) {

	auto t1 = std::vector<int>{1,2};

	StreamType res;
	serialize(t1,res);
	EXPECT_EQ(sizeof(decltype(t1)::value_type)*t1.size()+sizeof(size_t), res.size());
	EXPECT_EQ(res, std::vector<uint8_t>({2, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 2, 0, 0, 0}));

	res.clear();

	auto t2 = std::vector<std::vector<uint8_t>>{{1,2}, {3,4}};
	serialize(t2,res);
	EXPECT_EQ(get_size(t2), res.size());
	EXPECT_EQ(28u, res.size());
	EXPECT_EQ(res, std::vector<uint8_t>(
																			{2, 0, 0, 0, 0, 0, 0, 0,
																				2, 0, 0, 0, 0, 0, 0, 0, 1, 2,
																				2, 0, 0, 0, 0, 0, 0, 0, 3, 4 }));

}


TEST(Deserialize, Vector) {

	auto v1 = deserialize<std::vector<int>>({2,0,0,0,0,0,0,0,1,0,0,0,2,0,0,0});
	EXPECT_EQ(v1, std::vector<int>({1,2}));

	auto v2 =
	deserialize<std::vector<std::vector<uint8_t>>>(
																								 {2, 0, 0, 0, 0, 0, 0, 0,
																									 2, 0, 0, 0, 0, 0, 0, 0, 1, 2,
																									 2, 0, 0, 0, 0, 0, 0, 0, 3, 4
																								 });

	EXPECT_EQ(v2, std::vector<std::vector<uint8_t>>({{1,2},{3,4}}));
}

TEST(Serialize, IntTuple) {

	auto t1 = std::make_tuple(1,2);

	StreamType res;
	serialize(t1,res);
	EXPECT_EQ(sizeof(decltype(t1)), res.size());
	EXPECT_EQ(res, std::vector<uint8_t>({1, 0, 0, 0, 2, 0, 0, 0}));

	res.clear();

	auto t2 = std::make_tuple(256,256*2,256*3);
	serialize(t2,res);
	EXPECT_EQ(sizeof(decltype(t2)), res.size());
	EXPECT_EQ(res, std::vector<uint8_t>({0, 1, 0, 0, 0, 2, 0, 0, 0, 3, 0, 0}));

	res.clear();

	auto t3 = std::tuple<uint32_t, uint64_t>(0,1);
	serialize(t3,res);
	EXPECT_EQ(get_size(t3), res.size());
	EXPECT_EQ(res, std::vector<uint8_t>({0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0}));

}

TEST(Deserialize, IntTuple) {
	auto t1 = deserialize<std::tuple<int,int>>({1, 0, 0, 0, 2, 0, 0, 0});
	EXPECT_EQ(t1, std::make_tuple(1,2));

	auto t2 = deserialize<std::tuple<int,int,char>>({1, 0, 0, 0, 2, 0, 0, 0, 3});
	EXPECT_EQ(t2, std::make_tuple(1,2,3));
}

TEST(Serialize, TupleVec) {

	auto t1 = std::tuple<int,int,std::vector<uint8_t>>(10, 20, std::vector<uint8_t>{1,2});
	StreamType res;
	serialize(t1,res);
	EXPECT_EQ(18u, res.size());
	EXPECT_EQ(res, std::vector<uint8_t>({
		10, 0, 0, 0,
		20, 0, 0, 0,
		2, 0, 0, 0, 0, 0, 0, 0, 1, 2}));
}

TEST(Deserialize, TupleVec) {

	auto v1 = std::vector<uint8_t>{
		10, 0, 0, 0,
		20, 0, 0, 0,
		2, 0, 0, 0, 0, 0, 0, 0, 1, 2};

	auto t = deserialize<std::tuple<int,int,std::vector<uint8_t>>>(v1);
	EXPECT_EQ(t, std::make_tuple(10,20,std::vector<uint8_t>({1,2})));
}

TEST(Serialize, String) {

	std::string s = "string";
	StreamType res;
	serialize(s,res);
	EXPECT_EQ(14u, get_size(s));
	EXPECT_EQ(14u, res.size());
	EXPECT_EQ(res, std::vector<uint8_t>({6, 0, 0, 0, 0, 0, 0, 0, 's', 't', 'r', 'i', 'n', 'g'}));

}

TEST(Deserialize, String) {

	auto res = std::vector<uint8_t>{6, 0, 0, 0, 0, 0, 0, 0, 's', 't', 'r', 'i', 'n', 'g'};
	auto str = deserialize<std::string>(res);

	EXPECT_EQ("string", str);
}
