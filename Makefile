.PHONY: sort

sort:
	g++ ./sort/main.cpp -std=c++11 -Igenerator -Idatabase/utils -Idatabase/sorting -o products/sort
clean:
	rm -rf products
