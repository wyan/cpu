CPP_PARAMS=-g -std=c++20

TEST_DEPS=src/cpu.cc src/cpu.h test/*.cc test/vendor/*.c test/vendor/*.h
BASIC_DEPS=src/cpu.cc src/cpu.h
TOOL_DEPS=src/tools.cc src/tools.h
CPU_DEPS=$(BASIC_DEPS) $(TOOL_DEPS) src/main.cc

all: cpu cpu2bin test

cpu: build/cpu

tools: cpu2bin

cpu2bin: build/cpu2bin

test: build/test

build/cpu: $(CPU_DEPS)
	mkdir -p build
	g++ $(CPP_PARAMS) -o build/cpu src/main.cc src/cpu.cc src/tools.cc

build/cpu2bin: $(TOOL_DEPS) src/cpu2bin.cc
	mkdir -p build
	g++ $(CPP_PARAMS) -o build/cpu2bin src/tools.cc src/cpu2bin.cc

build/test: $(TEST_DEPS)
	mkdir -p build
	g++ $(CPP_PARAMS) -o build/test src/cpu.cc test/*.cc test/vendor/*.c

clean:
	rm -rf build

.PHONY:	all clean
