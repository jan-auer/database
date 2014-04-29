//
//  sorting.cpp
//  database
//
//  Created by Jan Michael Auer on 11/04/14.
//  Copyright (c) 2014 LightningSQL. All rights reserved.
//

#include <cstdio>
#include <algorithm>    
#include <vector>       
#include <queue>

#include "File.h"
#include "Chunk.h"
#include "Sorting.h"

using namespace std;

namespace lsql {

	/// Compares two Chunks based on their current element
	template<typename Element>
	struct ChunkComparator {
		bool operator()(const Chunk<Element>* a, const Chunk<Element>* b) const {
			return *a->current() > *b->current();
		}
	};

	/// A queue for chunks of elements which returns the smallest element as top().
	template<typename Element>
	struct ChunkQueue : priority_queue<Chunk<Element>*, vector<Chunk<Element>*>, ChunkComparator<Element>> {};
		
	/**
	 * Prepares individual runs by loading buckets into the memory 
	 * and sorting them. The buckets are then written to the temporary
	 * buckets file.
	 * NOTE: The input file is expected to be opened for read and the
	 * buckets file for write access.
	 *
	 * @param inputFile   A file which contains all numbers to sort.
	 * @param inputCount  The number of elements in the input file.
	 * @param bucketsFile A temporary file handle for all buckets.
	 * @param bucketSize  The maximum size for each bucket.
	 *
	 * @return The number of buckets created by this method.
	 */
	template<typename Element>
	off_t prepareBuckets(File<Element>& inputFile, off_t inputCount, File<Element>& bucketsFile, size_t bucketSize);

	/**
	 * Merges all previously sorted runs. The merge is performed
	 * using a priority queue and an output buffer. The algorithm tries
	 * not to exceed the given memory limit.
	 * NOTE: The buckets file is expected to be opened for read an the
	 * ouput file for write access.
	 *
	 * TODO: Merge recursively.
	 *
	 * @param bucketsFile A temporary file containing all buckets.
	 * @param bucketCount The total number of buckets.
	 * @param bucketSize  The size of the buckets.
	 * @param outputFile  A reference to the output file.
	 * @param memSize     The memory limit in bytes.
	 */
	template<typename Element>
	void mergeBuckets(File<Element>& bucketsFile, off_t bucketCount, size_t bucketSize, File<Element>& outputFile, size_t memSize);

	/**
	 * Writes the smallest element from the outputQueue to the specified
	 * output buffer. The buffer is flushed to the given output file as
	 * soon at it has reached its capacity.
	 * NOTE: The output file is expected to be opened with write access.
	 *
	 * @param outputQueue  A queue containing chunks of all previous runs.
	 * @param outputBuffer A buffer for the sorted output values.
	 * @param outputFile   The target file handle.
	 */
	template<typename Element>
	void shiftSmallest(ChunkQueue<Element>& outputQueue, vector<Element>& outputBuffer, File<Element>& outputFile);

	template<typename Element>
	void externalSort(File<Element>& inputFile, off_t inputCount, File<Element>& outputFile, size_t memSize) {
		size_t memElements = memSize / sizeof(Element);
		
		File<Element> bucketsFile;
		bucketsFile.allocate(memElements);
		
		off_t bucketCount = prepareBuckets(inputFile, inputCount, bucketsFile, memElements);
		mergeBuckets(bucketsFile, bucketCount, memElements, outputFile, memSize);
	};
	
	template<typename Element>
	off_t prepareBuckets(File<Element>& inputFile, off_t inputCount, File<Element>& bucketsFile, size_t bucketSize) {
		int64_t bucketCount = -1;
		off_t elementOffset = 0;
		
		vector<Element> bucket;
		bucket.reserve(bucketSize);
		
		do {
			inputFile.readVector(bucket, bucketSize, elementOffset);
			sort(bucket.begin(), bucket.end());
			bucketsFile.writeVector(bucket);
			
			bucketCount++;
			elementOffset += bucketSize;
		} while (bucket.size() > 0);
		
		return bucketCount;
	}
	
	template<typename Element>
	void mergeBuckets(File<Element>& bucketsFile, off_t bucketCount, size_t bucketSize, File<Element>& outputFile, size_t memSize) {
		off_t chunkSize = (memSize /*- bucketCount * sizeof(Chunk<Element>)*/) / sizeof(Element) / (bucketCount + 1);
		
		ChunkQueue<Element> outputQueue;
		vector<Element> outputBuffer;
		outputBuffer.reserve(chunkSize);
		
		for (int i = 0; i < bucketCount; i++) {
			Chunk<Element>* chunk = new Chunk<Element>(bucketsFile, i * bucketSize, chunkSize, bucketSize);
			chunk->next();
			outputQueue.push(chunk);
		}
		
		while (!outputQueue.empty())
			shiftSmallest(outputQueue, outputBuffer, outputFile);
		
		outputFile.writeVector(outputBuffer);
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
