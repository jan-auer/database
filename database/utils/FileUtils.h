//
//  Utils.h
//  database
//
//  Created by Jan Michael Auer on 11/04/14.
//  Copyright (c) 2014 LightningSQL. All rights reserved.
//

#ifndef __database__FileUtils__
#define __database__FileUtils__

#include <vector>

namespace lsql {
	
	class FileUtils {
	public:
		
		//
		static int openRead(const char* location);
		
		//
		static int openWrite(const char* location);
		
		//
		static bool close(int fd);
		
		//
		template<typename T>
		static bool allocate(int fd, off_t elementCount);
		
		//
		template<typename T>
		static std::vector<T> readAll();
		
		//
		template<typename T>
		static bool writeAll(int fd, std::vector<T> &data);
		
	};
	
}

#include "FileUtils.cpp"

#endif /* defined(__database__Utils__) */
