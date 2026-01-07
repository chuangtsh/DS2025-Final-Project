#include <iostream>
#include "PairingHeap.h"

// Demo and testing
int main() {
    std::cout << "=== Pairing Heap Implementation Demo ===" << std::endl << std::endl;
    
    // Test 1: Basic operations
    std::cout << "Test 1: Basic Insert and Extract Operations" << std::endl;
    PairingHeap<int> heap;
    
    std::cout << "Inserting: 2, 5, 6, 1, 3, 4" << std::endl;
    heap.insert(2);
    heap.insert(5);
    heap.insert(6);
    heap.insert(1);
    heap.insert(3);
    heap.insert(4);

    heap.printVisualization("After Insertions");
    
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

    heap2.printVisualization("With Duplicates");

    std::cout << "Extracting: ";
    while (!heap2.isEmpty()) {
        std::cout << heap2.extractMin() << " ";
    }
    std::cout << std::endl << std::endl;
    
    // Test 3: Merge operation
    std::cout << "Test 3: Merging Two Heaps" << std::endl;
    PairingHeap<int> heap3, heap4;
    
    heap3.insert(1);
    heap3.insert(3);
    heap3.insert(4);

    heap3.printVisualization("heap3(merge)");
    
    heap4.insert(2);
    heap4.insert(5);
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
    
    // Test 4: Delete min
    std::cout << "Test 4: Delete Min Operation" << std::endl;

    PairingHeap<int> heap5;

    heap5.insert(2);
    heap5.insert(5);
    heap5.insert(6);
    heap5.insert(1);
    heap5.insert(3);
    heap5.insert(4);

    heap5.printVisualization("Before Delete Min");

    std::cout << "Heap size: " << heap5.size() << std::endl;
    std::cout << "Minimum: " << heap5.findMin() << std::endl;

    heap5.deleteMin();

    heap5.printVisualization("After Delete Min");

    std::cout << "After deleting min:" << std::endl;
    std::cout << "Heap size: " << heap5.size() << std::endl;
    std::cout << "Minimum: " << heap5.findMin() << std::endl;
    

    std::cout << "Remaining heap elements: ";
    for (int i = 0; i < 10 && !heap5.isEmpty(); i++) {
        std::cout << heap5.extractMin() << " ";
    }
    std::cout << std::endl << std::endl;

    // Test 5: Decrease-key operation (essential for Dijkstra)
    std::cout << "Test 5: Decrease-Key Operation" << std::endl;
    
    PairingHeap<int> heap6;
    auto node6 = heap6.insert(6);
    heap6.insert(9);
    heap6.insert(8);
    heap6.insert(7);

    PairingHeap<int> heap7;
    heap7.insert(2);
    heap7.insert(5);
    heap7.merge(heap6);

    PairingHeap<int> heap8;
    heap8.insert(1);
    heap8.insert(4);
    heap8.insert(3);
    heap8.merge(heap7);

    heap8.printVisualization("Before Decrease Key");

    std::cout << "Initial heap minimum: " << heap8.findMin() << std::endl;
    
    std::cout << "Decreasing 6 to 2..." << std::endl;
    heap8.decreaseKey(node6, 2);

    heap8.printVisualization("After Decrease Key");

    std::cout << "New minimum: " << heap8.findMin() << std::endl;
    std::cout << "Elements after decrease-key: ";
    while (!heap8.isEmpty()) {
        std::cout << heap8.extractMin() << " ";
    }
    std::cout << std::endl << std::endl;
    
    std::cout << "=== All Tests Completed Successfully ===" << std::endl;
    
    return 0;
}
