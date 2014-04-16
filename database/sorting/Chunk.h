//
//  Chunk.h
//  database
//
//  Created by Jan Michael Auer on 12/04/14.
//  Copyright (c) 2014 LightningSQL. All rights reserved.
//

#ifndef __database__Chunk__
#define __database__Chunk__

#include <stdint.h>
#include <vector>
#include "File.h"

namespace lsql {

	/**
	 *
	 */
	template<typename Element>
	class Chunk {
		
		File<Element>& file;
		size_t offset, index;
		size_t size, totalSize;
		std::vector<Element> data;
		Element* curr;
		
	public:
		
		/**
		 *
		 */
		Chunk(File<Element>& file, off_t offset, size_t size, size_t totalSize = 0);
		
		/// Prevent the copy constructor from being called.
		Chunk(const Chunk& c) = delete;
		
		/// Prevent any assignments which could lead to duplication.
		Chunk& operator=(const Chunk& c) const = delete;
		
		/**
		 *
		 */
		const Element* next();
		
		/**
		 *
		 */
		const Element* current() const;
		
	private:

		/**
		 *
		 */
		void reload();
		
	};
	
}
		
#include "Chunk.cpp"

#endif /* defined(__database__Chunk__) */
