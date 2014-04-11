//
//  sorting.cpp
//  database
//
//  Created by Jan Michael Auer on 11/04/14.
//  Copyright (c) 2014 LightningSQL. All rights reserved.
//

#include <array>
#include "Sorting.h"

using namespace std;

namespace lsql {
	
	void Sorting::externalSort(int fdInput, uint64_t size, int fdOutput, uint64_t memSize) {
		array<uint64_t, sizeof(uint64_t)>* buffer;
		
		// determine number of buckets
		// sort each bucket:
		//  - load bucket into buffer
		//  - sort bucket in memory
		//  - store bucket in a temporary file
		// merge buckets:
		//  - load chunks of each bucket into a priority queue
		//  - take the smallest out and relaod from the origin chunk
		//  - reload a chunk from the temp bucket, if it is empty
	};
	
	uint32_t Sorting::prepareBuckets(int fdInput, uint64_t inputCount, int fdBuckets, uint64_t memSize) {
		
	}
	
	void Sorting::mergeBuckets(int fdBuckets, uint64_t bucketSize, int fdOutput, uint64_t memSize) {
		
	}
	
}