#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include <limits>
#include <iomanip>
#include <queue>
#include <string>

#include "../datastructure/PairingHeap.h"
#include "../datastructure/PooledPairingHeap.h"
#include "../baseline/BinaryHeap.h"          // <--- 新增這行
#include "../baseline/FourAryHeap.h"
#include "../baseline/FibonacciHeap.h"
#include "../baseline/PooledFibonacciHeap.h"
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
double runBenchmark(Func f, int iterations = 20, int warmup = 5) {
    for(int i=0; i<warmup; ++i) f();
    
    double totalTime = 0.0;
    for(int i=0; i<iterations; ++i) {
        Timer timer;
        f();
        totalTime += timer.elapsed();
    }
    return totalTime / iterations;
}

template<typename Func>
double runIsolated(Func f, int iterations = 20, int warmup = 5) {
    for(int i=0; i<warmup; ++i) f();
    
    double totalTime = 0.0;
    for(int i=0; i<iterations; ++i) {
        totalTime += f();
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

// --- Lazy Variants (STL, Binary, 4-ary, Fib-lazy, Pair-lazy) ---

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
                if(stats) stats->inserts++; 
            }
        }
    }
    return dist;
}

// 新增: Binary Heap Dijkstra
std::vector<int> dijkstraBinaryHeap(const Graph& graph, int source, OpStats* stats = nullptr) {
    int n = graph.size();
    std::vector<int> dist(n, std::numeric_limits<int>::max());
    BinaryHeap<std::pair<int, int>> pq; // 使用自製 BinaryHeap
    
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
// Note: Binary Heap and 4-ary Heap do not support efficient decrease-key
// so they are not included in this specific section.

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
    std::cout << "\n=== Benchmark 2: Insert-Heavy Workload (Pure Insert) ===" << std::endl;
    // Updated header to include Binary
    printf("%-10s | %-9s | %-9s | %-9s | %-9s | %-9s | %-9s | %-9s\n", 
           "Size", "STL", "Binary", "4-ary", "Fib", "Pair", "Fib(P)", "Pair(P)");
    printf("--------------------------------------------------------------------------------------\n");
    
    std::vector<int> sizes = {1000, 10000, 100000, 500000, 1000000};
    
    for (int N : sizes) {
        std::vector<int> data(N);
        std::mt19937 gen(42);
        std::uniform_int_distribution<> dis(1, 1000000);
        for(int& x : data) x = dis(gen);
        
        double t_stl = runIsolated([&]() {
            std::priority_queue<int, std::vector<int>, std::greater<>> pq;
            Timer t;
            for(int x : data) pq.push(x);
            return t.elapsed();
        });

        // Binary Heap Test
        double t_bin = runIsolated([&]() {
            BinaryHeap<int> pq;
            Timer t;
            for(int x : data) pq.insert(x);
            return t.elapsed();
        });
        
        double t_4ary = runIsolated([&]() {
            FourAryHeap<int> pq;
            Timer t;
            for(int x : data) pq.insert(x);
            return t.elapsed();
        });
        
        double t_fib = runIsolated([&]() {
            FibonacciHeap<int> pq;
            Timer t;
            for(int x : data) pq.insert(x);
            return t.elapsed();
        });
        
        double t_pair = runIsolated([&]() {
            PairingHeap<int> pq;
            Timer t;
            for(int x : data) pq.insert(x);
            return t.elapsed();
        });
        
        double t_pooled_fib = runIsolated([&]() {
            PooledFibonacciHeap<int> pq;
            Timer t;
            for(int x : data) pq.insert(x);
            return t.elapsed();
        });

        double t_pooled_pair = runIsolated([&]() {
            PooledPairingHeap<int> pq;
            Timer t;
            for(int x : data) pq.insert(x);
            return t.elapsed();
        });
        
        printf("%-10d | %-9s | %-9s | %-9s | %-9s | %-9s | %-9s | %-9s\n",
           N, fmt(t_stl).c_str(), fmt(t_bin).c_str(), fmt(t_4ary).c_str(), 
           fmt(t_fib).c_str(), fmt(t_pair).c_str(), 
           fmt(t_pooled_fib).c_str(), fmt(t_pooled_pair).c_str());
    }
}

