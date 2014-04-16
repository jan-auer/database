//
//  File.h
//  database
//
//  Created by Jan Michael Auer on 16/04/14.
//  Copyright (c) 2014 LightningSQL. All rights reserved.
//

#ifndef __database__File__
#define __database__File__

#include <sys/types.h>  
#include <string>
#include <vector>

namespace lsql {
	
	/**
	 * Represents a file containing typed data.
	 *
	 * @param Element The data type of the file.
	 */
	template<typename Element>
	class File {
		
	private:
		int fd;
		std::string path;
		
	public:
		
		/**
		 * Creates a new file for the given file descriptor.
		 * NOTE: The file is closed, when this object is destructed!
		 *
		 * @param fd A valid file descriptor.
		 */
		File(int fd);
		
		/**
		 * Creates a new file at the given location.
		 *
		 * @param path A fully qualified path to the file. The path
		 *             must exist, but the file does not.
		 */
		File(const std::string& path);
		
		/**
		 * Destructs this object. If the file is open, it will be
		 * closed automatically.
		 */
		~File();
		
		/**
		 * Opens the file. If @c write is @c true and does not exist,
		 * it will be created automatically.
		 *
		 * @param write Specifies whether to open the file in read or
		 *              write mode.
		 */
		bool open(bool write);
		
		/**
		 * Closes the file.
		 */
		bool close();
		
		/**
		 * Removes the file.
		 */
		bool remove();
		
		/**
		 * Allocates enough space to fit the given amount of elements.
		 *
		 * @param elementCount The number of elements to store in the file.
		 */
		bool allocate(off_t elementCount);
		
		/**
		 * Reads file contents into the given vector.
		 * If data does not fit into the vector, it will be resized first.
		 *
		 * @param data   A reference to the data vector.
		 * @param count  The number of elements to read.
		 * @param offset Number of elements to skip.
		 */
		bool readVector(std::vector<Element>& data, off_t count, off_t offset = 0);
		
		/**
		 * Writes all elements in the vector to the file.
		 * The file will automatically be resized, if the elements exceed
		 * the file size.
		 *
		 * @param data A reference to a vector containing the elements.
		 */
		bool writeVector(const std::vector<Element>& data);
		
	};
	
}

#include "File.cpp"

#endif /* defined(__database__File__) */
