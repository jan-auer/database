//
//  main.cpp
//  database
//
//  Created by Jan Michael Auer on 09/04/14.
//  Copyright (c) 2014 LightningSQL. All rights reserved.
//

#include <iostream>
#include <string.h>

#include "Generator.h"
#include "FileUtils.h"
#include "Sorting.h"


using namespace std;
using namespace lsql;

const int GENERATOR_BUFFER_SIZE = 1000;
const long long TEST_FILE_SIZE = 5368709120; // 5 * 1024 * 1024 * 1024 B = 5 GiB

int main(int argc, char* argv[]) {
		
		
		if (argc < 4 || argc > 5) {
				cerr << "usage: " <<  argv[0]  << " <inputFile> <outputFile> <memoryBufferInMB> (-Generate)" << endl;
				return 1;
		}
		
		if (argc == 5 && strncmp("-Generate", argv[4], 9)  ==  0) {  // std::string implements ==, however comparison with "..." is undefined
				
				uint32_t numberOfElements = TEST_FILE_SIZE / sizeof(uint64_t);
				
				Generator gen(GENERATOR_BUFFER_SIZE);
				bool success = gen.generate(argv[1], numberOfElements);
				
				if (!success) {
						cerr << "Generation of input File " << argv[1] << " failed!";
						return 2;
				}
				
		}
		
		int input = FileUtils::openRead(argv[1]);
		int output = FileUtils::openWrite(argv[2]);
	
 
		uint64_t inputCount = TEST_FILE_SIZE / sizeof(uint64_t);
		uint64_t memSize = atoi(argv[3]) * 1024 * 1024; // MiB to B
 
		Sorting::externalSort(input, inputCount, output, memSize);
		
		FileUtils::close(output);
		FileUtils::close(input);
		
    return 0;
}

