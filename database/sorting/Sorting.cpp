//
//  sorting.cpp
//  database
//
//  Created by Jan Michael Auer on 11/04/14.
//  Copyright (c) 2014 LightningSQL. All rights reserved.
//

#include <vector>
#include "FileUtils.h"
#include "Sorting.h"

using namespace std;

namespace lsql {

	const char* BUCKET_FILE_NAME = "./buckets";

	void Sorting::externalSort(int fdInput, uint64_t inputCount, int fdOutput, uint64_t memSize) {
		size_t bucketSize = memSize / sizeof(uint64_t);

		int fdBuckets = FileUtils::openWrite(BUCKET_FILE_NAME);
		FileUtils::allocate<uint64_t>(fdBuckets, inputCount);
		
		uint64_t bucketCount = prepareBuckets(fdInput, inputCount, fdBuckets, bucketSize);
		mergeBuckets(fdBuckets, bucketSize, bucketCount, fdOutput, memSize);
		
		//TODO: remove fdBuckets
	};
	
	uint64_t Sorting::prepareBuckets(int fdInput, uint64_t inputCount, int fdBuckets, uint64_t bucketSize) {
		int64_t bucketCount = -1;
		uint64_t elementOffset = 0;
		
		vector<uint64_t> bucket(0);
		bucket.reserve(bucketSize);
		
		do {
			FileUtils::readVector(fdInput, bucket, bucketSize, elementOffset);
			sort(bucket.begin(), bucket.end());
			FileUtils::writeVector(fdBuckets, bucket);
			
			bucketCount++;
			elementOffset += bucketSize;
		} while (bucket.size() > 0);
		
		return bucketCount;
	}
	
	void Sorting::mergeBuckets(int fdBuckets, uint64_t bucketSize, uint64_t bucketCount, int fdOutput, uint64_t memSize) {
		//  - load chunks of each bucket into a priority queue
		//  - take the smallest out and relaod from the origin chunk
		//  - reload a chunk from the temp bucket, if it is empty
	}
	
}