void benchmarkDecreaseKeyHeavy() {
    std::cout << "\n=== Benchmark 3: Decrease-Key-Heavy Workload ===" << std::endl;
    std::cout << "Note: STL, Binary, and 4-ary use 'Lazy Insertion' (add duplicate) instead of DecreaseKey." << std::endl;
    
    // Updated header
    printf("%-10s | %-9s | %-9s | %-9s | %-9s | %-9s | %-9s | %-9s\n", 
           "Size", "STL(L)", "Bin(L)", "4ary(L)", "Fib", "Pair", "Fib(P)", "Pair(P)");
    printf("--------------------------------------------------------------------------------------\n");
    
    std::vector<int> sizes = {1000, 10000, 50000, 100000, 200000}; // Reduced max for speed
    
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
        
        double t_stl = runIsolated([&]() {
            std::priority_queue<int, std::vector<int>, std::greater<>> pq;
            std::vector<int> current = initialData;
            for(int x : current) pq.push(x);
            
            Timer t;
            for(auto p : opData) {
                int idx = p.first;
                int nv = std::max(1, current[idx] - p.second);
                current[idx] = nv;
                pq.push(nv);
            }
            return t.elapsed();
        });

        // Binary Heap Lazy
        double t_bin = runIsolated([&]() {
            BinaryHeap<int> pq;
            std::vector<int> current = initialData;
            for(int x : current) pq.insert(x);
            
            Timer t;
            for(auto p : opData) {
                int idx = p.first;
                int nv = std::max(1, current[idx] - p.second);
                current[idx] = nv;
                pq.insert(nv);
            }
            return t.elapsed();
        });
        
        double t_4ary = runIsolated([&]() {
            FourAryHeap<int> pq;
            std::vector<int> current = initialData;
            for(int x : current) pq.insert(x);
             
            Timer t;
            for(auto p : opData) {
                int idx = p.first;
                int nv = std::max(1, current[idx] - p.second);
                current[idx] = nv; 
                pq.insert(nv);
            }
            return t.elapsed();
        });
        
        double t_fib = runIsolated([&]() {
             FibonacciHeap<int> pq;
             std::vector<FibNode<int>*> nodes;
             nodes.reserve(N);
             for(int x : initialData) nodes.push_back(pq.insert(x));
             
             Timer t;
             for(auto p : opData) {
                 int idx = p.first;
                 int nv = std::max(1, nodes[idx]->value - p.second);
                 pq.decreaseKey(nodes[idx], nv);
             }
             return t.elapsed();
        });
        
        double t_pair = runIsolated([&]() {
             PairingHeap<int> pq;
             std::vector<PairingNode<int>*> nodes;
             nodes.reserve(N);
             for(int x : initialData) nodes.push_back(pq.insert(x));
             
             Timer t;
             for(auto p : opData) {
                 int idx = p.first;
                 int nv = std::max(1, nodes[idx]->value - p.second);
                 pq.decreaseKey(nodes[idx], nv);
             }
             return t.elapsed();
        });

        double t_pooled_fib = runIsolated([&]() {
             PooledFibonacciHeap<int> pq;
             std::vector<FibNode<int>*> nodes;
             nodes.reserve(N);
             for(int x : initialData) nodes.push_back(pq.insert(x));
             
             Timer t;
             for(auto p : opData) {
                 int idx = p.first;
                 int nv = std::max(1, nodes[idx]->value - p.second);
                 pq.decreaseKey(nodes[idx], nv);
             }
             return t.elapsed();
        });

        double t_pooled = runIsolated([&]() {
             PooledPairingHeap<int> pq;
             std::vector<PairingNode<int>*> nodes;
             nodes.reserve(N);
             for(int x : initialData) nodes.push_back(pq.insert(x));
             
             Timer t;
             for(auto p : opData) {
                 int idx = p.first;
                 int nv = std::max(1, nodes[idx]->value - p.second);
                 pq.decreaseKey(nodes[idx], nv);
             }
             return t.elapsed();
        });
        
        printf("%-10d | %-9s | %-9s | %-9s | %-9s | %-9s | %-9s | %-9s\n",
           N, fmt(t_stl).c_str(), fmt(t_bin).c_str(), fmt(t_4ary).c_str(), 
           fmt(t_fib).c_str(), fmt(t_pair).c_str(), 
           fmt(t_pooled_fib).c_str(), fmt(t_pooled).c_str());
    }
}

