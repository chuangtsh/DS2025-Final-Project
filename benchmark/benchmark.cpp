#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include <limits>
#include <iomanip>
#include <queue>
#include <string>

#include "../datastructure/PairingHeap.h"
#include "../baseline/FourAryHeap.h"
#include "../baseline/FibonacciHeap.h"
#include "../baseline/baseline.h"

// ==========================================
// Utilities
// ==========================================

struct OpStats {
    long long inserts = 0;
    long long decreaseKeys = 0;
    long long extractMins = 0;
    
    void reset() { inserts = 0; decreaseKeys = 0; extractMins = 0; }
};

class Timer {
    std::chrono::high_resolution_clock::time_point start;
public:
    Timer() : start(std::chrono::high_resolution_clock::now()) {}
    double elapsed() const {
        auto end = std::chrono::high_resolution_clock::now();
        return std::chrono::duration<double, std::milli>(end - start).count();
    }
};

template<typename Func>
double runBenchmark(Func f, int iterations = 5, int warmup = 2) {
    for(int i=0; i<warmup; ++i) f();
    
    double totalTime = 0.0;
    for(int i=0; i<iterations; ++i) {
        Timer timer;
        f();
        totalTime += timer.elapsed();
    }
    return totalTime / iterations;
}

std::string fmt(double t) {
    char buf[32];
    snprintf(buf, sizeof(buf), "%.3f", t);
    return std::string(buf);
}

// ==========================================
// Graph Structures & Algorithms
// ==========================================

struct Edge { int to; int weight; };
using Graph = std::vector<std::vector<Edge>>;

Graph generateRandomGraph(int vertices, int avgDegree, int maxWeight) {
    Graph graph(vertices);
    std::random_device rd;
    std::mt19937 gen(42);
    std::uniform_int_distribution<> vertexDist(0, vertices - 1);
    std::uniform_int_distribution<> weightDist(1, maxWeight);
    
    for (int u = 0; u < vertices; u++) {
        int edges = avgDegree + (gen() % 5) - 2;
        if (edges < 0) edges = 0;
        for (int i = 0; i < edges; i++) {
            int v = vertexDist(gen);
            if (v != u) graph[u].push_back({v, weightDist(gen)});
        }
    }
    return graph;
}

// --- Lazy Variants (STL, 4-ary, Fib-lazy, Pair-lazy) ---

std::vector<int> dijkstraSTL(const Graph& graph, int source, OpStats* stats = nullptr) {
    int n = graph.size();
    std::vector<int> dist(n, std::numeric_limits<int>::max());
    std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>, std::greater<>> pq;
    
    dist[source] = 0;
    pq.push({0, source});
    if(stats) stats->inserts++;
    
    while (!pq.empty()) {
        auto [d, u] = pq.top();
        pq.pop();
        if(stats) stats->extractMins++;
        
        if (d > dist[u]) continue;
        for (const Edge& edge : graph[u]) {
            int v = edge.to;
            int newDist = dist[u] + edge.weight;
            if (newDist < dist[v]) {
                dist[v] = newDist;
                pq.push({newDist, v});
                if(stats) stats->inserts++; // Lazy insert
            }
        }
    }
    return dist;
}

std::vector<int> dijkstraFourAryHeap(const Graph& graph, int source, OpStats* stats = nullptr) {
    int n = graph.size();
    std::vector<int> dist(n, std::numeric_limits<int>::max());
    FourAryHeap<std::pair<int, int>> pq;
    
    dist[source] = 0;
    pq.insert({0, source});
    if(stats) stats->inserts++;
    
    while (!pq.isEmpty()) {
        auto [d, u] = pq.extractMin();
        if(stats) stats->extractMins++;
        
        if (d > dist[u]) continue;
        for (const Edge& edge : graph[u]) {
            int v = edge.to;
            int newDist = dist[u] + edge.weight;
            if (newDist < dist[v]) {
                dist[v] = newDist;
                pq.insert({newDist, v});
                if(stats) stats->inserts++;
            }
        }
    }
    return dist;
}

std::vector<int> dijkstraFibonacciHeapLazy(const Graph& graph, int source, OpStats* stats = nullptr) {
    int n = graph.size();
    std::vector<int> dist(n, std::numeric_limits<int>::max());
    FibonacciHeap<std::pair<int, int>> pq;
    
    dist[source] = 0;
    pq.insert({0, source});
    if(stats) stats->inserts++;
    
    while (!pq.isEmpty()) {
        auto [d, u] = pq.extractMin();
        if(stats) stats->extractMins++;
        
        if (d > dist[u]) continue;
        for (const Edge& edge : graph[u]) {
            int v = edge.to;
            int newDist = dist[u] + edge.weight;
            if (newDist < dist[v]) {
                dist[v] = newDist;
                pq.insert({newDist, v});
                if(stats) stats->inserts++;
            }
        }
    }
    return dist;
}

