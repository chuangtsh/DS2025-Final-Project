#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>
#include <random>
#include <limits>
#include <map>
#include <iomanip>
#include "../datastructure/PairingHeap.h"
#include <queue>

// Graph structure for Dijkstra's algorithm
struct Edge {
    int to;
    int weight;
};

using Graph = std::vector<std::vector<Edge>>;

// Dijkstra using Pairing Heap with decrease-key
std::vector<int> dijkstraPairingHeap(const Graph& graph, int source) {
    int n = graph.size();
    std::vector<int> dist(n, std::numeric_limits<int>::max());
    std::vector<PairingNode<std::pair<int, int>>*> nodeHandles(n, nullptr);
    
    PairingHeap<std::pair<int, int>> pq;  // {distance, vertex}
    
    dist[source] = 0;
    nodeHandles[source] = pq.insert({0, source});
    
    while (!pq.isEmpty()) {
        auto [d, u] = pq.extractMin();
        
        if (d > dist[u]) continue;  // Already processed with better distance
        
        for (const Edge& edge : graph[u]) {
            int v = edge.to;
            int newDist = dist[u] + edge.weight;
            
            if (newDist < dist[v]) {
                if (dist[v] == std::numeric_limits<int>::max()) {
                    // First time visiting this vertex
                    dist[v] = newDist;
                    nodeHandles[v] = pq.insert({newDist, v});
                } else {
                    // Use decrease-key operation
                    dist[v] = newDist;
                    pq.decreaseKey(nodeHandles[v], {newDist, v});
                }
            }
        }
    }
    
    return dist;
}

// Dijkstra using STL priority_queue (no decrease-key, inserts duplicates)
std::vector<int> dijkstraSTL(const Graph& graph, int source) {
    int n = graph.size();
    std::vector<int> dist(n, std::numeric_limits<int>::max());
    std::priority_queue<std::pair<int, int>, 
                        std::vector<std::pair<int, int>>,
                        std::greater<>> pq;
    
    dist[source] = 0;
    pq.push({0, source});
    
    while (!pq.empty()) {
        auto [d, u] = pq.top();
        pq.pop();
        
        if (d > dist[u]) continue;
        
        for (const Edge& edge : graph[u]) {
            int v = edge.to;
            int newDist = dist[u] + edge.weight;
            
            if (newDist < dist[v]) {
                dist[v] = newDist;
                pq.push({newDist, v});  // Insert duplicate instead of decrease-key
            }
        }
    }
    
    return dist;
}

// Generate random graph
Graph generateRandomGraph(int vertices, int avgDegree, int maxWeight) {
    Graph graph(vertices);
    std::random_device rd;
    std::mt19937 gen(42);  // Fixed seed for reproducibility
    std::uniform_int_distribution<> vertexDist(0, vertices - 1);
    std::uniform_int_distribution<> weightDist(1, maxWeight);
    
    for (int u = 0; u < vertices; u++) {
        int edges = avgDegree + (gen() % 5) - 2;  // Add some randomness
        for (int i = 0; i < edges; i++) {
            int v = vertexDist(gen);
            if (v != u) {
                int weight = weightDist(gen);
                graph[u].push_back({v, weight});
            }
        }
    }
    
    return graph;
}

// Timer utility
class Timer {
    std::chrono::high_resolution_clock::time_point start;
public:
    Timer() : start(std::chrono::high_resolution_clock::now()) {}
    
    double elapsed() const {
        auto end = std::chrono::high_resolution_clock::now();
        return std::chrono::duration<double, std::milli>(end - start).count();
    }
};

// Benchmark 1: Insert-heavy workload
void benchmarkInsertHeavy() {
    std::cout << "\n=== Benchmark 1: Insert-Heavy Workload ===" << std::endl;
    
    const int NUM_ELEMENTS = 100000;
    std::random_device rd;
    std::mt19937 gen(42);
    std::uniform_int_distribution<> dis(1, 1000000);
    
    std::vector<int> testData;
    for (int i = 0; i < NUM_ELEMENTS; i++) {
        testData.push_back(dis(gen));
    }
    
    // Pairing Heap
    {
        PairingHeap<int> heap;
        Timer timer;
        for (int val : testData) {
            heap.insert(val);
        }
        double elapsed = timer.elapsed();
        std::cout << "Pairing Heap: " << NUM_ELEMENTS << " inserts in " 
                  << std::fixed << std::setprecision(2) << elapsed << " ms" << std::endl;
    }
    
    // STL Priority Queue
    {
        std::priority_queue<int, std::vector<int>, std::greater<>> pq;
        Timer timer;
        for (int val : testData) {
            pq.push(val);
        }
        double elapsed = timer.elapsed();
        std::cout << "STL Priority Queue: " << NUM_ELEMENTS << " inserts in " 
                  << elapsed << " ms" << std::endl;
    }
}

