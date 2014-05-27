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
	enum NodeType {
		None,
		Inner,
		Leaf
	};

	/**
	 *
	 */
	template<typename Key, typename Comparator>
	class BTreeNode : private Comparator {

		using Comparator::compare;

		/**
		 *
		 */
		struct Header {
			NodeType type;
			size_t count;
			PID next;  //only for leafs
		};

		PID pid;
		size_t n; //Maximum number of Key/TID pairs in this node. Set in constructor

		Header* header;
		Key* keys;
		TID* values;

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
		 * Searches for an entry with the corresponding TID.
		 *
		 * For better use of cache locality and branch prediction, this method uses
		 * a linear search instead of a binary search.
		 *
		 * @param key        The key to search for.
		 * @param allowRight Whether there is a right outer value or not.
		 *
		 * @return Returns an ID with the specified type.
		 */
		TID lookup(const Key& key, bool allowRight = false, Key* found = nullptr) const;

		/**
		 * Inserts a key/TID pair into the leaf
		 *
		 * @param key   A reference to the key that should be inserted
		 * @param value A reference to the value which is either a PID for inner
		 *              nodes or a TID for leaf nodes.
		 * @return			true on success, false in case of error
		 */
		bool insert(const Key& key, const TID& value);

		/**
		 * Removes a key from the leaf. Logic simplified by accepting
		 * underfull pages.
		 *
		 * @param key		A reference to the key that should be removed
		 * @return			true if successful, false if key has not been found
		 */
		bool remove(const Key& key);

		/**
		 *
		 */
		const Key& splitInto(BTreeNode<Key, Comparator>& other);

		/**
		 *
		 */
		void switchKey(const Key& oldKey, const Key& newKey);

		/**
		 * Returns whether a node is an inner or a leaf node
		 */
		NodeType getType() const;

		/**
		 *
		 */
		bool isFull() const;
		
		/**
		 * Prints the content
		 *
		 * @param ostream		A reference to the stream to which the output should
		 *									be printed
		 * @return					An array of child PIDs.
		 */
		std::vector<PID> visualize(std::ostream& dataOut);

	private:

		/**
		 *
		 */
		void initialize(char* data);

		/**
		 * Resets the node and deletes all data.
		 *
		 * If the optional node type parameter is specified, the node type will
		 * also be converted.
		 *
		 * @param type OPTIONAL: The type of the node to be set in the header.
		 */
		void reset(NodeType type = NodeType::None);

		/**
		 *
		 */
		size_t findPos(const Key& key) const;

		/**
		 *
		 */
		void moveEntries(size_t offset, ssize_t distance);
		
	};
	
}

#include "BTreeNode-impl.h"