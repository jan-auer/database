#include <iostream>
#include "FileUtils.h"

using namespace std;
using namespace lsql;

const int BUFFER_SIZE = 1000;

class RandomLong {
	
private:
	uint64_t state;
	
public:
	explicit RandomLong(uint64_t seed=88172645463325252ull) : state(seed) {}
	
	uint64_t next() {
		state^=(state<<13);
		state^=(state>>7);
		return (state^=(state<<17));
	}
	
};

int main(int argc, char* argv[]) {
	RandomLong rand;
	int fd;
	unsigned int n;

	if (argc < 3) {
		cerr << "usage: " << argv[0] << " <file name> <number of elements>" << endl;
		return -1;
	}
	
	n = atoi(argv[2]);
	if (n == 0) {
		cerr << "invalid length: " << argv[2] << endl;
		return -1;
	}
	
	fd = FileUtils::openWrite(argv[1]);
	if (fd < 0)
		return -1;
	
	if (!FileUtils::allocate<uint64_t>(fd, n))
		return -1;
	
	vector<uint64_t> data(BUFFER_SIZE);
	for (int i = 0; i < n; i++) {
		data.push_back(rand.next());
		if (i % BUFFER_SIZE == 0) {
			if (!FileUtils::writeVector(fd, data))
				return -1;
			
			data.clear();
		}
	}
	
	if (!FileUtils::writeVector(fd, data))
		return -1;
	
	if (!FileUtils::close(fd))
		return -1;
	
	return 0;
}
