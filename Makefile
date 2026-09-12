.PHONY: all build test run-test clean

all: build

build:
	cmake -S . -B build -DCOBALT_BUILD_TESTS=OFF
	cmake --build build

test:
	cmake -S . -B build -DCOBALT_BUILD_TESTS=ON
	cmake --build build --target cobalt_test

run-test: test
	clear && ./build/cobalt_test

clean:
	cmake --build build --target clean