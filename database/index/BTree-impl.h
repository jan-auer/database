//
//  BTree.h
//  database
//
//  Created by Felix Jankowski on 19.05.14.
//  Copyright (c) 2014 LightningSQL. All rights reserved.
//

#pragma once

#include <cassert>
#include <iostream>

#include "buffer/BufferManager.h"
#include "BTreeNode.h"
#include "BTree.h"

namespace lsql {

	template<class Key, class Comperator>
	BTree<Key, Comperator>::BTree(BufferManager& bufferManager, uint16_t segmentId, uint32_t pageCount)
	: size(0), root(NULL_PID), Segment(bufferManager, segmentId, pageCount) {
		//Create root node
		root = createNode(NodeType::Inner);
	}

	template<class Key, class Comperator>
	TID BTree<Key, Comperator>::lookup(const Key& key) {
		BufferFrame* leafFramePtr = findLeaf(key);
		assert(leafFramePtr != nullptr);

		BTreeNode<Key, Comperator> leaf = BTreeNode<Key, Comperator>(*leafFramePtr);
		assert(leaf.getType() == NodeType::Leaf);
		TID tid = leaf.lookup(key);

		unfixPage(*leafFramePtr, false);

		return tid;
	}
	
	template<class Key, class Comperator>
	std::vector<TID> BTree<Key, Comperator>::lookupRange(const Key& key) {
		return std::vector<TID>();
	}

	template<class Key, class Comperator>
	bool BTree<Key, Comperator>::insert(const Key& key, const TID& tid) {
		BufferFrame* leafFramePtr = findLeaf(key);
		assert(leafFramePtr != nullptr);

		BTreeNode<Key, Comperator> leaf = BTreeNode<Key, Comperator>(*leafFramePtr);
		assert(leaf.getType() == (NodeType::Leaf));

		bool success = leaf.insert(key, tid);
		unfixPage(*leafFramePtr, success);  //unfix leaf after writing to it

		return success;
	}

	template<class Key, class Comperator>
	bool BTree<Key, Comperator>::erase(const Key& key) {
		BufferFrame* leafFramePtr = findLeaf(key);
		assert(leafFramePtr != nullptr);

		BTreeNode<Key, Comperator> leaf = BTreeNode<Key, Comperator>(*leafFramePtr);
		assert(leaf.getType() == (NodeType::Leaf));

		bool success = leaf.remove(key);
		unfixPage(*leafFramePtr, success);
		return success;
	}

	template<class Key, class Comperator>
	uint64_t BTree<Key, Comperator>::getSize() {
		return size;
	}

	template<class Key, class Comperator>
	void BTree<Key, Comperator>::visualizeRecurse(PID pid, std::ostream& dataOut) {
		BufferFrame& frame = fixPage(pid, false);
		BTreeNode<Key, Comperator> node(frame);
		std::vector<PID> childPids = node.visualize(dataOut);
		unfixPage(frame, false);

		for(std::vector<PID>::iterator it = childPids.begin(); it != childPids.end(); ++it) {
			visualizeRecurse(*it, dataOut);
		}
	}

	template<class Key, class Comperator>
	void BTree<Key, Comperator>::visualize() {
		std::ostream& os = std::cout;
		os << "digraph myBTree {\n node [shape=record]; \n";

		visualizeRecurse(root, std::cout);

		os << " }";
	}

	template<class Key, class Comperator>
	PID BTree<Key, Comperator>::createNode(NodeType type) {
		PID id = addPage();
		BufferFrame& frame = fixPage(id, true);
		BTreeNode<Key, Comperator>(frame, type).reset();
		unfixPage(frame, true);
		return id;
	}

	template<class Key, class Comperator>
	BufferFrame* BTree<Key, Comperator>::findLeaf(const Key& key, bool splitFullNodes) {
		BufferFrame* parentFrame = &fixPage(root, splitFullNodes);
		BufferFrame* childFrame = nullptr;

		while(true) {
			BTreeNode<Key, Comperator> parentNode(*parentFrame);
			PID childPid = parentNode.lookup(key, true);
			assert(childPid != NULL_PID);

			childFrame = &fixPage(childPid, splitFullNodes);
			BTreeNode<Key, Comperator> childNode(*childFrame);

			//Split child if necessary
			bool parentIsDirty = false;
			if (splitFullNodes && childNode.getFree() < 1) {
				PID child2Pid = childNode.splitNode();
				//TODO: insert split node into
				//TODO: Split root does not work!
				parentIsDirty = true;
				
			}

			/**
			 
			 //Split node into leaf and newLeaf. Insert newLeaf into parent
			 if (leaf.getFree() < 1) {
			 BTreeNode<Key, Comperator> parentNode = BTreeNode<Key, Comperator>(findResult.first);

			 const PID newLeafId = leaf.splitNode();
			 BufferFrame& newLeafFrame = fixPage(newLeafId, false);
			 BTreeNode<Key, Comperator> newLeafNode = BTreeNode<Key, Comperator>(newLeafFrame);

			 parentNode.insert(newLeafNode.firstKey(), newLeafId);
			 unfixPage(parentFrame, true);		//unfix parent dirty
			 unfixPage(newLeafFrame, false);	//unfix newLeaf without changes
			 } else {
			 unfixPage(parentFrame, false);		//unfix parent without changes
			 }

			 
			 */

			unfixPage(*parentFrame, parentIsDirty);

			if (childNode.getType() == NodeType::Leaf)
				break;
			else
				parentFrame = childFrame;
		}

		return childFrame;
	}

}