std::vector<int> dijkstraPairingHeapLazy(const Graph& graph, int source, OpStats* stats = nullptr) {
    int n = graph.size();
    std::vector<int> dist(n, std::numeric_limits<int>::max());
    PairingHeap<std::pair<int, int>> pq;
    
    dist[source] = 0;
    pq.insert({0, source});
    if(stats) stats->inserts++;
    
    while (!pq.isEmpty()) {
        auto [d, u] = pq.extractMin();
        if(stats) stats->extractMins++;
        
        if (d > dist[u]) continue;
        for (const Edge& edge : graph[u]) {
            int v = edge.to;
            int newDist = dist[u] + edge.weight;
            if (newDist < dist[v]) {
                dist[v] = newDist;
                pq.insert({newDist, v});
                if(stats) stats->inserts++;
            }
        }
    }
    return dist;
}

// --- Decrease-Key Variants (Fib-dec, Pair-dec) ---

std::vector<int> dijkstraFibonacciHeap(const Graph& graph, int source, OpStats* stats = nullptr) {
    int n = graph.size();
    std::vector<int> dist(n, std::numeric_limits<int>::max());
    std::vector<FibNode<std::pair<int, int>>*> nodeHandles(n, nullptr);
    FibonacciHeap<std::pair<int, int>> pq;
    
    dist[source] = 0;
    nodeHandles[source] = pq.insert({0, source});
    if(stats) stats->inserts++;
    
    while (!pq.isEmpty()) {
        auto [d, u] = pq.extractMin();
        if(stats) stats->extractMins++;
        
        if (d > dist[u]) continue;
        for (const Edge& edge : graph[u]) {
            int v = edge.to;
            int newDist = dist[u] + edge.weight;
            if (newDist < dist[v]) {
                if (dist[v] == std::numeric_limits<int>::max()) {
                    dist[v] = newDist;
                    nodeHandles[v] = pq.insert({newDist, v});
                    if(stats) stats->inserts++;
                } else {
                    dist[v] = newDist;
                    pq.decreaseKey(nodeHandles[v], {newDist, v});
                    if(stats) stats->decreaseKeys++;
                }
            }
        }
    }
    return dist;
}

std::vector<int> dijkstraPairingHeap(const Graph& graph, int source, OpStats* stats = nullptr) {
    int n = graph.size();
    std::vector<int> dist(n, std::numeric_limits<int>::max());
    std::vector<PairingNode<std::pair<int, int>>*> nodeHandles(n, nullptr);
    PairingHeap<std::pair<int, int>> pq;
    
    dist[source] = 0;
    nodeHandles[source] = pq.insert({0, source});
    if(stats) stats->inserts++;
    
    while (!pq.isEmpty()) {
        auto [d, u] = pq.extractMin();
        if(stats) stats->extractMins++;
        
        if (d > dist[u]) continue;
        for (const Edge& edge : graph[u]) {
            int v = edge.to;
            int newDist = dist[u] + edge.weight;
            if (newDist < dist[v]) {
                if (dist[v] == std::numeric_limits<int>::max()) {
                    dist[v] = newDist;
                    nodeHandles[v] = pq.insert({newDist, v});
                    if(stats) stats->inserts++;
                } else {
                    dist[v] = newDist;
                    pq.decreaseKey(nodeHandles[v], {newDist, v});
                    if(stats) stats->decreaseKeys++;
                }
            }
        }
    }
    return dist;
}

// ==========================================
// Benchmarks
// ==========================================

void benchmarkInsertHeavy() {
    std::cout << "\n=== Benchmark 1: Insert-Heavy Workload ===" << std::endl;
    printf("%-12s | %-12s | %-12s | %-12s | %-12s\n", 
           "Data Size", "STL (ms)", "4-ary (ms)", "Fib (ms)", "Pairing (ms)");
    printf("----------------------------------------------------------------------------\n");
    
    std::vector<int> sizes = {1000, 10000, 100000, 500000, 1000000};
    
    for (int N : sizes) {
        std::vector<int> data(N);
        std::mt19937 gen(42);
        std::uniform_int_distribution<> dis(1, 1000000);
        for(int& x : data) x = dis(gen);
        
        double t_stl = runBenchmark([&]() {
            std::priority_queue<int, std::vector<int>, std::greater<>> pq;
            for(int x : data) pq.push(x);
        });
        
        double t_4ary = runBenchmark([&]() {
            FourAryHeap<int> pq;
            for(int x : data) pq.insert(x);
        });
        
        double t_fib = runBenchmark([&]() {
            FibonacciHeap<int> pq;
            for(int x : data) pq.insert(x);
        });
        
        double t_pair = runBenchmark([&]() {
            PairingHeap<int> pq;
            for(int x : data) pq.insert(x);
        });
        
        printf("%-12d | %-12s | %-12s | %-12s | %-12s\n",
           N, fmt(t_stl).c_str(), fmt(t_4ary).c_str(), fmt(t_fib).c_str(), fmt(t_pair).c_str());
    }
}

