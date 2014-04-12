//
//  Generator.h
//  database
//
//  Created by Jan Michael Auer on 12/04/14.
//  Copyright (c) 2014 LightningSQL. All rights reserved.
//

#ifndef __database__Generator__
#define __database__Generator__

#include <iostream>
#include <vector>

namespace lsql {

	//
	class Generator {
		
		uint64_t bufferSize;
		uint64_t seed;
		
	public:
		
		//
		Generator(uint64_t bufferSize, uint64_t seed = 88172645463325252ull);

		//
		bool generate(const char* fileName, uint64_t n);
		
	};

}

#include "Generator.cpp"

#endif /* defined(__database__Generator__) */
