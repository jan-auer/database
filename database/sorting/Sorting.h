//
//  sorting.h
//  database
//
//  Created by Jan Michael Auer on 11/04/14.
//  Copyright (c) 2014 LightningSQL. All rights reserved.
//

#pragma once

#include <cstdint>
#include "utils/File.h"

namespace lsql {
	
	/**
	 * Sorts the specified file containing 64bit numbers using an
	 * external merge sort and stores the results into the 
	 * specified output file. The algorithm will not use more
	 * memory than specified in @c memSize.
	 *
	 * NOTE: The input and output files are expected to be open
	 * with the correct access rights.
	 *
	 * @param inputFile  A file which contains all numbers to sort.
	 * @param inputCount The number of elements in the input file.
	 * @param outputFile A file which will contain all output data.
	 * @param memSize    The amount of memory to be used for sorting.
	 */
	template<typename Element>
	void externalSort(File<Element>& inputFile, off_t inputCount, File<Element>& outputFile, size_t memSize);
	
}

#include "Sorting.cpp"