void benchmarkDecreaseKeyHeavy() {
    std::cout << "\n=== Benchmark 2: Decrease-Key-Heavy Workload ===" << std::endl;
    std::cout << "Highlighting Theoretical Capabilities (Allocators may impact Perf)" << std::endl;
    std::cout << "(Data Size = Initial Elements, Operations = Data Size)" << std::endl;
    printf("%-12s | %-12s | %-12s | %-12s | %-12s\n", 
           "Data Size", "STL (Lazy)", "4-ary (Lazy)", "Fib (Dec)", "Pair (Dec)");
    printf("----------------------------------------------------------------------------\n");
    
    // Extended scales
    std::vector<int> sizes = {1000, 10000, 50000, 100000, 200000, 500000};
    
    for (int N : sizes) {
        int ops = N; 
        std::vector<int> initialData(N);
        std::vector<std::pair<int, int>> opData(ops);
        
        std::mt19937 gen(42);
        std::uniform_int_distribution<> valDis(1000000, 2000000);
        for(int& x : initialData) x = valDis(gen);
        
        std::uniform_int_distribution<> idxDis(0, N - 1);
        std::uniform_int_distribution<> decDis(1, 10000);
        for(auto& p : opData) p = {idxDis(gen), decDis(gen)};
        
        double t_stl = runBenchmark([&]() {
            std::priority_queue<int, std::vector<int>, std::greater<>> pq;
            std::vector<int> current = initialData;
            for(int x : current) pq.push(x);
            for(auto p : opData) {
                int idx = p.first;
                int nv = std::max(1, current[idx] - p.second);
                current[idx] = nv;
                pq.push(nv);
            }
        });
        
        double t_4ary = runBenchmark([&]() {
            FourAryHeap<int> pq;
            std::vector<int> current = initialData;
            for(int x : current) pq.insert(x);
            for(auto p : opData) {
                int idx = p.first;
                int nv = std::max(1, current[idx] - p.second);
                current[idx] = nv;
                pq.insert(nv);
            }
        });
        
        double t_fib = runBenchmark([&]() {
             FibonacciHeap<int> pq;
             std::vector<FibNode<int>*> nodes;
             nodes.reserve(N);
             for(int x : initialData) nodes.push_back(pq.insert(x));
             for(auto p : opData) {
                 int idx = p.first;
                 int nv = std::max(1, nodes[idx]->value - p.second);
                 pq.decreaseKey(nodes[idx], nv);
             }
        });
        
        double t_pair = runBenchmark([&]() {
             PairingHeap<int> pq;
             std::vector<PairingNode<int>*> nodes;
             nodes.reserve(N);
             for(int x : initialData) nodes.push_back(pq.insert(x));
             for(auto p : opData) {
                 int idx = p.first;
                 int nv = std::max(1, nodes[idx]->value - p.second);
                 pq.decreaseKey(nodes[idx], nv);
             }
        });
        
        printf("%-12d | %-12s | %-12s | %-12s | %-12s\n",
           N, fmt(t_stl).c_str(), fmt(t_4ary).c_str(), fmt(t_fib).c_str(), fmt(t_pair).c_str());
    }
}

void benchmarkMergeSequence() {
    std::cout << "\n=== Benchmark 3: Sequence of Merges ===" << std::endl;
    std::cout << "(Data Size = Total Elements across all heaps)" << std::endl;
    printf("%-12s | %-12s | %-12s | %-12s | %-12s\n", 
           "Data Size", "STL (ms)", "4-ary (ms)", "Fib (ms)", "Pairing (ms)");
    printf("----------------------------------------------------------------------------\n");
    
    std::vector<int> totalElements = {10000, 100000, 500000, 1000000};
    const int HEAP_SIZE = 100;
    
    for (int TotalN : totalElements) {
        int numHeaps = TotalN / HEAP_SIZE;
        std::vector<std::vector<int>> heapData(numHeaps);
        std::mt19937 gen(42);
        std::uniform_int_distribution<> dis(1, 1000000);
        
        for(int i=0; i<numHeaps; ++i) {
            for(int j=0; j<HEAP_SIZE; ++j) heapData[i].push_back(dis(gen));
        }
        
        double t_stl = runBenchmark([&]() {
            std::vector<std::priority_queue<int, std::vector<int>, std::greater<>>> heaps(numHeaps);
            for(int i=0; i<numHeaps; ++i) 
                for(int x : heapData[i]) heaps[i].push(x);
            for(int i=1; i<numHeaps; ++i) {
                while(!heaps[i].empty()) {
                    heaps[0].push(heaps[i].top());
                    heaps[i].pop();
                }
            }
        });
        
        double t_4ary = runBenchmark([&]() {
            std::vector<FourAryHeap<int>> heaps(numHeaps);
            for(int i=0; i<numHeaps; ++i) 
                for(int x : heapData[i]) heaps[i].insert(x);
            for(int i=1; i<numHeaps; ++i) {
                while(!heaps[i].isEmpty()) {
                    heaps[0].insert(heaps[i].extractMin());
                }
            }
        });
        
        double t_fib = runBenchmark([&]() {
            std::vector<FibonacciHeap<int>> heaps(numHeaps);
            for(int i=0; i<numHeaps; ++i) 
                for(int x : heapData[i]) heaps[i].insert(x);
            for(int i=1; i<numHeaps; ++i) heaps[0].merge(heaps[i]);
        });
        
        double t_pair = runBenchmark([&]() {
            std::vector<PairingHeap<int>> heaps(numHeaps);
            for(int i=0; i<numHeaps; ++i) 
                for(int x : heapData[i]) heaps[i].insert(x);
            for(int i=1; i<numHeaps; ++i) heaps[0].merge(heaps[i]);
        });
        
       printf("%-12d | %-12s | %-12s | %-12s | %-12s\n",
           TotalN, fmt(t_stl).c_str(), fmt(t_4ary).c_str(), fmt(t_fib).c_str(), fmt(t_pair).c_str());
    }
}

