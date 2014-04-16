//
//  sorting.cpp
//  database
//
//  Created by Jan Michael Auer on 11/04/14.
//  Copyright (c) 2014 LightningSQL. All rights reserved.
//

#include <stdio.h>
#include <algorithm>    // std::sort
#include <vector>       // std::vector
#include <queue>        // std::priority_queue

#include "File.h"
#include "FileUtils.h"
#include "Chunk.h"
#include "Sorting.h"

using namespace std;

namespace lsql {

	template<typename Element>
	struct ChunkComparator {
		bool operator()(const Chunk<Element>* a, const Chunk<Element>* b) const {
			return *a->current() > *b->current();
		}
	};

	///
	template<typename Element>
	struct ChunkQueue : priority_queue<Chunk<Element>*, vector<Chunk<Element>*>, ChunkComparator<Element>> {};
		
	/**
	 *
	 */
	template<typename Element>
	off_t prepareBuckets(File<Element>& inputFile, off_t inputCount, File<Element>& buckets, size_t bucketSize);

	/**
	 *
	 */
	template<typename Element>
	void mergeBuckets(File<Element>& buckets, off_t bucketCount, size_t bucketSize, File<Element>& outputFile, size_t memSize);

	/**
	 *
	 */
	template<typename Element>
	void shiftSmallest(ChunkQueue<Element>& outputQueue, vector<Element>& outputBuffer, File<Element>& outputFile);

	template<typename Element>
	void externalSort(File<Element>& inputFile, off_t inputCount, File<Element>& outputFile, size_t memSize) {
		size_t memElements = memSize / sizeof(Element);
		
		File<Element> buckets;
		buckets.allocate(memElements);
		
		off_t bucketCount = prepareBuckets(inputFile, inputCount, buckets, memElements);
		mergeBuckets(buckets, bucketCount, memElements, outputFile, memSize);
	};
	
	template<typename Element>
	off_t prepareBuckets(File<Element>& inputFile, off_t inputCount, File<Element>& buckets, size_t bucketSize) {
		int64_t bucketCount = -1;
		off_t elementOffset = 0;
		
		vector<Element> bucket;
		bucket.reserve(bucketSize);
		
		do {
			inputFile.readVector(bucket, bucketSize, elementOffset);
			sort(bucket.begin(), bucket.end());
			buckets.writeVector(bucket);
			
			bucketCount++;
			elementOffset += bucketSize;
		} while (bucket.size() > 0);
		
		return bucketCount;
	}
	
	template<typename Element>
	void mergeBuckets(File<Element>& buckets, off_t bucketCount, size_t bucketSize, File<Element>& outputFile, size_t memSize) {
		off_t chunkSize = (memSize /*- bucketCount * sizeof(Chunk<Element>)*/) / sizeof(Element) / (bucketCount + 1);
		
		ChunkQueue<Element> outputQueue;
		vector<Element> outputBuffer;
		outputBuffer.reserve(chunkSize);
		
		for (int i = 0; i < bucketCount; i++) {
			Chunk<Element>* chunk = new Chunk<Element>(buckets, i * bucketSize, chunkSize, bucketSize);
			chunk->next();
			outputQueue.push(chunk);
		}
		
		while (!outputQueue.empty())
			shiftSmallest(outputQueue, outputBuffer, outputFile);
	}

	template<typename Element>
	void shiftSmallest(ChunkQueue<Element>& outputQueue, vector<Element>& outputBuffer, File<Element>& outputFile) {
		Chunk<Element>* chunk = outputQueue.top();
		outputQueue.pop();
		
		outputBuffer.push_back(*chunk->current());
		if (outputBuffer.size() == outputBuffer.capacity()) {
			outputFile.writeVector(outputBuffer);
			outputBuffer.clear();
		}
		
		if (chunk->next() != 0) {
			outputQueue.push(chunk);
		} else {
			delete chunk;
		}
	}
	
}
