//
//  BTreeNode.h
//  database
//
//  Created by Felix Jankowski on 19.05.14.
//  Copyright (c) 2014 LightningSQL. All rights reserved.
//

#pragma once

#include "BTreeNode.h"

namespace lsql {

	template<class Key, class Comperator>
	BTreeNode<Key, Comperator>::BTreeNode(BufferFrame& frame, NodeType type)
	: pid(frame.getId()) {

		int32_t headerSize = 0;
		char* data = static_cast<char*>(frame.getData());
		header = reinterpret_cast<Header*>(data);

		if (type != None)
			reset(type);

		//Inner node has n pointers and n+1 TIDs
		if (header->type == Inner)
			headerSize = sizeof(Header) + sizeof(TID);
		else if (header->type == Leaf)
			headerSize = sizeof(Header);

		n = ((BufferFrame::SIZE) - headerSize) / (sizeof(TID) + sizeof(Key));

		keys = reinterpret_cast<Key*>(data + sizeof(Header));
		tids = reinterpret_cast<TID*>(data + headerSize);
	}

	template<class Key, class Comperator>
	TID BTreeNode<Key, Comperator>::lookup(const Key& key, bool return_left) const {
		int i;
		Comperator comp;

		for (i = 0; i < header->count; ++i) {
			if (comp(keys[i], key))
				break;
		}

		if (i == header->count && !return_left)
			return NULL_TID;

		return tids[i];
	}

	template<class Key, class Comperator>
	bool BTreeNode<Key, Comperator>::insert(const Key& key, const PID& tid) {  //PID vs TID ??
		if(header->count == n)
			return false;

		assert(header->count < n);

		size_t position = (header->count)++;

		*(keys + position) = key;
		*(tids + position) = tid;

		return true;
	}

	template<class Key, class Comperator>
	bool BTreeNode<Key, Comperator>::remove(const Key& key) {
		int i;
		Comperator comp;

		for (i = 0; i < header->count; ++i) {
			if (comp(*(keys + i),key))
				break;
		}

		if (i == header->count)
			return false;

		assert(i > 0);
		assert(i < header->count);

		//overwrite element number i with element number i + 1
		memmove(keys + i, keys + i + 1, (header->count - i - 1) * sizeof(*keys));
		memmove(tids + i, tids + i + 1, (header->count - i - 1) * sizeof(*tids));

		--(header->count);

		return true;
	}

	template<class Key, class Comperator>
	PID BTreeNode<Key, Comperator>::splitNode() {
		return NULL_PID;
	}

	template<class Key, class Comperator>
	std::vector<PID> BTreeNode<Key, Comperator>::visualize(std::ostream& dataOut) {

		std::vector<PID> childPids;

		//print the node header including node pid
		dataOut << "node" << pid << " [shape=record, label=\"<count> " << header->count
						<< " | <isLeaf> " << ((header->type == NodeType::Leaf) ? "true" : "false");

		//print the keys, sequentally iterated and numbered
		for (int i = 0; i < n; ++i) {
			dataOut << " | <key" << i << "> ";
			if(i < header->count)
				dataOut << &keys[i];
		}


		if (header->type == NodeType::Inner) {

			//Print the pointers to the children and put them into childPids vector
			for (int i = 0; i < n + 1; ++i) {
				dataOut << " |   <ptr" << i <<">";
				if(i < header->count + 1 ) {
					childPids.push_back(tids[i]);
					dataOut << " *";
				}
			}

			dataOut << "\"];" << std::endl;

			//pointer to child pages
			for (int i = 0; i < header->count + 1; ++i) {
				dataOut << "node" << pid << ":ptr" << i << " -> leaf" << tids[i] << ":count;" << std::endl;
			}

		} else { //Leaf

			for (int i = 0; i < header->count; ++i) {
				dataOut << " | " << tids[i];
			}

			if (header->next != NULL_PID) {
				dataOut << " | <next> *\"];" << std::endl;
				dataOut << "node" << pid << ":next -> leaf" << header->next << ":count;" << std::endl;
			} else
				dataOut << " | <next> \"];" << std::endl;

		}

		return childPids;
	}

	template<class Key, class Comperator>
	void BTreeNode<Key, Comperator>::reset() {
		header->count = 0;
		header->next = NULL_PID;
	}

	template<class Key, class Comperator>
	void BTreeNode<Key, Comperator>::reset(NodeType type) {
		reset();
		if (type != None)
			header->type = type;
	}

	template<class Key, class Comperator>
	NodeType BTreeNode<Key, Comperator>::getType() {
		return header->type;
	}

	template<class Key, class Comperator>
	size_t BTreeNode<Key, Comperator>::getFree() {
		return n - header->count;
	}

}
