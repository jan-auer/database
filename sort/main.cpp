//
//  main.cpp
//  database
//
//  Created by Jan Michael Auer on 09/04/14.
//  Copyright (c) 2014 LightningSQL. All rights reserved.
//

#include <iostream>
#include <string.h>
#include <chrono>

#include "Generator.h"
#include "utils/File.h"
#include "sorting/Sorting.h"

using namespace lsql;

typedef chrono::high_resolution_clock Clock;

const int GENERATOR_BUFFER_SIZE = 1000;

int main(int argc, char* argv[]) {
	if (argc < 5 || argc > 6) {
		cerr << "usage: " << argv[0] << " <inputFile> <outputFile> <fileSizeInMB> <memoryBufferInMB> (-Generate)" << endl;
		return 1;
	}
	
	uint64_t elementCount = std::stoi(argv[3]) / sizeof(uint64_t) * 1024 * 1024;
	
	// std::string implements ==, however comparison with "..." is undefined
	if (argc == 6 && strncmp("-Generate", argv[5], 9) == 0) {
		cout << "Generating " << elementCount << " random numbers in '" << argv[1] << "'" << endl;
		
		Generator gen(GENERATOR_BUFFER_SIZE);
		bool success = gen.generate(argv[1], elementCount);
		
		if (!success) {
			cerr << "Generation of input File " << argv[1] << " failed!";
			return 2;
		}
	}
	
	File<uint64_t> input(argv[1], false);
	File<uint64_t> output(argv[2], true);
	
	uint64_t memSize = std::stoi(argv[4]) * 1024 * 1024; // MiB to B

	cout << "Starting to sort..." << endl;
	auto s = Clock::now();
	externalSort(input, elementCount, output, memSize);
	auto e = Clock::now();
	cout << "Sorting completed in " << chrono::duration_cast<chrono::milliseconds>(e-s).count() << "ms." << endl;

    return 0;
}
