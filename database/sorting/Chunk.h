//
//  Chunk.h
//  database
//
//  Created by Jan Michael Auer on 12/04/14.
//  Copyright (c) 2014 LightningSQL. All rights reserved.
//

#pragma once

#include <cstdint>
#include <vector>

#include "utils/File.h"

namespace lsql {

	/**
	 * Represents the chunk of a segment within a file. 
	 * This will later be replaced by the segment manager.
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
		 * Creates and initializes a new chunk. 
		 * At this moment no data is loaded. The file is accessed for the first
		 * time after a call to Chunk::next().
		 *
		 * @param file      A reference to an open file.
		 * @param offset    The starting offset of the segment within the file.
		 * @param size      The maximum size of this chunk.
		 * @param totalSize The total size of the segment.
		 */
		Chunk(File<Element>& file, off_t offset, size_t size, size_t totalSize = 0);
		
		/// Prevent the copy constructor from being called.
		Chunk(const Chunk& c) = delete;
		
		/// Prevent any assignments which could lead to duplication.
		Chunk& operator=(const Chunk& c) const = delete;
		
		/**
		 * Loads the next element from the segment. If no more elements are
		 * left in the segment, 0 is returned. 
		 *
		 * @return A pointer to the next element or null.
		 */
		const Element* next();
		
		/**
		 * Returns the current element.
		 * NOTE: Chunk::next() has to be called at least once, before invoking
		 * this method. All prior calls return an undefined pointer.
		 *
		 * @return A pointer to the current element.
		 */
		const Element* current() const;
		
	private:

		/**
		 * Reloads the next chunk of the segment from the file.
		 */
		void reload();
		
	};
	
}
		
#include "Chunk.cpp"
