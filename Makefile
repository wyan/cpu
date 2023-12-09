all:
	mkdir -p build
	g++ -std=c++20 -o build/cpu src/*.cc
