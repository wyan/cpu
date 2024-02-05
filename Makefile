CPP_PARAMS=-g -std=c++20

all: cpu test

test:
	mkdir -p build
	g++ $(CPP_PARAMS) -o build/test src/cpu.cc test/*.cc test/vendor/*.c

cpu:
	mkdir -p build
	g++ $(CPP_PARAMS) -o build/cpu src/*.cc

clean:
	rm -rf build

.PHONY:	all clean cpu test
