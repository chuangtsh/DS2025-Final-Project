CXX := g++
CXXFLAGS := -std=c++17 -O2 -Wall -Idatastructure -Ibaseline

.PHONY: all clean

all: datastructure/datastructure.exe benchmark/benchmark.exe baseline/baseline.exe

datastructure/datastructure.exe: datastructure/datastructure.cpp datastructure/PairingHeap.h
	$(CXX) $(CXXFLAGS) datastructure/datastructure.cpp -o $@

benchmark/benchmark.exe: benchmark/benchmark.cpp datastructure/PairingHeap.h baseline/baseline.h baseline/FourAryHeap.h baseline/FibonacciHeap.h
	$(CXX) $(CXXFLAGS) benchmark/benchmark.cpp -o $@

baseline/baseline.exe: baseline/baseline.cpp baseline/baseline.h baseline/FourAryHeap.h baseline/FibonacciHeap.h
	$(CXX) $(CXXFLAGS) $< -o $@

clean:
	rm -f datastructure/*.exe benchmark/*.exe baseline/*.exe graph_dots/*.dot