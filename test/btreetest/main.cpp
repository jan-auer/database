
#include <string>
#include <cstdint>
#include <cassert>
#include <vector>
#include <sstream>
#include <stdlib.h>
#include <string.h>

#include "buffer/BufferManager.h"
#include "common/IDs.h"
#include "index/BTree.h"

using namespace lsql;

template<typename T>
int compare(const T& a, const T& b) {
	return (b < a) - (a < b);
}

/* Comparator functor for uint64_t*/
struct MyCustomUInt64Cmp {
	int compare(uint64_t a, uint64_t b) const {
		return ::compare(a, b);
	}
};

template <unsigned len>
struct Char {
	char data[len];
};

/* Comparator functor for char */
template <unsigned len>
struct MyCustomCharCmp {
	int compare(const Char<len>& a, const Char<len>& b) const {
		return memcmp(a.data, b.data, len);
	}
};

typedef std::pair<uint64_t, uint64_t> IntPair;

/* Comparator for IntPair */
struct MyCustomIntPairCmp {
	int compare(const IntPair& a, const IntPair& b) const {
		int first = ::compare(a.first, b.first);
		if (first != 0)
			return first;
		else
			return ::compare(a.second, b.second);
	}
};

template <class T>
const T& getKey(const uint64_t& i);

template <>
const uint64_t& getKey(const uint64_t& i) { return i; }

std::vector<std::string> char20;
template <>
const Char<20>& getKey(const uint64_t& i) {
	std::stringstream ss;
	ss << i;
	std::string s(ss.str());
	char20.push_back(std::string(20-s.size(), '0')+s);
	return *reinterpret_cast<const Char<20>*>(char20.back().data());
}

std::vector<IntPair> intPairs;
template <>
const IntPair& getKey(const uint64_t& i) {
	intPairs.push_back(std::make_pair(i/3, 3-(i%3)));
	return intPairs.back();
}


template <class T, class CMP>
void test(uint64_t n) {
	// Set up stuff, you probably have to change something here to match to your interfaces
	BufferManager bm(100);
	// ...
	BTree<T, CMP> bTree(bm, 1);

	// Insert values
	for (uint64_t i=0; i<n; ++i)
		bTree.insert(getKey<T>(i),static_cast<TID>(i*i));
	assert(bTree.getSize()==n);

	// Check if they can be retrieved
	for (uint64_t i=0; i<n; ++i) {
		TID tid = bTree.lookup(getKey<T>(i));
		assert(tid != NULL_TID);
		assert(tid == i*i);
	}

	// Delete some values
	for (uint64_t i=0; i<n; ++i)
		if ((i%7)==0)
			bTree.erase(getKey<T>(i));

	// Check if the right ones have been deleted
	for (uint64_t i=0; i<n; ++i) {
		if ((i%7)==0) {
			assert(bTree.lookup(getKey<T>(i)) == NULL_TID);
		} else {
			TID tid = bTree.lookup(getKey<T>(i));
			assert(bTree.lookup(getKey<T>(i)) != NULL_TID);
			assert(tid==i*i);
		}
	}

	// Delete everything
	for (uint64_t i=0; i<n; ++i)
		bTree.erase(getKey<T>(i));
	assert(bTree.getSize()==0);
}

int main(int argc, char* argv[]) {
	// Get command line argument
	const uint64_t n = (argc==2) ? strtoul(argv[1], NULL, 10) : 1000*1000ul;

	// Test index with 64bit unsigned integers
	test<uint64_t, MyCustomUInt64Cmp>(n);

	// Test index with 20 character strings
	test<Char<20>, MyCustomCharCmp<20>>(n);

	// Test index with compound key
	test<IntPair, MyCustomIntPairCmp>(n);
	return 0;
}
