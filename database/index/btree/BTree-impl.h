//
//  BTree.cpp
//  database
//
//  Created by Felix Jankowski on 19.05.14.
//  Copyright (c) 2014 LightningSQL. All rights reserved.
//

#include "BTree.h"

#include "BTreeNode.h"


namespace lsql {

	template<class Key, class Comperator>
	BTree::BTree(BufferManager& bufferManager, uint16_t segmentId, uint32_t pageCount = 0)
	: Segment(bufferManager, segmentId, pageCount) {}


	template<class Key, class Comperator>
	bool BTree::insert(const Key& key, const TID& tid) {
		BufferFrame& frame = findFreeFrame(record.getSize());

		BTreeNode bn(this, frame);
		TID id = bn.createSlot();
		bn.insert(id, record);

		unfixPage(frame, true);
		return id;
	}
	

}