//
//  File.h
//  database
//
//  Created by Jan Michael Auer on 16/04/14.
//  Copyright (c) 2014 LightningSQL. All rights reserved.
//

#ifndef __database__File__
#define __database__File__

#include <stdint.h>
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
		 * Creates a new temporary file with a unique name.
		 * The file is automatically opened in w+ mode and removed
		 * when closed or destructed.
		 */
		File();
		
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
		 * @param path  A fully qualified path to the file. The path
		 *              must exist, but the file does not.
		 * @param write Specifies whether to open the file in read
		 *              or write mode.
		 */
		File(const std::string& path, bool write = false);
		
		/**
		 * Destructs this object. If the file is open, it will be
		 * closed automatically.
		 */
		~File();
		
		/**
		 * Gets the internal file descriptor. If the file is not 
		 * opened, this method returns zero.
		 *
		 * @return The internal file descriptor.
		 */
		int descriptor() const;
		
		/**
		 * Opens the file. If @c write is @c true and does not exist,
		 * it will be created automatically.
		 *
		 * @param write Specifies whether to open the file in read or
		 *              write mode.
		 */
		bool open(bool write);
		
		/**
		 * Closes the file, if it is open in any mode.
		 */
		bool close();
		
		/**
		 * Removes the file.
		 * If the file is open, it will be closed first.
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
		 * @param offset Number of elements to skip in the file.
		 */
		bool readVector(std::vector<Element>& data, off_t count, off_t offset = 0);
		
		/**
		 * Writes all elements from the vector to the file.
		 * The file will automatically be resized, if the elements exceed
		 * the file size.
		 *
		 * @param data A reference to a vector containing the elements.
		 */
		bool writeVector(const std::vector<Element>& data);
		
		/**
		 * Reads file contents into the given array.
		 *
		 * @param data   A pointer to the data array.
		 * @param size   The length of the file segment to read.
		 * @param offset Number of elements to skip in the file.
		 */
		ssize_t read(void* data, off_t size, off_t offset = 0);
		
		/**
		 * Writes all elements from the array to the file.
		 * The file will automatically be resized, if the elements exceed
		 * the file size.
		 *
		 * @param data  A reference to a vector containing the elements.
		 * @param size  The length of the memory segment to write.
		 * @param offset Number of elements to skip in the file.
		 */
		bool write(const void* data, off_t size, off_t offset = 0);
		
	};
	
}

#include "File.cpp"

#endif /* defined(__database__File__) */
