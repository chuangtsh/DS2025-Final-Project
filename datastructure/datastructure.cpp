#include <iostream>
#include "PairingHeap.h"

// Demo and testing
int main() {
    std::cout << "=== Pairing Heap Implementation Demo ===" << std::endl << std::endl;
    
    // Test 1: Basic operations
    std::cout << "Test 1: Basic Insert and Extract Operations" << std::endl;
    PairingHeap<int> heap;
    
    std::cout << "Inserting: 5, 3, 8, 1, 9, 2, 7" << std::endl;
    heap.insert(5);
    heap.insert(3);
    heap.insert(8);
    heap.insert(1);
    heap.insert(9);
    heap.insert(2);
    heap.insert(7);
    
    std::cout << "Heap size: " << heap.size() << std::endl;
    std::cout << "Minimum element: " << heap.findMin() << std::endl;
    
    std::cout << "Extracting elements in sorted order: ";
    while (!heap.isEmpty()) {
        std::cout << heap.extractMin() << " ";
    }
    std::cout << std::endl << std::endl;
    
    // Test 2: Heap with duplicates
    std::cout << "Test 2: Handling Duplicates" << std::endl;
    PairingHeap<int> heap2;
    
    std::cout << "Inserting: 5, 5, 3, 3, 3, 1" << std::endl;
    heap2.insert(5);
    heap2.insert(5);
    heap2.insert(3);
    heap2.insert(3);
    heap2.insert(3);
    heap2.insert(1);
    
    std::cout << "Extracting: ";
    while (!heap2.isEmpty()) {
        std::cout << heap2.extractMin() << " ";
    }
    std::cout << std::endl << std::endl;
    
    // Test 3: Merge operation
    std::cout << "Test 3: Merging Two Heaps" << std::endl;
    PairingHeap<int> heap3, heap4;
    
    heap3.insert(1);
    heap3.insert(5);
    heap3.insert(9);

    heap3.printVisualization("heap3(merge)");
    
    heap4.insert(2);
    heap4.insert(4);
    heap4.insert(6);

    heap4.printVisualization("heap4(merge)");

    std::cout << "Heap 3 size before merge: " << heap3.size() << std::endl;
    std::cout << "Heap 4 size before merge: " << heap4.size() << std::endl;
    
    heap3.merge(heap4);

    heap3.printVisualization("heap3+4(merged)");

    std::cout << "Heap 3 size after merge: " << heap3.size() << std::endl;
    std::cout << "Heap 4 size after merge: " << heap4.size() << std::endl;
    std::cout << "Merged heap elements: ";
    while (!heap3.isEmpty()) {
        std::cout << heap3.extractMin() << " ";
    }
    std::cout << std::endl << std::endl;
    
    // Test 4: Large dataset
    std::cout << "Test 4: Performance with Larger Dataset" << std::endl;
    PairingHeap<int> largeHeap;
    
    std::cout << "Inserting 1000 elements..." << std::endl;
    for (int i = 1000; i > 0; i--) {
        largeHeap.insert(i);
    }
    
    std::cout << "Heap size: " << largeHeap.size() << std::endl;
    std::cout << "Minimum: " << largeHeap.findMin() << std::endl;
    
    std::cout << "Extracting first 10 elements: ";
    for (int i = 0; i < 10 && !largeHeap.isEmpty(); i++) {
        std::cout << largeHeap.extractMin() << " ";
    }
    std::cout << std::endl;
    std::cout << "Remaining elements: " << largeHeap.size() << std::endl << std::endl;
    
    // Test 5: Decrease-key operation (essential for Dijkstra)
    std::cout << "Test 5: Decrease-Key Operation" << std::endl;
    PairingHeap<int> heap5;
    
    auto node1 = heap5.insert(50);
    auto node2 = heap5.insert(30);
    auto node3 = heap5.insert(70);
    auto node4 = heap5.insert(40);
    
    std::cout << "Initial heap minimum: " << heap5.findMin() << std::endl;
    
    std::cout << "Decreasing 70 to 10..." << std::endl;
    heap5.decreaseKey(node3, 10);
    
    std::cout << "New minimum: " << heap5.findMin() << std::endl;
    std::cout << "Elements after decrease-key: ";
    while (!heap5.isEmpty()) {
        std::cout << heap5.extractMin() << " ";
    }
    std::cout << std::endl << std::endl;
    
    std::cout << "=== All Tests Completed Successfully ===" << std::endl;
    
    return 0;
}
