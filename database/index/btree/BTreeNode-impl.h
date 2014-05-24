//
//  BTreeNode.cpp
//  database
//
//  Created by Felix Jankowski on 19.05.14.
//  Copyright (c) 2014 LightningSQL. All rights reserved.
//

#include "BTreeNode.h"

namespace lsql {

	template<class Key, class Comperator>
	BTreeNode::BTreeNode(BufferFrame& frame, NodeType type)
	: pid(frame.getId()) {


		int32_t headerSize = sizeof(Header) + (header->count + 1) * sizeof(Slot);
		return int32_t(BufferFrame::SIZE) - header->usedSpace - headerSize;

		header = static_cast<Header*>(frame.getData());

		if (type == Inner) {
			header = static_cast<InnerHeader*>(header);
			data = reinterpret_cast<char*>(data + sizeof(InnerHeader));
		} else if (type == Leaf) {
			header = static_cast<LeafHeader*>(header);
			data = reinterpret_cast<char*>(data + sizeof(LeafHeader));
		}

	}



}