.DEFAULT_GOAL := build
.PHONY: build build-release test fmt clean
CMAKE ?= cmake
CMAKE_FLAGS ?=
JOBS ?= 4
build:
	$(CMAKE) -S . -B build/debug -DCMAKE_BUILD_TYPE=Debug $(CMAKE_FLAGS)
	$(CMAKE) --build build/debug --config Debug --parallel $(JOBS)
	$(CMAKE) -DROOT=. -DBUILD_DIR=build/debug -P cmake/CompileCommands.cmake
build-release:
	$(CMAKE) -S . -B build/release -DCMAKE_BUILD_TYPE=Release $(CMAKE_FLAGS)
	$(CMAKE) --build build/release --config Release --parallel $(JOBS)
test: build
	ctest --test-dir build/debug -C Debug --output-on-failure --timeout 30
fmt:
	$(CMAKE) -DROOT=. -P cmake/Format.cmake
clean:
	$(CMAKE) -DROOT=. -P cmake/Clean.cmake
