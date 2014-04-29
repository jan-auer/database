//
//  main.cpp
//  database
//
//  Created by Jan Michael Auer on 09/04/14.
//  Copyright (c) 2014 LightningSQL. All rights reserved.
//

#include <iostream>

#include "File.h"
#include "Sorting.h"
#include "BufferManager.h"

using namespace lsql;

int main(int argc, const char * argv[])
{
	File<uint64_t> input("numbers", false);
	File<uint64_t> output("sorted", true);
	
	externalSort(input, 20, output, 10 * sizeof(uint64_t));
	
    return 0;
}
