//
//  Chunk.h
//  database
//
//  Created by Jan Michael Auer on 12/04/14.
//  Copyright (c) 2014 LightningSQL. All rights reserved.
//

#ifndef __database__Chunk__
#define __database__Chunk__

#include <iostream>
#include <vector>

namespace lsql {

	//
	template<typename T>
	class Chunk {
		
		int fd;
		size_t offset, index;
		size_t size, totalSize;
		std::vector<T> data;
		T* curr;
		
	public:
		
		//
		Chunk(int fd, off_t offset, size_t size, size_t totalSize = 0);
		
		// Prevent the copy constructor from being called.
		Chunk(const Chunk& c) = delete;
		
		// Prevent any assignments which could lead to duplication.
		Chunk& operator=(const Chunk& c) const = delete;
		
		//
		const T* next();
		
		//
		const T* current() const;
		
	private:

		//
		void reload();
		
	};
	
}
		
#include "Chunk.cpp"

#endif /* defined(__database__Chunk__) */
