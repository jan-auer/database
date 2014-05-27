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
	 * Possible types of BTree nodes: Inner and Leaf.
	 */
	enum NodeType {
		None,
		Inner,
		Leaf
	};

	/**
	 * Wrapper class for nodes of B+-Trees.
	 */
	template<typename Key, typename Comparator>
	class BTreeNode : private Comparator {

		using Comparator::compare;

		/** Header for BTree nodes. */
		struct Header {
			NodeType type;
			size_t count;
			PID next;  //only for leafs
		};

		PID pid;
		size_t n;

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
		 * @param found      The key of this pointer will be set to the actually 
		 *                   resolved key.
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
		 * Splits contents into the specified other node.
		 * 
		 * The other node receives the bigger half of the current contents. Additionally,
		 * the next pointer is set correctly. Behavior is based on the node type:
		 *  - INNER NODE: The median value will be moved to the parent node, so it
		 *    is neither in the left nor the right split node. Child pointers are
		 *    set correctly.
		 *  - LEAF NODE: There are no child pointers. The median value stays in this
		 *    node.
		 *
		 * This method will fail, if it is not full yet.
		 *
		 * @return The key which has to be inserted into the parent.
		 */
		const Key& splitInto(BTreeNode<Key, Comparator>& other);

		/**
		 * Updates the specified key with @c newKey. This is useful, when child nodes
		 * are split up and the upper bound of one child has changed.
		 *
		 * @param oldKey The old key to be updated.
		 * @param newKey The new key.
		 */
		void switchKey(const Key& oldKey, const Key& newKey);

		/**
		 * Returns whether a node is an inner or a leaf node.
		 */
		NodeType getType() const;

		/**
		 * Determines whether this node has reached it's capacity or not.
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
		 * Initializes this node. This eventually overwrites all node contents.
		 *
		 * @param data A pointer to the internal data of this node.
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
		 * Resolves the position of the key within this node. If they key is not
		 * contained, a position is returned, the key should have.
		 * 
		 * @param key A reference to the key.
		 * 
		 * @return An index between 0 and N.
		 */
		size_t findPos(const Key& key) const;

		/**
		 * Moves all entries within this page to insert new ones or remove old ones.
		 *
		 * @param offset   The first element to move.
		 * @param distance The distance to move the elements.
		 */
		void moveEntries(size_t offset, ssize_t distance);
		
	};
	
}

#include "BTreeNode-impl.h"