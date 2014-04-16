//
//  Generator.cpp
//  database
//
//  Created by Jan Michael Auer on 12/04/14.
//  Copyright (c) 2014 LightningSQL. All rights reserved.
//

#include "File.h"
#include "Generator.h"

namespace lsql {
	
	/**
	 * Generates random uint64_t nubers.
	 */
	class RandomLong {
		
		uint64_t state;
		
	public:
		
		/**
		 * Creates a new random number generator.
		 * 
		 * @param seed A seed for deterministic number generation.
		 */
		explicit RandomLong(uint64_t seed) : state(seed) {}
		
		/**
		 * Creates the next random number.
		 */
		uint64_t next() {
			state ^= state << 13;
			state ^= state >> 7;
			return (state ^= (state << 17));
		}
		
	};

	Generator::Generator(size_t bufferSize, uint64_t seed) :
		bufferSize(bufferSize),
		seed(seed) {}
	
	bool Generator::generate(const char* fileName, off_t n) const {
		RandomLong rand(seed);
		
		if (n <= 0) {
			cerr << "Invalid number of elements: " << n << endl;
			return false;
		}
		
		File<uint64_t> f(fileName);
		if (!f.open(true))
			return false;
		if (!f.allocate(n))
			return false;
		
		vector<uint64_t> data;
		data.reserve(bufferSize);
		
		for (off_t i = 0; i < n; i++) {
			uint64_t num = rand.next();
			data.push_back(num);
			if (i % bufferSize == 0) {
				if (!f.writeVector(data))
					return false;
				
				data.clear();
			}
		}
		
		if (!f.writeVector(data))
			return false;
		
		return true;
	}
	
}