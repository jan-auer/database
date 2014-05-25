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
			std::pair<BufferFrame&, PID> findResult = findLeaf(key);
			PID leafPID = findResult.second;
			assert(leafPID != NULL_PID);

			BufferFrame& frame = fixPage(leafPID, true);

			BTreeNode<Key, Comperator> leaf = BTreeNode<Key, Comperator>(frame);
			assert(leaf.getType() == (BTreeNode<Key, Comperator>::NodeType::Leaf));

			if (leaf.getFree() < 1) {
				leaf.splitNode();
				//ToDo: Decide where to insert
				//ToDo: Insert new leaf into parent
				unfixPage(findResult.first, true);
			} else {
				unfixPage(findResult.first, false);
			}

			bool success = leaf.insert(key, tid);
			unfixPage(leaf.getFrame(), true);

			return success;
		}


		/**
		 * Finds a key and returns the key's TID. Returns a NULL_TID if not found
		 *
		 * @param	key		A const reference
		 */
		TID lookup(const Key& key) { //const {
			std::pair<BufferFrame&, PID> findResult = findLeaf(key);
			PID leafPID = findResult.second;
			if (leafPID == NULL_PID) {
				unfixPage(findResult.first, false);
				return NULL_TID;
			}

			BufferFrame& frame = fixPage(leafPID, false);
			unfixPage(findResult.first, false);

			BTreeNode<Key, Comperator> leaf = BTreeNode<Key, Comperator>(frame);
			assert(leaf.getType() == (BTreeNode<Key, Comperator>::NodeType::Leaf));
			TID tid = leaf.lookup(key);

			unfixPage(leaf.getFrame(), false);

			return tid;
		}
		

		/**
		 * Removes a key from the Index
		 *
		 * @param key		A const reference of the key to be removed from the index
		 */
		bool erase(const Key& key) {
			BTreeNode<Key, Comperator>(root).reset();
			size = 0;
			pageCount = 1;
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
		std::pair<BufferFrame&, PID> findLeaf(const Key& key, bool splitFullNodes = false) {
			BufferFrame& frame = fixPage(NULL_TID, false);


			/*
			 if (leaf.getFree < 1) {
			 //ToDo: Lock parent frame first
			 leaf.split();
			 //ToDo: Decide where to insert
			 }
			 */

			std::pair<BufferFrame&, PID> pair = {frame, NULL_TID};
			return pair;
		}
	};

}