void benchmarkMergeSequence() {
    std::cout << "\n=== Benchmark 1: Sequence of Merges (Pure Merge) ===" << std::endl;
    // Updated header
    printf("%-10s | %-9s | %-9s | %-9s | %-9s | %-9s\n", 
           "Size", "STL", "Binary", "4-ary", "Fib", "Pair");
    printf("----------------------------------------------------------------------\n");
    
    std::vector<int> totalElements = {10000, 100000, 500000};
    const int HEAP_SIZE = 100;
    
    for (int TotalN : totalElements) {
        int numHeaps = TotalN / HEAP_SIZE;
        std::vector<std::vector<int>> heapData(numHeaps);
        std::mt19937 gen(42);
        std::uniform_int_distribution<> dis(1, 1000000);
        
        for(int i=0; i<numHeaps; ++i) {
            for(int j=0; j<HEAP_SIZE; ++j) heapData[i].push_back(dis(gen));
        }
        
        double t_stl = runIsolated([&]() {
            std::vector<std::priority_queue<int, std::vector<int>, std::greater<>>> heaps(numHeaps);
            for(int i=0; i<numHeaps; ++i) 
                for(int x : heapData[i]) heaps[i].push(x);
            
            Timer t;
            for(int i=1; i<numHeaps; ++i) {
                while(!heaps[i].empty()) {
                    heaps[0].push(heaps[i].top());
                    heaps[i].pop();
                }
            }
            return t.elapsed();
        });

        // Binary Heap Merge
        double t_bin = runIsolated([&]() {
            std::vector<BinaryHeap<int>> heaps(numHeaps);
            for(int i=0; i<numHeaps; ++i) 
                for(int x : heapData[i]) heaps[i].insert(x);
            
            Timer t;
            for(int i=1; i<numHeaps; ++i) {
                // Binary heaps don't have efficient merge, so we act like 4-ary
                while(!heaps[i].isEmpty()) {
                    heaps[0].insert(heaps[i].extractMin());
                }
            }
            return t.elapsed();
        });
        
        double t_4ary = runIsolated([&]() {
            std::vector<FourAryHeap<int>> heaps(numHeaps);
            for(int i=0; i<numHeaps; ++i) 
                for(int x : heapData[i]) heaps[i].insert(x);
            
            Timer t;
            for(int i=1; i<numHeaps; ++i) {
                while(!heaps[i].isEmpty()) {
                    heaps[0].insert(heaps[i].extractMin());
                }
            }
            return t.elapsed();
        });
        
        double t_fib = runIsolated([&]() {
            std::vector<FibonacciHeap<int>> heaps(numHeaps);
            for(int i=0; i<numHeaps; ++i) 
                for(int x : heapData[i]) heaps[i].insert(x);
            
            Timer t;
            for(int i=1; i<numHeaps; ++i) heaps[0].merge(heaps[i]);
            return t.elapsed();
        });
        
        double t_pair = runIsolated([&]() {
            std::vector<PairingHeap<int>> heaps(numHeaps);
            for(int i=0; i<numHeaps; ++i) 
                for(int x : heapData[i]) heaps[i].insert(x);
             
            Timer t;
            for(int i=1; i<numHeaps; ++i) heaps[0].merge(heaps[i]);
            return t.elapsed();
        });
        
       printf("%-10d | %-9s | %-9s | %-9s | %-9s | %-9s\n",
           TotalN, fmt(t_stl).c_str(), fmt(t_bin).c_str(), fmt(t_4ary).c_str(), fmt(t_fib).c_str(), fmt(t_pair).c_str());
    }
}

