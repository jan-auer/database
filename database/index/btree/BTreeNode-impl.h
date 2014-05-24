//
//  BTreeNode.h
//  database
//
//  Created by Felix Jankowski on 19.05.14.
//  Copyright (c) 2014 LightningSQL. All rights reserved.
//

#pragma once

#include <iostream>

#include "common/IDs.h"
#include "segment/Segment.h"

namespace lsql {


	template<class Key, class Comperator>
	class BTreeNode {

	public:
		enum NodeType {
			None, Inner, Leaf
		};

	private:
		/*** These are the different headers for each node ***/
		struct Header {
			NodeType type;
			uint64_t LSN = 0;
			uint32_t count;
			PID next;  //only for leaves
		};

		PID pid;
		uint32_t n; //Maximum number of Key/TID pairs in this node
		BufferFrame& bf;

		Header* header;
		Key* keys;
		TID* tids;

	public:

		/**
		 * Creates a new node in the B+-Tree.
		 *
		 * @param frame   The corresponding buffer frame containing page data.
		 * @param type		OPTIONAL: Whether this node is an inner or a leaf.
		 *								If specified, node is reset upon instantiation.
		 */
		BTreeNode(BufferFrame& frame, NodeType type = None)
		: pid(frame.getId()), bf(frame) {

			int32_t headerSize = 0;
			char* data = static_cast<char*>(frame.getData());
			header = reinterpret_cast<Header*>(data);

			if (type != None) {
				reset(type);
			}

			//Inner node has n pointers and n+1 TIDs
			if (header->type == Inner)
				headerSize = sizeof(Header) + sizeof(TID);
			else if (header->type == Leaf)
				headerSize = sizeof(Header);

			n = ((BufferFrame::SIZE) - headerSize) / (sizeof(TID) + sizeof(Key));

			keys = reinterpret_cast<Key*>(data + sizeof(Header));
			tids = reinterpret_cast<TID*>(data + headerSize);

		}


		/**
		 * Finds a TID
		 *
		 * @return PID of the next leaf if inner node, otherwise TID of key. NULL_TID if not found
		 */
		TID lookup(const Key& key) const {
			return NULL_TID;
		}


		/**
		 * Inserts a key/TID pair into the leaf
		 *
		 * @param key		A reference to the key that should be inserted
		 * @param tid		A reference to the key's tid if leaf node or of
		 *							the next node's PID if inner node
		 * @return			true on success, false in case of error
		 */
		bool insert(const Key& key, const TID& tid) {
			return false;
		}


		/**
		 * Removes a key from the leaf. Logic simplified by accepting
		 * underfull pages.
		 *
		 * @param key		A reference to the key that should be removed
		 * @return			true if successful, false if key has not been found
		 */
		bool remove(Key& key) {
			return false;
		}


		/**
		 * Splits the current node into two seperate ones
		 * Used for Concurrent Access (2), slide 40, chapter 3
		 *
		 * @return		A PID of the new (2nd) leaf, created by the split
		 */
		PID splitNode() {
			return NULL_PID;
		}


		/**
		 * Returns whether a node is an inner or a leaf node
		 */
		NodeType getType() {
			return header->type;
		}


		uint32_t getFree() {
			return n - header->count;
		}


		BufferFrame& getFrame() {
			return bf;
		}

		/**
		 * Resets the node and deletes all data
		 */
		void reset() {
			header->count = 0;
			header->next = NULL_PID;
		}


		void reset(NodeType type) {
			reset();
			header->type = type;
		}
		
	};

}
