//
//  Chunk.cpp
//  database
//
//  Created by Jan Michael Auer on 12/04/14.
//  Copyright (c) 2014 LightningSQL. All rights reserved.
//

#include <vector>       // std::vector
#include <algorithm>    // std::min

#include "FileUtils.h"
#include "Chunk.h"

using namespace std;

namespace lsql {

	template<typename T>
	Chunk<T>::Chunk(int fd, off_t offset, size_t size, size_t totalSize) :
		fd(fd),
		offset(offset),
		size(size),
		totalSize(totalSize),
		index(0),
		curr(0){}
	
	template<typename T>
	const T* Chunk<T>::next() {
		if (index >= totalSize)
			return 0;
		
		if (index % size == 0)
			reload();
		
		return curr = &data[index++ % size];
	}
	
	template<typename T>
	const T* Chunk<T>::current() const {
		return curr;
	}
	
	template<typename T>
	void Chunk<T>::reload() {
		FileUtils::readVector(fd, data, min(size, totalSize - index), offset + index);
//		for (auto e : data) cout << hex << "Reading: " << e << endl;
	}
	
}