//
//  BTree.h
//  database
//
//  Created by Jan Michael Auer on 26/05/14.
//  Copyright (c) 2014 LightningSQL. All rights reserved.
//

#pragma once

#include <vector>

#include "common/IDs.h"
#include "buffer/BufferManager.h"
#include "segment/Segment.h"
#include "BTreeNode.h"

namespace lsql {

	/**
	 * This is a B+-Tree data structure for indexing relations.
	 *
	 * @param Keys The datatype of the indexed values
	 * @param Comparator A class providing the < Comparator for each data type
	 */
	template<class Key, class Comparator>
	class BTree : protected Segment, private Comparator {

		using Comparator::compare;

		typedef BTreeNode<Key, Comparator> Node;

		uint64_t size;
		PID root;

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
		 * Finds a key and returns the key's TID. Returns a NULL_TID if not found
		 *
		 * @param	key		A const reference
		 */
		TID lookup(const Key& key);

		/**
		 * @return	FIXME: an iterator that allows to iterate over the result set
		 */
		std::vector<TID> lookupRange(const Key& key);

		/**
		 * Inserts a new Key/TID-Pair into the BTree Index
		 *
		 * @param key		A const reference of the key to be inserted into the tree
		 * @param tid		A reference of the key's TID
		 */
		bool insert(const Key& key, const TID& tid);

		/**
		 * Removes a key from the Index
		 *
		 * @param key		A const reference of the key to be removed from the index
		 * @return			true on success or false if key is not in index
		 */
		bool erase(const Key& key);

		/**
		 * Getter for the size
		 *
		 * @return A uint64_t of the number of Keys in the index
		 */
		uint64_t getSize();

		/**
		 * TODO:
		 */
		void visualize();

	private:

		void visualizeRecurse(PID pid, std::ostream& dataOut);

		/**
		 * This function recursively travels thourgh a B+-Tree and tries to find
		 * the leaf where a key should be stored.
		 * Note: Can split full nodes for concurrent access
		 *
		 * @param Key   A reference to the key that should be found
		 * @param split A bool value. If true, every traversed node that
		 *              does not have at least one empty slot is split
		 * @return      A pointer to a fixed BufferFrame of the leave
		 *              containing the key
		 */
		BufferFrame& findLeafFrame(const Key& key, bool split = false);

	};

}

#include "BTree-impl.h"