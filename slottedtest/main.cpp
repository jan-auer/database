#include <iostream>
#include <vector>
#include <cassert>
#include <cstring>
#include <unordered_map>

#include "common/IDs.h"
#include "buffer/BufferManager.h"
#include "segment/SPSegment.h"

using namespace std;
using namespace lsql;

// TODO: adapt to your implementation.
uint64_t extractPage(TID tid) {
	return tid.page();
}

// Sizes are in (slotted) pages.
const unsigned initialSize = 100;
const unsigned totalSize = initialSize;// + 50;

const unsigned maxInserts = 1000000ul;
const unsigned maxDeletes = 10000ul;
const unsigned maxUpdates = 10000ul;

// Percentage of a page that can be used to store the payload.
const double loadFactor = 0.8;

const vector<string> testData = {
	"640K ought to be enough for anybody",
	"Beware of bugs in the above code; I have only proved it correct, not tried"
	" it",
	"Tape is Dead. Disk is Tape. Flash is Disk.",
	"for seminal contributions to database and transaction processing research "
	"and technical leadership in system implementation",
	"Lorem ipsum dolor sit amet, consectetur adipiscing elit. Fusce iaculis "
	"risus ut ipsum pellentesque vitae venenatis elit viverra. Class aptent"
	" taciti sociosqu ad litora torquent per conubia nostra, per inceptos "
	"himenaeos. Curabitur ante mi, auctor in aliquet non, sagittis ac est. "
	"Phasellus in viverra mauris. Quisque scelerisque nisl eget sapien "
	"venenatis nec consectetur odio aliquam. Maecenas lobortis mattis "
	"semper. Ut lacinia urna nec lorem lacinia consectetur. In non enim "
	"vitae dui rhoncus dictum. Sed vel fringilla felis. Curabitur tincidunt"
	" justo ac nulla scelerisque accumsan. Cum sociis natoque penatibus et "
	"magnis dis parturient montes, nascetur ridiculus mus. Cras tempor "
	"venenatis orci, quis vulputate risus dapibus id. Aliquam elementum "
	"congue nulla, eget tempus justo fringilla sed. Maecenas odio erat, "
	"commodo a blandit quis, tincidunt vel risus. Proin sed ornare tellus. "
	"Donec tincidunt urna ac turpis rutrum varius. Etiam vehicula semper "
	"velit ut mollis. Aliquam quis sem massa. Morbi ut massa quis purus "
	"ullamcorper aliquet. Sed nisi justo, fermentum id placerat eu, "
	"dignissim eu elit. Cum sociis natoque penatibus et magnis dis "
	"parturient montes, nascetur ridiculus mus. Suspendisse interdum "
	"laoreet commodo. Nullam turpis velit, tristique in sodales sit amet, "
	"molestie et diam. Quisque blandit velit quis augue sodales vestibulum."
	" Phasellus ut magna non arcu egestas volutpat. Etiam id ultricies "
	"ligula. Donec non lectus eget risus lobortis pretium. Sed rutrum augue"
	" eu tellus scelerisque sit amet interdum massa volutpat. Maecenas nunc"
	" ligula, blandit quis adipiscing eget, fermentum nec massa. Vivamus in"
	" commodo nunc. Quisque elit mi, consequat eget vestibulum lacinia, "
	"ultrices eu purus. Vestibulum tincidunt consequat nulla, quis tempus "
	"eros volutpat sed. Aliquam elementum massa vel ligula bibendum aliquet"
	" non nec purus. Nunc sollicitudin orci sed nisi eleifend molestie. "
	"Praesent scelerisque vehicula quam et dignissim. Suspendisse potenti. "
	"Sed lacus est, aliquet auctor mollis ac, iaculis at metus. Aenean at "
	"risus sed lectus volutpat bibendum non id odio. Pellentesque habitant "
	"morbi tristique senectus et netus et malesuada fames ac turpis "
	"egestas. Mauris purus lorem, congue ac tristique sit amet, gravida eu "
	"neque. Nullam lacus tellus, venenatis a blandit ac, consequat sed "
	"massa. Mauris ultrices laoreet lorem. Nam elementum, est vel elementum"
	" commodo, enim tellus mattis diam, a bibendum mi enim vitae magna. "
	"Aliquam nisi dolor, aliquam at porta sit amet, tristique id nulla. In "
	"purus leo, tristique eget faucibus id, pharetra vel diam. Nunc "
	"eleifend commodo feugiat. Mauris sed diam quis est dictum rutrum in eu"
	" erat. Suspendisse potenti. Duis adipiscing nisl eu augue dignissim "
	"sagittis. Praesent vitae nisl dolor. Duis interdum, dolor a viverra "
	"imperdiet, lorem lectus luctus sem, sit amet rutrum augue dolor id "
	"erat. Vestibulum ac orci condimentum velit mollis scelerisque eu eu "
	"est. Aenean fringilla placerat enim, placerat adipiscing felis feugiat"
	" quis. Cras sed."
};

