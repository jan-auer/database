//
//  File.cpp
//  database
//
//  Created by Jan Michael Auer on 16/04/14.
//  Copyright (c) 2014 LightningSQL. All rights reserved.
//

#include <cassert>
#include <cstdio>
#include <fcntl.h>
#include <sys/uio.h>
#include <unistd.h>
#include <iostream>
#include "File.h"

using namespace std;

namespace lsql {
	
	template<typename Element>
	File<Element>::File() {
		fd = fileno(tmpfile());
	}

	template<typename Element>
	File<Element>::File(int fd) : fd(fd) {}
	
	template<typename Element>
	File<Element>::File(const std::string& path, bool write) : fd(0), path(path) {
		open(write);
	}
	
	template<typename Element>
	File<Element>::~File() {
		close();
	}
	
	template<typename Element>
	int File<Element>::descriptor() const {
		return fd;
	}
	
	template<typename Element>
	bool File<Element>::open(bool write) {
		close();
		
		if (write) {
			fd = ::open(path.c_str(), O_CREAT|O_TRUNC|O_RDWR, S_IRUSR|S_IWUSR);
		} else {
			fd = ::open(path.c_str(), O_RDONLY);
		}
		
		if (fd <= 0) {
			cerr << "Cannot open file '" << path << "' for "
			     << (write ? "write" : "read") << " access: "
			     << strerror(errno) << endl;
			return false;
		}
		
		return true;
	}
	
	template<typename Element>
	bool File<Element>::close() {
		if (fd <= 0)
			return false;
		
		int ret = ::close(fd);
		fd = 0;
		
		return ret == 0;
	}
	
	template<typename Element>
	bool File<Element>::remove() {
		close();
		
		if (::remove(path.c_str()) == 0)
			return true;
		
		cerr << "Cannot delete file: " << strerror(errno) << endl;
		return false;
	}
	
	template<typename Element>
	bool File<Element>::allocate(off_t elementCount) {
		assert(fd > 0);
		assert(elementCount >= 0);
		
		off_t fileSize = elementCount * sizeof(Element);
		int ret = ftruncate(fd, fileSize);
		
		if (ret == 0) {
			return true;
		} else {
			cerr << "Could not allocate file space: " << strerror(ret) << endl;
			return false;
		}
	}
	
	template<typename Element>
	bool File<Element>::readVector(std::vector<Element>& data, off_t count, off_t offset) {
		data.resize(count);

		Element* rawData = data.data();
		off_t rawSize = data.size() * sizeof(Element);
		off_t rawOffset = offset * sizeof(Element);
		
		ssize_t readSize = read(rawData, rawSize, rawOffset);
		if (readSize < 0)
			return false;
		
		data.resize(readSize / sizeof(Element));
		return true;
	}
	
	template<typename Element>
	bool File<Element>::writeVector(const std::vector<Element>& data) {
		return write(data.data(), data.size() * sizeof(Element));
	}
	
	template<typename Element>
	ssize_t File<Element>::read(char* data, off_t size, off_t offset) {
		assert(fd > 0);
		assert(size >= 0);
		assert(offset >= 0);
		
		while (ssize_t readSize = ::pread(fd, data, size, offset)) {

			static ssize_t totalReadSize = 0;

			totalReadSize += readSize;

			if (readSize < 0) {
				cerr << "Cannot read from file: " << strerror(errno) << endl;
				return -1;
			} else if (readSize == 0) {
				//End of File or all requested bytes read
				return totalReadSize;
			}

			//less bytes returned than requested, continue reading
			//set values
			assert((offset += readSize) > 0);
			assert((size -= readSize) > 0);

			data += readSize; //pointer arithmetics


			//ToDo: Momentan wird Schleife einmal mit size = 0 ausgeführt, aus Performancegründen sollte dies noch mit einer Fallunterscheidung abgefangen werden
		}
	}
	
	template<typename Element>
	bool File<Element>::write(const char* data, off_t size, off_t offset) {
		assert(fd > 0);
		assert(size >= 0);
		assert(offset >= 0);

		while (ssize_t writtenSize = ::pwrite(fd, data, size, offset)) {

			if (writtenSize == size) {
				return true;
			} else if (writtenSize < 0) {
				cerr << "Cannot write to file: " << strerror(errno) << endl;
				return false;
			}

			offset += writtenSize;
			assert((size -= writtenSize) > 0);

			data += writtenSize; //pointer arithmetics

		}
	}
	
}
