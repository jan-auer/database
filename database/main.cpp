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
	int input = FileUtils::openRead("numbers");
	int output = FileUtils::openWrite("sorted");
	
	Sorting::externalSort(input, 20, output, 10 * sizeof(uint64_t));
	
	FileUtils::close(output);
	FileUtils::close(input);
	
    return 0;
}