// Benchmark 2: Decrease-key-heavy workload
void benchmarkDecreaseKeyHeavy() {
    std::cout << "\n=== Benchmark 2: Decrease-Key-Heavy Workload ===" << std::endl;
    
    const int NUM_ELEMENTS = 10000;
    const int NUM_DECREASES = 50000;
    
    std::random_device rd;
    std::mt19937 gen(42);
    std::uniform_int_distribution<> valDis(100000, 1000000);  // Start with larger values
    
    // Pairing Heap with decrease-key
    {
        PairingHeap<int> heap;
        std::vector<PairingNode<int>*> nodes;
        
        // Insert initial elements
        for (int i = 0; i < NUM_ELEMENTS; i++) {
            nodes.push_back(heap.insert(valDis(gen)));
        }
        
        Timer timer;
        std::uniform_int_distribution<> nodeDis(0, NUM_ELEMENTS - 1);
        std::uniform_int_distribution<> decreaseDis(1, 10000);
        
        for (int i = 0; i < NUM_DECREASES; i++) {
            int idx = nodeDis(gen);
            int decrease = decreaseDis(gen);
            int newVal = std::max(1, nodes[idx]->value - decrease);  // Ensure valid decrease
            heap.decreaseKey(nodes[idx], newVal);
        }
        
        double elapsed = timer.elapsed();
        std::cout << "Pairing Heap: " << NUM_DECREASES << " decrease-key ops in " 
                  << std::fixed << std::setprecision(2) << elapsed << " ms" << std::endl;
    }
    
    // STL Priority Queue (simulate with re-insert)
    {
        std::priority_queue<int, std::vector<int>, std::greater<>> pq;
        std::vector<int> values;
        
        for (int i = 0; i < NUM_ELEMENTS; i++) {
            int val = valDis(gen);
            values.push_back(val);
            pq.push(val);
        }
        
        Timer timer;
        std::uniform_int_distribution<> nodeDis(0, NUM_ELEMENTS - 1);
        std::uniform_int_distribution<> decreaseDis(1, 10000);
        
        for (int i = 0; i < NUM_DECREASES; i++) {
            int idx = nodeDis(gen);
            int decrease = decreaseDis(gen);
            int newVal = std::max(1, values[idx] - decrease);
            values[idx] = newVal;
            pq.push(newVal);  // Must insert duplicate (no decrease-key)
        }
        
        double elapsed = timer.elapsed();
        std::cout << "STL Priority Queue (re-insert): " << NUM_DECREASES 
                  << " simulated decrease-keys in " << elapsed << " ms" << std::endl;
        std::cout << "  (Note: STL queue size inflated to " << pq.size() 
                  << " due to duplicates)" << std::endl;
    }
}

// Benchmark 3: Sequence of merges
void benchmarkMergeSequence() {
    std::cout << "\n=== Benchmark 3: Sequence of Merges ===" << std::endl;
    
    const int NUM_HEAPS = 1000;
    const int ELEMENTS_PER_HEAP = 100;
    
    std::random_device rd;
    std::mt19937 gen(42);
    std::uniform_int_distribution<> dis(1, 100000);
    
    // Pairing Heap merges
    {
        std::vector<PairingHeap<int>> heaps(NUM_HEAPS);
        
        // Populate heaps
        for (int i = 0; i < NUM_HEAPS; i++) {
            for (int j = 0; j < ELEMENTS_PER_HEAP; j++) {
                heaps[i].insert(dis(gen));
            }
        }
        
        Timer timer;
        
        // Merge all heaps into first heap
        for (int i = 1; i < NUM_HEAPS; i++) {
            heaps[0].merge(heaps[i]);
        }
        
        double elapsed = timer.elapsed();
        std::cout << "Pairing Heap: Merged " << NUM_HEAPS << " heaps (" 
                  << NUM_HEAPS * ELEMENTS_PER_HEAP << " total elements) in "
                  << std::fixed << std::setprecision(2) << elapsed << " ms" << std::endl;
        std::cout << "  Final heap size: " << heaps[0].size() << std::endl;
    }
    
    std::cout << "  (Note: STL priority_queue doesn't support merge operation)" << std::endl;
}

// Benchmark 4: Dijkstra's algorithm
void benchmarkDijkstra() {
    std::cout << "\n=== Benchmark 4: Dijkstra's Algorithm ===" << std::endl;
    
    const int VERTICES = 5000;
    const int AVG_DEGREE = 20;
    const int MAX_WEIGHT = 100;
    
    std::cout << "Generating random graph: " << VERTICES << " vertices, "
              << "avg degree " << AVG_DEGREE << std::endl;
    
    Graph graph = generateRandomGraph(VERTICES, AVG_DEGREE, MAX_WEIGHT);
    int source = 0;
    
    // Dijkstra with Pairing Heap
    std::vector<int> distPairing;
    {
        Timer timer;
        distPairing = dijkstraPairingHeap(graph, source);
        double elapsed = timer.elapsed();
        std::cout << "Pairing Heap (with decrease-key): " 
                  << std::fixed << std::setprecision(2) << elapsed << " ms" << std::endl;
    }
    
    // Dijkstra with STL
    std::vector<int> distSTL;
    {
        Timer timer;
        distSTL = dijkstraSTL(graph, source);
        double elapsed = timer.elapsed();
        std::cout << "STL Priority Queue (insert duplicates): " << elapsed << " ms" << std::endl;
    }
    
    // Verify results match
    bool match = (distPairing == distSTL);
    std::cout << "Results match: " << (match ? "YES" : "NO") << std::endl;
}

int main() {
    std::cout << "=== Priority Queue Benchmark Suite ===" << std::endl;
    std::cout << "Comparing Pairing Heap vs STL priority_queue" << std::endl;
    
    benchmarkInsertHeavy();
    benchmarkDecreaseKeyHeavy();
    benchmarkMergeSequence();
    benchmarkDijkstra();
    
    std::cout << "\n=== All Benchmarks Completed ===" << std::endl;
    
    return 0;
}
