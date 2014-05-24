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
		char* data;
		Header* header;

	public:

		/**
		 * Creates a new node in the B+-Tree.
		 *
		 * @param frame   The corresponding buffer frame containing page data.
		 */
		BTreeNode(BufferFrame& frame, NodeType type);


		TID lookup(Key& key) const;

		bool insert(const Key& key, const TID& tid);

		bool remove(Key& key);

		std::pair<TID, TID> splitNode();

	};

}


#include "BTreeNode-impl.h"