void benchmarkDijkstraLazy() {
    std::cout << "\n=== Benchmark 4a: Dijkstra (Lazy Variants) ===" << std::endl;
    std::cout << "(Note: Allocator overhead is significant for Fib/Pair)" << std::endl;
    
    printf("%-9s | %-9s | %-10s | %-9s | %-12s | %-9s\n", 
           "Vertices", "STL", "4-ary", "Fib(Lazy)", "Pair(Lazy)", "Ops(Ins/Ext)");
    printf("-------------------------------------------------------------------------------\n");
    
    std::vector<int> vertCounts = {1000, 5000, 10000, 20000, 50000};
    
    for (int V : vertCounts) {
        Graph g = generateRandomGraph(V, 20, 100);
        
        // Collect stats once
        OpStats stats;
        dijkstraSTL(g, 0, &stats);
        
        double t_stl = runBenchmark([&](){ dijkstraSTL(g, 0); });
        double t_4ary = runBenchmark([&](){ dijkstraFourAryHeap(g, 0); });
        double t_fib = runBenchmark([&](){ dijkstraFibonacciHeapLazy(g, 0); });
        double t_pair = runBenchmark([&](){ dijkstraPairingHeapLazy(g, 0); });

        std::string opsStr = std::to_string(stats.inserts) + "/" + std::to_string(stats.extractMins);

        printf("%-9d | %-9s | %-10s | %-9s | %-12s | %-9s\n",
               V, 
               fmt(t_stl).c_str(), fmt(t_4ary).c_str(), 
               fmt(t_fib).c_str(), fmt(t_pair).c_str(),
               opsStr.c_str());
    }
}

void benchmarkDijkstraDecKey() {
    std::cout << "\n=== Benchmark 4b: Dijkstra (Decrease-Key Variants) ===" << std::endl;
    
    printf("%-9s | %-9s | %-9s | %-16s\n", 
           "Vertices", "Fib(Dec)", "Pair(Dec)", "Ops(Ins/Dec/Ext)");
    printf("---------------------------------------------------------\n");
    
    std::vector<int> vertCounts = {1000, 5000, 10000, 20000, 50000};
    
    for (int V : vertCounts) {
        Graph g = generateRandomGraph(V, 20, 100);
        
        OpStats stats;
        dijkstraFibonacciHeap(g, 0, &stats);
        
        double t_fib_dec = runBenchmark([&](){ dijkstraFibonacciHeap(g, 0); });
        double t_pair_dec = runBenchmark([&](){ dijkstraPairingHeap(g, 0); });
        
        std::string opsStr = std::to_string(stats.inserts) + "/" + 
                             std::to_string(stats.decreaseKeys) + "/" + 
                             std::to_string(stats.extractMins);
        
        printf("%-9d | %-9s | %-9s | %-16s\n",
               V, 
               fmt(t_fib_dec).c_str(), fmt(t_pair_dec).c_str(),
               opsStr.c_str());
    }
}

int main() {
    std::cout << "=== Priority Queue Benchmark Suite ===" << std::endl;
    std::cout << "Runs per benchmark: 5 (+2 warmup)" << std::endl;
    
    benchmarkInsertHeavy();
    benchmarkDecreaseKeyHeavy();
    benchmarkMergeSequence();
    
    // Split Dijkstra benchmarks
    benchmarkDijkstraLazy();
    benchmarkDijkstraDecKey();
    
    return 0;
}