void benchmarkDijkstraLazy() {
    std::cout << "\n=== Benchmark 4a: Dijkstra (Lazy Variants) ===" << std::endl;
    // Updated header
    printf("%-9s | %-9s | %-9s | %-9s | %-9s | %-9s | %-9s | %-9s\n", 
           "Vertices", "STL", "Binary", "4-ary", "Fib(L)", "Pair(L)", "Fib(P)", "Pair(P)");
    printf("------------------------------------------------------------------------------------------\n");
    
    std::vector<int> vertCounts = {1000, 5000, 10000, 20000, 50000};
    
    for (int V : vertCounts) {
        Graph g = generateRandomGraph(V, 20, 100);
        
        // Collect stats once (using STL)
        OpStats stats;
        dijkstraSTL(g, 0, &stats);
        
        double t_stl = runBenchmark([&](){ dijkstraSTL(g, 0); });
        double t_bin = runBenchmark([&](){ dijkstraBinaryHeap(g, 0); }); // Binary Heap
        double t_4ary = runBenchmark([&](){ dijkstraFourAryHeap(g, 0); });
        double t_fib = runBenchmark([&](){ dijkstraFibonacciHeapLazy(g, 0); });
        double t_pair = runBenchmark([&](){ dijkstraPairingHeapLazy(g, 0); });

        double t_pooled_fib_lazy = runBenchmark([&](){ 
             PooledFibonacciHeap<std::pair<int, int>> pq;
             std::vector<int> dist(g.size(), std::numeric_limits<int>::max());
             dist[0] = 0;
             pq.insert({0, 0});
             while(!pq.isEmpty()) {
                 auto [d, u] = pq.extractMin();
                 if(d > dist[u]) continue;
                 for(const auto& edge : g[u]) {
                     if(dist[u] + edge.weight < dist[edge.to]) {
                         dist[edge.to] = dist[u] + edge.weight;
                         pq.insert({dist[edge.to], edge.to});
                     }
                 }
             }
        });

        double t_pooled_pair_lazy = runBenchmark([&](){ 
             PooledPairingHeap<std::pair<int, int>> pq;
             std::vector<int> dist(g.size(), std::numeric_limits<int>::max());
             dist[0] = 0;
             pq.insert({0, 0});
             while(!pq.isEmpty()) {
                 auto [d, u] = pq.extractMin();
                 if(d > dist[u]) continue;
                 for(const auto& edge : g[u]) {
                     if(dist[u] + edge.weight < dist[edge.to]) {
                         dist[edge.to] = dist[u] + edge.weight;
                         pq.insert({dist[edge.to], edge.to});
                     }
                 }
             }
        });

        printf("%-9d | %-9s | %-9s | %-9s | %-9s | %-9s | %-9s | %-9s\n",
               V, 
               fmt(t_stl).c_str(), fmt(t_bin).c_str(), fmt(t_4ary).c_str(), 
               fmt(t_fib).c_str(), fmt(t_pair).c_str(),
               fmt(t_pooled_fib_lazy).c_str(), fmt(t_pooled_pair_lazy).c_str());
    }
}

