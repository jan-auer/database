//
//  BTree.h
//  database
//
//  Created by Felix Jankowski on 19.05.14.
//  Copyright (c) 2014 LightningSQL. All rights reserved.
//

#pragma once
#include "BTree.h"

#include <iostream>
#include "buffer/BufferManager.h"
#include "BTreeNode.h"
#include <vector>

namespace lsql {

	/**
	 * This is a B+-Tree datastructure for indexing data
	 *
	 * @param Keys The datatype of the indexed values
	 * @param Comperator A class providing the < comperator for each data type
	 */
	template<class Key, class Comperator>
	class BTree : protected Segment {

	private:
		uint64_t size;

	public:

		/**
		 * Creates a wrapper for segments containing a B+-Tree.
		 *
		 * @param bufferManager The buffer manager instance.
		 * @param segmentId     The segment identifier.
		 * @param pageCount     The number of pages in this segment.
		 */
		BTree(BufferManager& bufferManager, uint16_t segmentId, uint32_t pageCount = 0);


		/**
		 *
		 */
		bool insert(const Key& key, const TID& tid);


		/**
		 *
		 */
		bool erase(const Key& key);

		/**
		 *
		 */
		TID lookup(const Key& key) const ;

		/**
		 *
		 */
		std::vector<TID> lookupRange(const Key& key) const;


		/**
		 * Getter for the size
		 */
		uint64_t getSize();

	};

}