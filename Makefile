.PHONY: all build test run-test clean

all: build

build:
	cmake -S . -B build
	cmake --build build

test:
	cmake --build build --target test

run-test: test
	./build/test

clean:
	cmake --build build --target clean