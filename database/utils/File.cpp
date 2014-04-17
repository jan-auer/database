//
//  File.cpp
//  database
//
//  Created by Jan Michael Auer on 16/04/14.
//  Copyright (c) 2014 LightningSQL. All rights reserved.
//

#include <assert.h>
#include <stdio.h>
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
	ssize_t File<Element>::read(void* data, off_t size, off_t offset) {
		assert(fd > 0);
		assert(size >= 0);
		assert(offset >= 0);
		
		ssize_t readSize = ::pread(fd, data, size, offset);
		if (readSize < 0) {
			cerr << "Cannot read from file: " << strerror(errno) << endl;
			return -1;
		} else {
			return readSize;
		}
	}
	
	template<typename Element>
	bool File<Element>::write(const void* data, off_t size) {
		assert(fd > 0);
		assert(size >= 0);
		
		ssize_t writtenSize = ::write(fd, data, size);
		if (writtenSize == size) {
			return true;
		} else {
			cerr << "Cannot write to file: " << strerror(errno) << endl;
			return false;
		}
	}
	
}
