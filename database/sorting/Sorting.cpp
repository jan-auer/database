//
//  sorting.cpp
//  database
//
//  Created by Jan Michael Auer on 11/04/14.
//  Copyright (c) 2014 LightningSQL. All rights reserved.
//

#include <algorithm>    // std::sort
#include <vector>       // std::vector
#include <queue>        // std::priority_queue

#include "FileUtils.h"
#include "Chunk.h"
#include "Sorting.h"

using namespace std;

namespace lsql {

	const char* BUCKET_FILE_NAME = "./buckets";
	
	typedef uint64_t BucketValue;
	typedef vector<BucketValue> Bucket;
	typedef Chunk<BucketValue> BucketChunk;
	
	struct ChunkComparator {
		bool operator()(const BucketChunk* a, const BucketChunk* b) const {
			return *a->current() > *b->current();
		}
	};
	
	typedef priority_queue<BucketChunk*, vector<BucketChunk*>, ChunkComparator> ChunkQueue;
	
	
	//ToDo: Add documentation
	uint64_t prepareBuckets(int fdInput, uint64_t inputCount, int fdBuckets, uint64_t bucketSize);
	void mergeBuckets(int fdBuckets, uint64_t bucketSize, uint64_t bucketCount, int fdOutput);
	void shiftSmallest(ChunkQueue& outputQueue, Bucket& outputBuffer, int fdOutput);

	void Sorting::externalSort(int fdInput, uint64_t inputCount, int fdOutput, uint64_t memSize) {
		size_t bucketSize = memSize / sizeof(BucketValue);

		int fdBuckets = FileUtils::openWrite(BUCKET_FILE_NAME);
		FileUtils::allocate<BucketValue>(fdBuckets, inputCount);
		uint64_t bucketCount = prepareBuckets(fdInput, inputCount, fdBuckets, bucketSize);
		FileUtils::close(fdBuckets);
		
		fdBuckets = FileUtils::openRead(BUCKET_FILE_NAME);
		mergeBuckets(fdBuckets, bucketSize, bucketCount, fdOutput);
		FileUtils::close(fdBuckets);
		
		FileUtils::remove(BUCKET_FILE_NAME);
	};
	
	uint64_t prepareBuckets(int fdInput, uint64_t inputCount, int fdBuckets, uint64_t bucketSize) {
		int64_t bucketCount = -1;
		uint64_t elementOffset = 0;
		
		Bucket bucket(0);
			
		//FIXME: Crash with bad alloc if size is too big, e.g. 2048 MiB Memory
		bucket.reserve(bucketSize);
		
		do {
			FileUtils::readVector(fdInput, bucket, bucketSize, elementOffset);
			sort(bucket.begin(), bucket.end());
//			for (auto e : bucket) cout << " - " << hex << e << endl;
			FileUtils::writeVector(fdBuckets, bucket);
			
			bucketCount++;
			elementOffset += bucketSize;
			
			cout << "INFO: Finished preparing bucket " << bucketCount << endl;

		} while (bucket.size() > 0);
		
		return bucketCount;
	}
	
	void mergeBuckets(int fdBuckets, uint64_t bucketSize, uint64_t bucketCount, int fdOutput) {
		
		uint64_t chunkSize = bucketSize / (bucketCount + 1);

		ChunkQueue outputQueue;
		Bucket outputBuffer;
		outputBuffer.reserve(chunkSize);
		
		cout << "INFO: Starting k-way merge" << endl;

		for (int i = 0; i < bucketCount; i++) {
			BucketChunk* chunk = new BucketChunk(fdBuckets, i * bucketSize, chunkSize, bucketSize);
			chunk->next();
			outputQueue.push(chunk);
//			cout << "Inserting: " << hex << *chunk->current()
//			     << " (top: " << *outputQueue.top()->current() << ")" << endl;
		}
			
		while (!outputQueue.empty())
			shiftSmallest(outputQueue, outputBuffer, fdOutput);
	}
	
	void shiftSmallest(ChunkQueue& outputQueue, Bucket& outputBuffer, int fdOutput) {
		BucketChunk* chunk = outputQueue.top();
		outputQueue.pop();
		
//		cout << "Writing: " << hex << *chunk->current() << endl;
		outputBuffer.push_back(*chunk->current());
		if (outputBuffer.size() == outputBuffer.capacity()) {
			FileUtils::writeVector(fdOutput, outputBuffer);
			outputBuffer.clear();
		}
		
		if (chunk->next() != 0) {
			outputQueue.push(chunk);
//			cout << "Inserting: " << hex << *chunk->current()
//			     << " (top: " << *outputQueue.top()->current() << ")" << endl;
		} else {
			delete chunk;
		}
	}
	
}
