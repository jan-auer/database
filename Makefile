EXECUTABLES=database sort buffertest slottedtest btreetest unit_test

.PHONY: $(EXECUTABLES)

CPP_FILES = database/common/IDs.cpp    \
		database/utils/Lock.cpp            \
		database/utils/Mutex.cpp           \
		database/buffer/BufferManager.cpp  \
		database/buffer/BufferFrame.cpp    \
		database/segment/Record.cpp        \
		database/segment/Segment.cpp       \
		database/segment/SlottedPage.cpp   \
		database/segment/SPSegment.cpp     \
		database/schema/SchemaManager.cpp  \
		unit_test/gtest/gtest-all.cc

CC=clang++
FLAGS=-std=c++11 -O3
INC=-I generator -I database
CPP=./$@/main.cpp $(CPP_FILES)
OUT=-o bin/$@

all: $(EXECUTABLES)

$(EXECUTABLES):
	$(CC) $(CPP) $(FLAGS) $(INC) $(OUT)

clean:
	rm bin/*
