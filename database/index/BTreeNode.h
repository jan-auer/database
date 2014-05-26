//
//  BTreeNode.h
//  database
//
//  Created by Jan Michael Auer on 26/05/14.
//  Copyright (c) 2014 LightningSQL. All rights reserved.
//

#pragma once

#include "common/IDs.h"
#include "buffer/BufferFrame.h"

namespace lsql {

	/**
	 *
	 */
	template<class Key, class Comperator>
	class BTreeNode {

	public:

		enum NodeType {
			None, Inner, Leaf
		};

	private:

		struct Header {
			NodeType type;
			uint64_t LSN = 0;
			size_t count;
			PID next;  //only for leafs
		};

		PID pid;
		size_t n; //Maximum number of Key/TID pairs in this node. Set in constructor

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
		BTreeNode(BufferFrame& frame, NodeType type = None);

		/**
		 * Finds a TID
		 *
		 * @return PID of the next leaf if inner node, otherwise TID of key.
		 *				 NULL_TID is returned if not found
		 */
		TID lookup(const Key& key, bool return_left = false) const;

		/**
		 * Inserts a key/TID pair into the leaf
		 *
		 * @param key		A reference to the key that should be inserted
		 * @param tid		A reference to the key's tid if leaf node or of
		 *							the next node's PID if inner node
		 * @return			true on success, false in case of error
		 */
		bool insert(const Key& key, const PID& tid);

		/**
		 * Removes a key from the leaf. Logic simplified by accepting
		 * underfull pages.
		 *
		 * @param key		A reference to the key that should be removed
		 * @return			true if successful, false if key has not been found
		 */
		bool remove(const Key& key);

		/**
		 * Splits the current node into two seperate ones
		 * Used for Concurrent Access (2), slide 40, chapter 3
		 *
		 * @return		A PID of the new (2nd) leaf, created by the split
		 */
		PID splitNode();

		/**
		 * Resets the node and deletes all data
		 *
		 * @param type	OPTIONAL: The type of the node to be set in the header.
		 */
		void reset();

		/**
		 *
		 */
		void reset(NodeType type);

		/**
		 * Returns whether a node is an inner or a leaf node
		 */
		NodeType getType();
		
		/**
		 *
		 */
		size_t getFree();
		
	};
	
}

#include "BTreeNode-impl.h"