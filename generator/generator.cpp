#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>
#include <unistd.h>

class RandomLong
{
		/// The state
		uint64_t state;
		
public:
		/// Constructor
		explicit RandomLong(uint64_t seed=88172645463325252ull) : state(seed) {}
		
		/// Get the next value
		uint64_t next() { state^=(state<<13); state^=(state>>7); return (state^=(state<<17)); }
		
		
};


# if defined(__APPLE__)

// dirty workaround to have sth similar to posix_fallocate on BSD Systems
// taken from: https://trac.transmissionbt.com/browser/branches/1.5x/libtransmission/fdlimit.c?rev=8203#L170

int posix_fallocate(int fd, off_t offset, off_t len)	 {
		
		fstore_t fst;
		fst.fst_flags = F_ALLOCATECONTIG;
		fst.fst_posmode = F_PEOFPOSMODE;
		fst.fst_offset = offset;
		fst.fst_length = len;
		fst.fst_bytesalloc = 0;
		
		return fcntl( fd, F_PREALLOCATE, &fst );
		
}

#endif

int main(int argc, char* argv[]) {
		if (argc < 3) {
				std::cerr << "usage: " << argv[0] << " <file name> <number of elements>" << std::endl;
				return -1;
		}
		RandomLong rand;
		unsigned n = atoi(argv[2]);
		if (n==0) {
				std::cerr << "invalid length: " << argv[2] << std::endl;
				return -1;
		}
		int fd, ret;
		if ((fd = open(argv[1], O_CREAT|O_TRUNC|O_WRONLY, S_IRUSR|S_IWUSR)) < 0) {
				std::cerr << "cannot open file '" << argv[1] << "': " << strerror(errno) << std::endl;
				return -1;
		}
		if ((ret = posix_fallocate(fd, 0, n*sizeof(uint64_t))) != 0)
				std::cerr << "warning: could not allocate file space: " << strerror(ret) << std::endl;
		for (unsigned i=0; i<n; ++i) {
				uint64_t x = rand.next();
				if (write(fd, &x, sizeof(uint64_t)) < 0) {
						std::cout << "error writing to " << argv[1] << ": " << strerror(errno) << std::endl;
				}
		}
		close(fd);
		return 0;
}
