//
//  File.cpp
//  database
//
//  Created by Jan Michael Auer on 16/04/14.
//  Copyright (c) 2014 LightningSQL. All rights reserved.
//

#include <assert.h>
#include <fcntl.h>
#include <sys/uio.h>
#include <unistd.h>
#include <iostream>
#include "File.h"

using namespace std;

namespace lsql {
	
	template<typename Element>
	File<Element>::File(int fd) : fd(fd) {}
	
	template<typename Element>
	File<Element>::File(const std::string& path) : fd(0), path(path) {}
	
	template<typename Element>
	File<Element>::~File() {
		close();
	}
	
	template<typename Element>
	bool File<Element>::open(bool write) {
		close();
		
		if (write) {
			fd = ::open(path.c_str(), O_CREAT|O_TRUNC|O_WRONLY, S_IRUSR|S_IWUSR);
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
		if (::remove(path) == 0)
			return true;
		
		cerr << "Cannot delete file: " << strerror(errno) << endl;
		return false;
	}
	
	template<typename Element>
	bool File<Element>::allocate(off_t elementCount) {
		assert(fd > 0);
		
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
		assert(fd > 0);
		
		data.resize(count);
		Element* rawData = data.data();
		size_t rawSize = sizeof(Element) * count;
		
		ssize_t readSize = pread(fd, rawData, rawSize, offset * sizeof(Element));
		if (readSize < 0) {
			cerr << "Cannot read from file: " << strerror(errno) << endl;
			return false;
		} else {
			data.resize(readSize / sizeof(Element));
			return true;
		}
	}
	
	template<typename Element>
	bool File<Element>::writeVector(const std::vector<Element>& data) {
		assert(fd > 0);
		
		const Element* rawData = data.data();
		size_t rawSize = sizeof(Element) * data.size();
		
		ssize_t writtenSize = write(fd, rawData, rawSize);
		if (writtenSize == rawSize) {
			return true;
		} else {
			cerr << "Cannot write to file: " << strerror(errno) << endl;
			return false;
		}
	}
	
}
