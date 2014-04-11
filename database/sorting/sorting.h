//
//  sorting.h
//  database
//
//  Created by Jan Michael Auer on 11/04/14.
//  Copyright (c) 2014 LightningSQL. All rights reserved.
//

#ifndef __database__sorting__
#define __database__sorting__

#include <iostream>

namespace lsql {
	
	class Sorting {
	public:
		void externalSort(int fdInput, uint64_t size, int fdOutput, uint64_t memSize);
	};
	
}

#endif /* defined(__database__sorting__) */
