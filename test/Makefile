CC ?= cc
CXX ?= c++
STD_CFLAGS = -std=c99
STD_CXXFLAGS = -std=gnu++11
WARNINGS = -Wall -Wextra -Wno-missing-field-initializers
OPT = -O0

.PHONY: test_c test_cpp
all: test_c test_cpp

test_c: test.c
	$(CC) $(STD_CFLAGS) $(WARNINGS) $(OPT) -o $@ $<

test_cpp: test.cpp
	$(CXX) $(STD_CXXFLAGS) $(WARNINGS) $(OPT) -o $@ $<

run: test_c test_cpp
	./test_c
	@echo
	./test_cpp

clean:
	rm -f test_c test_cpp

.PHONY: run clean