void benchmarkDijkstraDecKey() {
    std::cout << "\n=== Benchmark 4b: Dijkstra (Decrease-Key Variants) ===" << std::endl;
    std::cout << "Binary and 4-ary heaps are omitted here as they don't support O(1) decrease-key." << std::endl;
    
    printf("%-9s | %-9s | %-9s | %-9s | %-9s | %-16s\n", 
           "Vertices", "Fib(Dec)", "Pair(Dec)", "Fib(P)", "Pair(P)", "Ops(Ins/Dec/Ext)");
    printf("--------------------------------------------------------------------------------\n");
    
    std::vector<int> vertCounts = {1000, 5000, 10000, 20000, 50000};
    
    for (int V : vertCounts) {
        Graph g = generateRandomGraph(V, 20, 100);
        
        OpStats stats;
        dijkstraFibonacciHeap(g, 0, &stats);
        
        double t_fib_dec = runBenchmark([&](){ dijkstraFibonacciHeap(g, 0); });
        double t_pair_dec = runBenchmark([&](){ dijkstraPairingHeap(g, 0); });
        
        double t_pooled_fib = runBenchmark([&](){ 
             int n = g.size();
             std::vector<int> dist(n, std::numeric_limits<int>::max());
             std::vector<FibNode<std::pair<int, int>>*> nodeHandles(n, nullptr);
             PooledFibonacciHeap<std::pair<int, int>> pq;
             
             dist[0] = 0;
             nodeHandles[0] = pq.insert({0, 0});
             
             while (!pq.isEmpty()) {
                 auto [d, u] = pq.extractMin();
                 if (d > dist[u]) continue;
                 for (const Edge& edge : g[u]) {
                     int v = edge.to;
                     int newDist = dist[u] + edge.weight;
                     if (newDist < dist[v]) {
                         if (dist[v] == std::numeric_limits<int>::max()) {
                             dist[v] = newDist;
                             nodeHandles[v] = pq.insert({newDist, v});
                         } else {
                             dist[v] = newDist;
                             pq.decreaseKey(nodeHandles[v], {newDist, v});
                         }
                     }
                 }
             }
        });

        double t_pooled = runBenchmark([&](){ 
             int n = g.size();
             std::vector<int> dist(n, std::numeric_limits<int>::max());
             std::vector<PairingNode<std::pair<int, int>>*> nodeHandles(n, nullptr);
             PooledPairingHeap<std::pair<int, int>> pq;
             
             dist[0] = 0;
             nodeHandles[0] = pq.insert({0, 0});
             
             while (!pq.isEmpty()) {
                 auto [d, u] = pq.extractMin();
                 if (d > dist[u]) continue;
                 for (const Edge& edge : g[u]) {
                     int v = edge.to;
                     int newDist = dist[u] + edge.weight;
                     if (newDist < dist[v]) {
                         if (dist[v] == std::numeric_limits<int>::max()) {
                             dist[v] = newDist;
                             nodeHandles[v] = pq.insert({newDist, v});
                         } else {
                             dist[v] = newDist;
                             pq.decreaseKey(nodeHandles[v], {newDist, v});
                         }
                     }
                 }
             }
        });
        
        std::string opsStr = std::to_string(stats.inserts) + "/" + 
                             std::to_string(stats.decreaseKeys) + "/" + 
                             std::to_string(stats.extractMins);
        
        printf("%-9d | %-9s | %-9s | %-9s | %-9s | %-16s\n",
               V, 
               fmt(t_fib_dec).c_str(), fmt(t_pair_dec).c_str(), 
               fmt(t_pooled_fib).c_str(), fmt(t_pooled).c_str(),
               opsStr.c_str());
    }
}


int main() {
    std::cout << "=== Priority Queue Benchmark Suite (Complete) ===" << std::endl;
    std::cout << "Runs per benchmark: 20 (+5 warmup)" << std::endl;
    
    benchmarkMergeSequence();
    benchmarkInsertHeavy();
    benchmarkDecreaseKeyHeavy();
    
    // Split Dijkstra benchmarks
    benchmarkDijkstraLazy();
    benchmarkDijkstraDecKey();
    
    return 0;
}