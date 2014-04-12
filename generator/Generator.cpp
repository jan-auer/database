//
//  Generator.cpp
//  database
//
//  Created by Jan Michael Auer on 12/04/14.
//  Copyright (c) 2014 LightningSQL. All rights reserved.
//

#include "FileUtils.h"
#include "Generator.h"

namespace lsql {
	
	
	class RandomLong {
		
		uint64_t state;
		
	public:
		
		//
		explicit RandomLong(uint64_t seed) : state(seed) {}
		
		//
		uint64_t next() {
			state ^= state << 13;
			state ^= state >> 7;
			return (state ^= (state << 17));
		}
		
	};

	
	Generator::Generator(uint64_t bufferSize, uint64_t seed) :
		bufferSize(bufferSize),
		seed(seed) {}
	
	bool Generator::generate(const char* fileName, uint64_t n) {
		RandomLong rand(seed);
		
		if (n <= 0) {
			cerr << "Invalid number of elements: " << n << endl;
			return false;
		}
		
		int fd = FileUtils::openWrite(fileName);
		if (fd < 0)
			return false;
		
		if (!FileUtils::allocate<uint64_t>(fd, n))
			return false;
		
		
		vector<uint64_t> data;
		data.reserve(bufferSize);
		
		for (int i = 0; i < n; i++) {
			uint64_t num = rand.next();
			data.push_back(num);
			if (i % bufferSize == 0) {
				if (!FileUtils::writeVector(fd, data))
					return false;
				
				data.clear();
			}
		}
		
		if (!FileUtils::writeVector(fd, data))
			return false;
		
		if (!FileUtils::close(fd))
			return false;
		
		return true;
	}
	
}