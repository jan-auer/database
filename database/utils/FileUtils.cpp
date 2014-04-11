//
//  Utils.cpp
//  database
//
//  Created by Jan Michael Auer on 11/04/14.
//  Copyright (c) 2014 LightningSQL. All rights reserved.
//

#include <fcntl.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <iostream>
#include "FileUtils.h"

using namespace std;

#if defined(__APPLE__)
int posix_fallocate(int fd, off_t offset, off_t len);
#endif

namespace lsql {

	int FileUtils::openRead(const char* location) {
		int fd = open(location, O_RDONLY);
		if (fd < 0) {
			cerr << "Cannot open file '" << location << "' for read access: "
			     << strerror(errno) << endl;
		}
		return fd;
	}
	
	int FileUtils::openWrite(const char* location) {
		int fd = open(location, O_CREAT|O_TRUNC|O_WRONLY, S_IRUSR|S_IWUSR);
		if (fd < 0) {
			cerr << "Cannot open file '" << location << "' for write access: "
			     << strerror(errno) << endl;
		}
		return fd;
	}
	
	bool FileUtils::close(int fd) {
		return ::close(fd) == 0;
	}
	
	template<typename T>
	bool FileUtils::allocate(int fd, off_t elementCount) {
		off_t fileSize = elementCount * sizeof(T);
		int ret = posix_fallocate(fd, 0, fileSize);
		
		if (ret == 0) {
			return true;
		} else {
			cerr << "Warning: Could not allocate file space: " << strerror(ret) << endl;
			return false;
		}
	}
	
	template<typename T>
	bool FileUtils::readVector(int fd, std::vector<T> &data, off_t elementCount, off_t elementOffset) {
		data.resize(elementCount);
		
		T* rawData = data.data();
		size_t rawSize = sizeof(T) * elementCount;

		ssize_t readSize = pread(fd, rawData, rawSize, elementOffset * sizeof(T));
		if (readSize < 0) {
			cerr << "Cannot read from file: " << strerror(errno) << endl;
			return false;
		} else {
			data.resize(readSize / sizeof(T));
			return true;
		}
	}
	
	template<typename T>
	bool FileUtils::writeVector(int fd, vector<T> &data) {
		const T* rawData = data.data();
		size_t rawSize = sizeof(T) * data.size();
		
		ssize_t writtenSize = write(fd, rawData, rawSize);
		if (writtenSize == rawSize) {
			return true;
		} else {
			cerr << "Cannot write to file: " << strerror(errno) << endl;
			return false;
		}
	}
	
}

# if defined(__APPLE__)

// dirty workaround to have sth similar to posix_fallocate on BSD Systems
// taken from: https://trac.transmissionbt.com/browser/branches/1.5x/libtransmission/fdlimit.c?rev=8203#L170
int posix_fallocate(int fd, off_t offset, off_t len) {
	fstore_t fst;
	fst.fst_flags = F_ALLOCATECONTIG;
	fst.fst_posmode = F_PEOFPOSMODE;
	fst.fst_offset = offset;
	fst.fst_length = len;
	fst.fst_bytesalloc = 0;
	
	return fcntl( fd, F_PREALLOCATE, &fst );
}

#endif
