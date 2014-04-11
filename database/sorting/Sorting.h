//
//  sorting.h
//  database
//
//  Created by Jan Michael Auer on 11/04/14.
//  Copyright (c) 2014 LightningSQL. All rights reserved.
//

#ifndef __database__sorting__
#define __database__sorting__

#include <iostream>

namespace lsql {
	
	//
	class Sorting {
	public:
		
		//
		static void externalSort(int fdInput, uint64_t inputCount, int fdOutput, uint64_t memSize);
		
	private:
		
		//
		static uint64_t prepareBuckets(int fdInput, uint64_t inputCount, int fdBuckets, uint64_t memSize);
		
		//
		static void mergeBuckets(int fdBuckets, uint64_t bucketSize, uint64_t bucketCount, int fdOutput, uint64_t memSize);
		
	};
	
}

#include "Sorting.cpp"

#endif /* defined(__database__sorting__) */