class Random64 {
	uint64_t state;
public:
	explicit Random64(uint64_t seed=88172645463325252ull) : state(seed) {}
	uint64_t next() {
		state ^= (state << 13);
		state ^= (state >> 7);
		return (state ^= (state << 17));
	}
};

int main(int argc, char** argv) {
	// Check arguments.
//	if (argc != 2) {
//		cerr << "usage: " << argv[0] << " <pageSize>" << endl;
//		return -1;
//	}
	const unsigned pageSize = BufferFrame::SIZE;

	// Bookkeeping.
	// TID -> testData entry.
	unordered_map<uint64_t, unsigned> values;
	// pageID -> bytes used within this page.
	unordered_map<unsigned, unsigned> usage;

	// Set up everything.
	BufferManager bm(100);

	//ToDo: Create Schema Manager
	//SchemaManager lookup
	SPSegment sp(bm, 1, 0);
	Random64 rnd;

	// Insert some records.
	for (unsigned i = 0; i < maxInserts; ++i) {
		// Select string/record to insert.
		uint64_t r = rnd.next() % testData.size();
		const string s = testData[r];

		// Check that there is space available for 's'.
		bool full = true;
		for (unsigned p = 0; p < initialSize; ++p) {
			if (usage[p] + s.size() < loadFactor * pageSize) {
				full = false;
				break;
			}
		}
		if (full) {
			break;
		}

		// Insert record.
		TID tid = sp.insert(Record(s.size(), s.c_str()));

		// TIDs should not be overwritten.
		assert(values.find(tid.id) == values.end());
		values[tid.id] = r;

		// Extract the pageId from the TID.
		unsigned pageId = extractPage(tid);

		// pageId should be within [0, initialSize).
		assert(pageId < initialSize);
		usage[pageId] += s.size();
	}

	unsigned actualInserts = values.size();

	// Look up & delete some records.
	for (unsigned i = 0; i < actualInserts && i < maxDeletes; ++i) {
		// Select operation.
		bool del = rnd.next() % 10 == 0;

		// Select victim.
		TID tid = values.begin()->first;
		unsigned pageId = extractPage(tid);
		const string& value = testData[(values.begin()->second)
																	 % testData.size()];
		unsigned len = value.size();

		// Lookup.
		Record rec = sp.lookup(tid);
		assert(rec.getSize() == len);
		assert(memcmp(rec.getData(), value.c_str(), len) == 0);

		// Do delete.
		if (del) {
			assert(sp.remove(tid));
			values.erase(tid.id);
			usage[pageId] -= len;
		}
	}

	// Update some values ('usage' counter invalid from here on).
	for (unsigned i = 0; i < maxUpdates; ++i) {
		// Select victim.
		TID tid = values.begin()->first;

		// Select new string/record.
		uint64_t r = rnd.next() % testData.size();
		const string s = testData[r];

		// Replace old with new value.
		sp.update(tid, Record(s.size(), s.c_str()));
		values[tid.id] = r;
	}

	// Lookups.
	for (auto p : values) {
		TID tid = p.first;
		const string& value = testData[p.second];
		unsigned len = value.size();
		Record rec = sp.lookup(tid);
		assert(rec.getSize() == len);
		assert(memcmp(rec.getData(), value.c_str(), len) == 0);
	}

	std::cout << "Success!" << std::endl;
	return 0;
}
