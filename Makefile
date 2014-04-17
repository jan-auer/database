EXECUTABLES=database sort

.PHONY: $(EXECUTABLES)

CC=g++
FLAGS=-std=c++11 -O3
INC=-I generator -I database/utils -I database/sorting
CPP=./$@/main.cpp
OUT=-o bin/$@

all: $(EXECUTABLES)

$(EXECUTABLES):
	$(CC) $(CPP) $(FLAGS) $(INC) $(OUT)

clean:
	rm bin/*
