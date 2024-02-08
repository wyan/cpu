CPP_PARAMS=-g -std=c++20

all: cpu cpu2bin test

test:
	mkdir -p build
	g++ $(CPP_PARAMS) -o build/test src/cpu.cc test/*.cc test/vendor/*.c

cpu:
	mkdir -p build
	g++ $(CPP_PARAMS) -o build/cpu src/main.cc src/cpu.cc src/tools.cc

cpu2bin:
	mkdir -p build
	g++ $(CPP_PARAMS) -o build/cpu2bin src/tools.cc src/cpu2bin.cc

clean:
	rm -rf build

.PHONY:	all clean cpu cpu2bin test
