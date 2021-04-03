include_dir = ./include
headers = $(wildcard $(include_dir)/*.hpp) $(wildcard $(include_dir)/functional/*.hpp)
files = test

CPP_FLAGS = -std=c++2a -fconcepts-diagnostics-depth=5 -Iinclude

.PHONY: all
all: run

.PHONY: run
run: bin/test
	./bin/test

bin/test: test.cpp $(headers)
	@echo Building $(@F)
	g++ $(CPP_FLAGS) $< -o bin/$(@F)
