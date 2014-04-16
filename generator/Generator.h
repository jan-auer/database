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

	/**
	 * Creates files with deterministic random numbers.
	 */
	class Generator {
		
		size_t bufferSize;
		uint64_t seed;
		
	public:
		
		/**
		 * Creates a new file generator.
		 * Instead of writing to the file directly, all output is 
		 * stored in an output buffer.
		 *
		 * @param bufferSize The size of the output buffer.
		 * @param seed       A seed for the random number generator.
		 */
		Generator(size_t bufferSize, uint64_t seed = 88172645463325252ull);

		/**
		 * Generates a new random file at the specified location.
		 *
		 * @param fileName A fully qualified path to the file.
		 * @param n        The number of elements in the file.
		 */
		bool generate(const char* fileName, off_t n) const;
		
	};

}

#include "Generator.cpp"

#endif /* defined(__database__Generator__) */
