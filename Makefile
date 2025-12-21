CXX := g++
CXXFLAGS := -std=c++17 -O2 -Wall -Iinclude

.PHONY: all clean

all: datastructure/datastructure.exe benchmark/benchmark.exe baseline/baseline.exe

datastructure/datastructure.exe: datastructure/datastructure.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

benchmark/benchmark.exe: benchmark/benchmark.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

baseline/baseline.exe: baseline/baseline.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

clean:
	rm -f datastructure/*.exe benchmark/*.exe baseline/*.exe
