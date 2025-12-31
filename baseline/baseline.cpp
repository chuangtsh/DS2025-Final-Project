#include <iostream>
#include <queue>
#include <vector>
#include <functional>

// Baseline Priority Queue using STL
// NOTE: std::priority_queue does NOT support efficient decrease-key
// This is a key limitation compared to pairing heaps

template<typename T>
class BaselinePriorityQueue {
private:
    std::priority_queue<T, std::vector<T>, std::greater<T>> pq;  // Min-heap
    
public:
    // Insert an element
    void insert(const T& value) {
        pq.push(value);
    }
    
    // Get minimum element
    const T& findMin() const {
        if (isEmpty()) {
            throw std::runtime_error("Queue is empty");
        }
        return pq.top();
    }
    
    // Remove minimum element
    void deleteMin() {
        if (isEmpty()) {
            throw std::runtime_error("Queue is empty");
        }
        pq.pop();
    }
    
    // Extract minimum element
    T extractMin() {
        T minValue = findMin();
        deleteMin();
        return minValue;
    }
    
    // Check if empty
    bool isEmpty() const {
        return pq.empty();
    }
    
    // Get size
    size_t size() const {
        return pq.size();
    }
    
    // Clear the queue
    void clear() {
        while (!isEmpty()) {
            deleteMin();
        }
    }
};

int main() {
    std::cout << "=== Baseline STL Priority Queue Implementation ===" << std::endl << std::endl;
    
    // Test 1: Basic operations
    std::cout << "Test 1: Basic Insert and Extract Operations" << std::endl;
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
    
    // Test 2: Large dataset
    std::cout << "Test 2: Performance with Larger Dataset" << std::endl;
    BaselinePriorityQueue<int> largePQ;
    
    std::cout << "Inserting 1000 elements..." << std::endl;
    for (int i = 1000; i > 0; i--) {
        largePQ.insert(i);
    }
    
    std::cout << "Queue size: " << largePQ.size() << std::endl;
    std::cout << "Minimum: " << largePQ.findMin() << std::endl;
    
    std::cout << "Extracting first 10 elements: ";
    for (int i = 0; i < 10 && !largePQ.isEmpty(); i++) {
        std::cout << largePQ.extractMin() << " ";
    }
    std::cout << std::endl;
    std::cout << "Remaining elements: " << largePQ.size() << std::endl << std::endl;
    
    std::cout << "NOTE: This baseline does NOT support decrease-key operation" << std::endl;
    std::cout << "For Dijkstra's algorithm, the workaround is to insert duplicate" << std::endl;
    std::cout << "entries with updated values, which is less efficient." << std::endl << std::endl;
    
    std::cout << "=== Baseline Tests Completed ===" << std::endl;
    
    return 0;
}
