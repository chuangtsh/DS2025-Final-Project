CXX := g++

# ============================================================
# Baseline flags shared by all targets
# ============================================================
STD      := -std=c++17
WARN     := -Wall -Wextra -Wpedantic
INCLUDES := -Idatastructure -Ibaseline

# ------------------------------------------------------------
# Optimization strategy:
# - Keep -O3 (algorithmic behavior preserved)
# - REMOVE -march=native (machine-specific bias)
# - REMOVE -flto (cross-TU inlining bias)
# - KEEP -DNDEBUG (no asserts)
# ------------------------------------------------------------
# Portable, fair benchmark configuration
OPT := -O3 -DNDEBUG -march=x86-64 -mtune=generic -fno-tree-vectorize

# ------------------------------------------------------------
# Determinism / stability helpers
# ------------------------------------------------------------
DET := -fno-omit-frame-pointer

CXXFLAGS := $(STD) $(OPT) $(WARN) $(INCLUDES) $(DET)

# Linker flags intentionally minimal
LDFLAGS :=

.PHONY: all clean

all: datastructure/datastructure.exe benchmark/benchmark.exe baseline/baseline.exe

# ============================================================
# Targets
# ============================================================

datastructure/datastructure.exe: datastructure/datastructure.cpp datastructure/PairingHeap.h
	$(CXX) $(CXXFLAGS) $< -o $@

benchmark/benchmark.exe: benchmark/benchmark.cpp \
	datastructure/PairingHeap.h \
	baseline/baseline.h \
	baseline/FourAryHeap.h \
	baseline/FibonacciHeap.h
	$(CXX) $(CXXFLAGS) $< -o $@

baseline/baseline.exe: baseline/baseline.cpp \
	baseline/baseline.h \
	baseline/FourAryHeap.h \
	baseline/FibonacciHeap.h
	$(CXX) $(CXXFLAGS) $< -o $@

clean:
	rm -f datastructure/*.exe benchmark/*.exe baseline/*.exe graph_dots/*.dot
