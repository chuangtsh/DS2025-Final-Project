#include <iostream>
#include "baseline.h"
#include "BinaryHeap.h"
#include "FourAryHeap.h"
#include "FibonacciHeap.h"
int main() {
    std::cout << "=== Baseline Heap Implementations ===" << std::endl << std::endl;
    
    // Test 1: Basic operations
    std::cout << "Test 1: STL Priority Queue" << std::endl;
    BaselinePriorityQueue<int> pq;
    
    std::cout << "Inserting: 5, 3, 8, 1, 9, 2, 7" << std::endl;
    pq.insert(5);
    pq.insert(3);
    pq.insert(8);
    pq.insert(1);
    pq.insert(9);
    pq.insert(2);
    pq.insert(7);
    
    std::cout << "Queue size: " << pq.size() << std::endl;
    std::cout << "Minimum element: " << pq.findMin() << std::endl;
    
    std::cout << "Extracting elements in sorted order: ";
    while (!pq.isEmpty()) {
        std::cout << pq.extractMin() << " ";
    }
    std::cout << std::endl << std::endl;

    // Test 1.5: Custom Binary Heap (New!)
    std::cout << "========================================" << std::endl;
    std::cout << "Test 1.5: Custom Binary Heap" << std::endl;
    std::cout << "========================================" << std::endl;
    BinaryHeap<int> binHeap;
    
    std::cout << "Inserting: 5, 3, 8, 1, 9, 2, 7" << std::endl;
    binHeap.insert(5);
    binHeap.insert(3);
    binHeap.insert(8);
    binHeap.insert(1);
    binHeap.insert(9);
    binHeap.insert(2);
    binHeap.insert(7);
    
    std::cout << "Queue size: " << binHeap.size() << std::endl;
    std::cout << "Minimum element: " << binHeap.findMin() << std::endl;
    
    std::cout << "Extracting elements in sorted order: ";
    while (!binHeap.isEmpty()) {
        std::cout << binHeap.extractMin() << " ";
    }
    std::cout << std::endl;
    std::cout << "NOTE: This is the custom implementation to compare against 4-ary heap." << std::endl;
    std::cout << std::endl << std::endl;
    
    // Test 2: 4-ary Heap
    std::cout << "========================================" << std::endl;
    std::cout << "Test 2: 4-ary Heap" << std::endl;
    std::cout << "========================================" << std::endl;
    FourAryHeap<int> fourAry;
    
    std::cout << "Inserting: 5, 3, 8, 1, 9, 2, 7" << std::endl;
    fourAry.insert(5);
    fourAry.insert(3);
    fourAry.insert(8);
    fourAry.insert(1);
    fourAry.insert(9);
    fourAry.insert(2);
    fourAry.insert(7);
    
    std::cout << "Queue size: " << fourAry.size() << std::endl;
    std::cout << "Minimum element: " << fourAry.findMin() << std::endl;
    
    std::cout << "Extracting elements in sorted order: ";
    while (!fourAry.isEmpty()) {
        std::cout << fourAry.extractMin() << " ";
    }
    std::cout << std::endl;
    
    std::cout << "NOTE: 4-ary heap offers better cache performance than binary heap" << std::endl;
    std::cout << "but still does NOT support efficient decrease-key operation." << std::endl;
    std::cout << std::endl << std::endl;
    
    // Test 3: Fibonacci Heap
    std::cout << "========================================" << std::endl;
    std::cout << "Test 3: Fibonacci Heap" << std::endl;
    std::cout << "========================================" << std::endl;
    FibonacciHeap<int> fibHeap;
    
    std::cout << "Inserting: 5, 3, 8, 1, 9, 2, 7" << std::endl;
    auto node5 = fibHeap.insert(5);
    auto node3 = fibHeap.insert(3);
    fibHeap.insert(8);
    fibHeap.insert(1);
    fibHeap.insert(9);
    fibHeap.insert(2);
    fibHeap.insert(7);
    
    std::cout << "Queue size: " << fibHeap.size() << std::endl;
    std::cout << "Minimum element: " << fibHeap.findMin() << std::endl;
    
    std::cout << "\nTesting decrease-key operation:" << std::endl;
    std::cout << "Decrease key of node(5) to 0" << std::endl;
    fibHeap.decreaseKey(node5, 0);
    std::cout << "New minimum: " << fibHeap.findMin() << std::endl;
    
    std::cout << "Decrease key of node(3) to -1" << std::endl;
    fibHeap.decreaseKey(node3, -1);
    std::cout << "New minimum: " << fibHeap.findMin() << std::endl;
    
    std::cout << "\nExtracting elements in sorted order: ";
    while (!fibHeap.isEmpty()) {
        std::cout << fibHeap.extractMin() << " ";
    }
    std::cout << std::endl;
    
    std::cout << "NOTE: Fibonacci heap supports O(1) amortized decrease-key," << std::endl;
    std::cout << "similar to Pairing Heap, making it suitable for Dijkstra's algorithm." << std::endl;
    std::cout << std::endl << std::endl;
    
    std::cout << "========================================" << std::endl;
    std::cout << "Summary Comparison" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "1. STL priority_queue (Binary Heap):" << std::endl;
    std::cout << "   - Insert: O(log n)" << std::endl;
    std::cout << "   - Extract-Min: O(log n)" << std::endl;
    std::cout << "   - Decrease-Key: NOT SUPPORTED" << std::endl;
    std::cout << "   - Merge: NOT SUPPORTED" << std::endl;
    std::cout << std::endl;
    
    std::cout << "2. 4-ary Heap:" << std::endl;
    std::cout << "   - Insert: O(log_4 n)" << std::endl;
    std::cout << "   - Extract-Min: O(4 * log_4 n)" << std::endl;
    std::cout << "   - Decrease-Key: NOT SUPPORTED" << std::endl;
    std::cout << "   - Better cache performance than binary heap" << std::endl;
    std::cout << std::endl;
    
    std::cout << "3. Fibonacci Heap:" << std::endl;
    std::cout << "   - Insert: O(1) amortized" << std::endl;
    std::cout << "   - Extract-Min: O(log n) amortized" << std::endl;
    std::cout << "   - Decrease-Key: O(1) amortized ✓" << std::endl;
    std::cout << "   - Merge: O(1) ✓" << std::endl;
    std::cout << "   - More complex implementation, higher constant factors" << std::endl;
    std::cout << std::endl;
    
    std::cout << "4. Pairing Heap (Your Implementation):" << std::endl;
    std::cout << "   - Insert: O(1)" << std::endl;
    std::cout << "   - Extract-Min: O(log n) amortized" << std::endl;
    std::cout << "   - Decrease-Key: O(1) amortized ✓" << std::endl;
    std::cout << "   - Merge: O(1) ✓" << std::endl;
    std::cout << "   - Simpler than Fibonacci, often faster in practice" << std::endl;
    std::cout << std::endl;
    
    std::cout << "=== Baseline Tests Completed ===" << std::endl;
    
    return 0;
}
