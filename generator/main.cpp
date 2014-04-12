#include <iostream>
#include "Generator.h"

using namespace std;
using namespace lsql;

const int BUFFER_SIZE = 1000;

int main(int argc, char* argv[]) {
	if (argc < 3) {
		cerr << "usage: " << argv[0] << " <file name> <number of elements>" << endl;
		return 1;
	}
	
	Generator gen(BUFFER_SIZE);
	bool success = gen.generate(argv[1], atoi(argv[2]));
	
	return success ? 0 : 1;
}
