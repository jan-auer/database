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
	 * @param Keys       The datatype of the indexed values
	 * @param Comparator A class providing the < Comparator for each data type
	 */
	template<class Key, class Comparator>
	class BTree : protected Segment, private Comparator {

		using Comparator::compare;

		/** Shortcut for BTree nodes. */
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
		 * Looks up the target TID for a record identified by @c key.
		 *
		 * @param key A reference to the key of the record.
		 * @return The TID of the entry, or NULL_TID.
		 */
		TID lookup(const Key& key);

		/**
		 * Looks up all entries with keys between @c from and @c to. 
		 * TODO: Implement this method.
		 * 
		 * @param from A reference to the key of the first element.
		 * @param to   A reference to the key of the last element (including).
		 *
		 * @return An iterator that allows to iterate over the result set.
		 */
		std::vector<TID> lookupRange(const Key& from, const Key& to);

		/**
		 * Inserts a new pair of key and TID into the BTree index.
		 *
		 * This method will fail, if a record with the specified key already exists
		 * in the index. The BTree will automatically grow, if there is not enough
		 * space in the underlying data structure.
		 *
		 * @param key A const reference of the key to be inserted into the tree
		 * @param tid A reference of the TID of the indexed record.
		 *
		 * @return True if the key could be inserted; otherwise false.
		 */
		bool insert(const Key& key, const TID& tid);

		/**
		 * Removes an entry from the index, identified by the given key.
		 *
		 * @param key A reference of the key to be removed from the index.
		 * @return True if the the entry has been deleted; otherwise false.
		 */
		bool erase(const Key& key);

		/**
		 * Returns the number of elements in the tree.
		 */
		uint64_t getSize();

		/**
		 * Renders the tree into the DOT format for GraphWIZ. The method will use 
		 * std::cout for rendering.
		 */
		void visualize();

	private:

		/**
		 * Recursively vizualizes the node specified by pid.
		 *
		 * @param pid     The id of the BufferFrame to visualize.
		 * @param dataOut An output stream to pipe out.
		 */
		void visualizeRecurse(PID pid, std::ostream& dataOut);

		/**
		 * This function recursively travels thourgh a B+-Tree and tries to find
		 * the leaf where a key should be stored.
		 *
		 * While traversing through the tree, this method will automatically split 
		 * full nodes. To prevent race conditions and dead locks, lock coupling 
		 * (latching) is used.
		 *
		 * The returned BufferFrame is locked by this method. If split is @c true,
		 * an exclusive lock is used. See @c BufferFrame::lock for more information.
		 *
		 * @param key   A reference to the key that should be found.
		 * @param split Whether to split nodes while traversing or not.
		 * @return      A reference to a fixed BufferFrame of the corresponding leaf.
		 */
		BufferFrame& findLeafFrame(const Key& key, bool split = false);

	};

}

#include "BTree-impl.h"