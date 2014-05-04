//
//  Chunk.cpp
//  database
//
//  Created by Jan Michael Auer on 12/04/14.
//  Copyright (c) 2014 LightningSQL. All rights reserved.
//

#include <vector>       // std::vector
#include <algorithm>    // std::min

#include "Chunk.h"

using namespace std;

namespace lsql {

	template<typename Element>
	Chunk<Element>::Chunk(File<Element>& file, off_t offset, size_t size, size_t totalSize) :
		file(file),
		offset(offset),
		size(size),
		totalSize(totalSize),
		index(0),
		curr(0){}
	
	template<typename Element>
	const Element* Chunk<Element>::next() {
		if (index >= totalSize)
			return 0;
		
		if (index % size == 0)
			reload();
		
		return curr = &data[index++ % size];
	}
	
	template<typename Element>
	const Element* Chunk<Element>::current() const {
		return curr;
	}
	
	template<typename Element>
	void Chunk<Element>::reload() {
		file.readVector(data, min(size, totalSize - index), offset + index);
	}
	
}
