//
//  main.cpp
//  database
//
//  Created by Jan Michael Auer on 09/04/14.
//  Copyright (c) 2014 LightningSQL. All rights reserved.
//

#include <iostream>

#include "FileUtils.h"
#include "Sorting.h"

using namespace lsql;

int main(int argc, const char * argv[])
{
	int numbers = FileUtils::openRead("numbers");
	Sorting::externalSort(numbers, 10, 0, 6 * sizeof(uint64_t));
	
	std::cout << "Hello, World!\n";
    return 0;
}

