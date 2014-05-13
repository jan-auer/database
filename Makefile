EXECUTABLES=database sort buffertest unit_test

.PHONY: $(EXECUTABLES)

CPP_FILES = database/schema/Schema.cpp database/relation/Record.cpp database/relation/SPSegment.cpp database/relation/SlottedPage.cpp database/common/IDs.cpp database/schema/SchemaManager.cpp database/buffer/BufferManager.cpp database/buffer/BufferFrame.cpp database/utils/Lock.cpp database/utils/Mutex.cpp unit_test/gtest/gtest-all.cc

CC=g++
FLAGS=-std=c++11 -O3
INC=-I generator -I database
CPP=./$@/main.cpp $(CPP_FILES)
OUT=-o bin/$@

all: $(EXECUTABLES)

$(EXECUTABLES):
	$(CC) $(CPP) $(FLAGS) $(INC) $(OUT)

clean:
	rm bin/*
