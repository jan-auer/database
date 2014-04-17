EXECUTABLES=database sort

.PHONY: $(EXECUTABLES)

CC=g++
STD=-std=c++11
INC=-I generator -I database/utils -I database/sorting
CPP=./$@/main.cpp
OUT=-o bin/$@

all: $(EXECUTABLES)

$(EXECUTABLES):
	$(CC) $(CPP) $(STD) $(INC) $(OUT)

clean:
	rm bin/*
