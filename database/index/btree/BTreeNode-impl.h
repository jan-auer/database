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

		enum NodeType {
			Inner, Leaf
		};

		/*** These are the different headers for each node ***/
		struct Header {
			NodeType type;
			uint64_t LSN = 0;
			uint32_t count;
		};

		struct InnerHeader : Header	{
			const NodeType type = Inner;
		};

		struct LeafHeader : Header {
			const NodeType type = Leaf;
			uint64_t LSN = 0;
			uint32_t count;
			PID next;
		};

		PID pid;
		uint32_t n; //Maximum number of Key/TID pairs in this node

		Header* header;
		Key* keys;
		TID* tids;

	public:

		/**
		 * Creates a new node in the B+-Tree.
		 *
		 * @param frame   The corresponding buffer frame containing page data.
		 * @param type		Whether this node is an inner or a leaf
		 */
		BTreeNode(BufferFrame& frame, NodeType type)
		: pid(frame.getId()) {

			int32_t headerSize = 0;
			char* data = static_cast<char*>(frame.getData());

			if (type == Inner) {
				header = static_cast<InnerHeader*>(data);
				//Inner node has n pointers and n+1 TIDs
				headerSize = sizeof(InnerHeader) + sizeof(TID);

				keys = reinterpret_cast<Key*>(data + sizeof(InnerHeader));
			} else if (type == Leaf) {
				header = static_cast<LeafHeader*>(data);
				headerSize = sizeof(LeafHeader);

				keys = reinterpret_cast<Key*>(data + sizeof(LeafHeader));
			}

			n = ((BufferFrame::SIZE) - headerSize) / (sizeof(TID) + sizeof(Key));
			tids = reinterpret_cast<TID*>(data + headerSize);

		}


		/**
		 * Finds a TID
		 *
		 * @return PID of the next leaf if inner node, otherwise TID of key. NULL_TID if not found
		 */
		TID lookup(Key& key) const;


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
		 * Removes a key from the Leaf.
		 *
		 * @param key		A reference to the key that should be removed
		 * @return			true if successful, false if key has not been found
		 */
		bool remove(Key& key);


		/**
		 * Splits the current node into two seperate ones
		 * Used for Concurrent Access (2), slide 40, chapter 3
		 *
		 * @return		A PID of the new (2nd) leaf, created by the split
		 */
		PID splitNode();

	};

}
