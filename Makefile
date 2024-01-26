all:
	mkdir -p build
	g++ -g -std=c++20 -o build/cpu src/*.cc

clean:
	rm -rf build
