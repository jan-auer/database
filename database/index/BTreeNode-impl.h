//
//  BTreeNode.h
//  database
//
//  Created by Felix Jankowski on 19.05.14.
//  Copyright (c) 2014 LightningSQL. All rights reserved.
//

#pragma once

#include <cassert>
#include <cstring>
#include "BTreeNode.h"

namespace lsql {

	template<typename Key, typename Comparator>
	BTreeNode<Key, Comparator>::BTreeNode(BufferFrame& frame, NodeType type)
	: pid(frame.getId()) {
		char* data = static_cast<char*>(frame.getData());
		header = reinterpret_cast<Header*>(data);

		if (type != None)
			reset(type);

		initialize(data);
	}

	template<typename Key, typename Comparator>
	TID BTreeNode<Key, Comparator>::lookup(const Key& key, bool allowRight, Key* found) const {
		size_t i = findPos(key);

		if (i == header->count && !allowRight)
			return NULL_TID;

		if (found != nullptr)
			*found = keys[i];

		return values[i];
	}

	template<typename Key, typename Comparator>
	bool BTreeNode<Key, Comparator>::insert(const Key& key, const TID& value) {
		assert(header->count <= n);
		if(header->count == n)
			return false;

		size_t pos = findPos(key);
		moveEntries(pos, 1);
		keys[pos] = key;
		values[pos] = value;

		return true;
	}

	template<typename Key, typename Comparator>
	bool BTreeNode<Key, Comparator>::remove(const Key& key) {
		size_t i = findPos(key);

		if (i == header->count || compare(key, keys[i]) != 0)
			return false;

		moveEntries(i + 1, -1);
		return true;
	}

	template<typename Key, typename Comparator>
	const Key& BTreeNode<Key, Comparator>::splitInto(BTreeNode<Key, Comparator>& other) {
		assert(header->count == n);

		other.header->next = header->next;
		header->next = other.pid;

		// Not happy about this...
		size_t i, c, o;
		if (header->type == NodeType::Inner) {
			i = n / 2;
			c = header->count = n / 2;
		} else {
			i = (n - 1) / 2;
			c = header->count = (n + 1) / 2;
		}
		other.header->count = o = n - i - 1;

		// Not happy about this either...
		std::memcpy(other.keys, keys + n - o, o * sizeof(Key));
		if (header->type == NodeType::Inner) {
			std::memcpy(other.values, values + n - o, (o + 1) * sizeof(TID));
		} else {
			std::memcpy(other.values, values + n - o, o * sizeof(TID));
		}

		return keys[header->count - 1];
	}

	template<typename Key, typename Comparator>
	void BTreeNode<Key, Comparator>::switchKey(const Key& oldKey, const Key& newKey) {
		size_t i = findPos(oldKey);
		keys[i] = newKey;
	}

	template<typename Key, typename Comparator>
	NodeType BTreeNode<Key, Comparator>::getType() const {
		return header->type;
	}

	template<typename Key, typename Comparator>
	bool BTreeNode<Key, Comparator>::isFull() const {
		return header->count >= n;
	}

	template<class Key, class Comparator>
	std::vector<PID> BTreeNode<Key, Comparator>::visualize(std::ostream& dataOut) {

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
					childPids.push_back(values[i]);
					dataOut << " *";
				}
			}

			dataOut << "\"];" << std::endl;

			//pointer to child pages
			for (int i = 0; i < header->count + 1; ++i) {
				dataOut << "node" << pid << ":ptr" << i << " -> leaf" << values[i] << ":count;" << std::endl;
			}

		} else { //Leaf

			for (int i = 0; i < header->count; ++i) {
				dataOut << " | " << values[i];
			}

			if (header->next != NULL_PID) {
				dataOut << " | <next> *\"];" << std::endl;
				dataOut << "node" << pid << ":next -> leaf" << header->next << ":count;" << std::endl;
			} else
				dataOut << " | <next> \"];" << std::endl;

		}
		
		return childPids;
	}

	template<typename Key, typename Comparator>
	void BTreeNode<Key, Comparator>::initialize(char* data) {
		assert(data != nullptr);
		assert(header->type != NodeType::None);

		size_t size = BufferFrame::SIZE - sizeof(Header);
		if (header->type == NodeType::Inner)
			size -= sizeof(TID);

		n = size / (sizeof(Key) + sizeof(TID));

		keys = reinterpret_cast<Key*>(data + sizeof(Header));
		values = reinterpret_cast<TID*>(data + sizeof(Header) + n * sizeof(Key));
	}

	template<typename Key, typename Comparator>
	void BTreeNode<Key, Comparator>::reset(NodeType type) {
		header->count = 0;
		header->next = NULL_PID;

		if (type != None)
			header->type = type;
	}

	template<typename Key, typename Comparator>
	size_t BTreeNode<Key, Comparator>::findPos(const Key& key) const {
		for (size_t pos = 0; pos < header->count; pos++)
			if (compare(key, keys[pos]) <= 0)
				return pos;

		return header->count;
	}

	template<typename Key, typename Comparator>
	void BTreeNode<Key, Comparator>::moveEntries(size_t offset, ssize_t distance) {
		std::memmove(keys + offset + distance, keys + distance, header->count - offset * sizeof(Key));
		std::memmove(values + offset + distance, values + distance, header->count - offset * sizeof(TID));
		header->count += distance;
	}

}
