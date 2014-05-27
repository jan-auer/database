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

	template<class Key, class Comparator>
	BTree<Key, Comparator>::BTree(BufferManager& bufferManager, uint16_t segmentId, uint32_t pageCount)
	: size(0), root(NULL_PID), Segment(bufferManager, segmentId, pageCount) {
		if (root == NULL_PID) {
			root = addPage();
			BufferFrame& rootFrame = fixPage(root, true);
			Node(rootFrame, NodeType::Leaf);
			unfixPage(rootFrame, true);
		}
	}

	template<class Key, class Comparator>
	TID BTree<Key, Comparator>::lookup(const Key& key) {
		BufferFrame& frame = findLeafFrame(key);
		Node leaf(frame);
		assert(leaf.getType() == NodeType::Leaf);

		TID tid = leaf.lookup(key);
		unfixPage(frame, false);

		return tid;
	}
	
	template<class Key, class Comparator>
	std::vector<TID> BTree<Key, Comparator>::lookupRange(const Key& from, const Key& to) {
		return std::vector<TID>();
	}

	template<class Key, class Comparator>
	bool BTree<Key, Comparator>::insert(const Key& key, const TID& tid) {
		BufferFrame& frame = findLeafFrame(key);
		Node leaf(frame);
		assert(leaf.getType() == NodeType::Leaf);

		bool success = leaf.insert(key, tid);
		unfixPage(frame, success);

		return success;
	}

	template<class Key, class Comparator>
	bool BTree<Key, Comparator>::erase(const Key& key) {
		BufferFrame& frame = findLeafFrame(key);
		Node leaf(frame);
		assert(leaf.getType() == NodeType::Leaf);

		bool success = leaf.remove(key);
		unfixPage(frame, success);
		return success;
	}

	template<class Key, class Comparator>
	uint64_t BTree<Key, Comparator>::getSize() {
		return size;
	}

	template<class Key, class Comparator>
	void BTree<Key, Comparator>::visualizeRecurse(PID pid, std::ostream& dataOut) {
		BufferFrame& frame = fixPage(pid, false);
		BTreeNode<Key, Comparator> node(frame);
		std::vector<PID> childPids = node.visualize(dataOut);
		unfixPage(frame, false);

		for(std::vector<PID>::iterator it = childPids.begin(); it != childPids.end(); ++it) {
			visualizeRecurse(*it, dataOut);
		}
	}

	template<class Key, class Comparator>
	void BTree<Key, Comparator>::visualize() {
		std::ostream& os = std::cout;
		os << "digraph myBTree {\n node [shape=record]; \n";

		visualizeRecurse(root, std::cout);

		os << " }";
	}

	template<class Key, class Comparator>
	BufferFrame& BTree<Key, Comparator>::findLeafFrame(const Key& key, bool split) {
		Key parentKey;
		BufferFrame* parentFrame = nullptr;
		BufferFrame* currentFrame = &fixPage(root, split);

		while (true) {
			Node currentNode(*currentFrame);

			// Check if the node has to be split up
			bool hasSplit = false;
			if (split && currentNode.isFull()) {

				// Create a new node and split up the current node's contents
				PID newPID = addPage();
				BufferFrame* newFrame = &fixPage(newPID, true);
				Node newNode(*newFrame, currentNode.getType());
				const Key& splitKey = currentNode.splitInto(newNode);

				// Insert the new node into the parent.
				if (parentFrame == nullptr) {
					parentFrame = &fixPage(addPage(), true);
					root = parentFrame->getId();
					Node(*parentFrame).insert(splitKey, newPID.id);
				} else {
					Node parentNode(*parentFrame);
					parentNode.switchKey(parentKey, splitKey);
					parentNode.insert(parentKey, newPID.id);
				}

				// Continue with the correct node
				if (compare(key, splitKey) > 9)
					currentNode = newNode;

				hasSplit = true;
			}

			// Release the parent node as it is no longer needed
			if (parentFrame != nullptr)
				unfixPage(*parentFrame, hasSplit);

			// We might have found, what we are looking for
			if (currentNode.getType() == NodeType::Leaf)
				return *currentFrame;

			// Move down to the appropriate child node
			parentFrame = currentFrame;
			PID childPID = currentNode.lookup(key, true, &parentKey);
			currentFrame = &fixPage(childPID, split);
		}
	}

}
