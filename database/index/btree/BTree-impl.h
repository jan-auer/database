//
//  BTree.h
//  database
//
//  Created by Felix Jankowski on 19.05.14.
//  Copyright (c) 2014 LightningSQL. All rights reserved.
//

#pragma once

#include <iostream>
#include <vector>
#include <cassert>

#include "buffer/BufferManager.h"
#include "BTreeNode-impl.h"

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
		PID root;

	public:

		/**
		 * Creates a wrapper for segments containing a B+-Tree.
		 *
		 * @param bufferManager The buffer manager instance.
		 * @param segmentId     The segment identifier.
		 * @param pageCount     The number of pages in this segment.
		 */
		BTree(BufferManager& bufferManager, uint16_t segmentId, uint32_t pageCount = 0)
		: size(0), root(NULL_PID), Segment(bufferManager, segmentId, pageCount) {
			//Create root node
			root = createNode(BTreeNode<Key, Comperator>::NodeType::Inner);
		}


		/**
		 * Inserts a new Key/TID-Pair into the BTree Index
		 *
		 * @param key		A const reference of the key to be inserted into the tree
		 * @param tid		A reference of the key's TID
		 */
		bool insert(const Key& key, const TID& tid) {
			return false;
		}


		/**
		 * Removes a key from the Index
		 *
		 * @param key		A const reference of the key to be removed from the index
		 */
		bool erase(const Key& key) {
			return false;
		}


		/**
		 * Finds a key and returns the key's TID. Retunrs a NULL_TID if not found
		 *
		 * @param	key		A const reference
		 */
		TID lookup(const Key& key) { //const {
			PID leafPID = findLeaf(key);
			if (leafPID == NULL_PID)
				return NULL_TID;

			BufferFrame& frame = fixPage(leafPID, false);

			BTreeNode<Key, Comperator> leaf = BTreeNode<Key, Comperator>(frame);
			assert(leaf.getType() == (BTreeNode<Key, Comperator>::NodeType::Leaf));
			TID tid = leaf.lookup(key);

			unfixPage(frame, false);

			return tid;
		}


		/**
		 * @return	FIXME: an iterator that allows to iterate over the result set
		 */
		std::vector<TID> lookupRange(const Key& key) const {
			return std::vector<TID>();
		}


		/**
		 * Getter for the size
		 *
		 * @return A uint64_t of the number of Keys in the index
		 */
		uint64_t getSize() {
			return size;
		}


		//TODO:
		void visualize();



	private:
		PID createNode(typename BTreeNode<Key, Comperator>::NodeType type) {
			PID id = addPage();
			BufferFrame& frame = fixPage(id, true);
			BTreeNode<Key, Comperator>(frame, type).reset();
			unfixPage(frame, true);
			return id;
		}


		/**
		 * This function recursively travels thourgh a B+-Tree and tries to find
		 * the leaf where a key should be stored. 
		 * Note: Can split full nodes for concurrent access
		 *
		 * @param Key		A reference to the key that should be found
		 * @param splitFullNodes		A bool value. If true, every traversed node that
		 *												does not have at least one empty slot is split
		 * @return			PID of the leave that should contain the key
		 */
		PID findLeaf(const Key& key, bool splitFullNodes = false) {
			return NULL_PID;
		}
	};

}
