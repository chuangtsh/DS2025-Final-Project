#ifndef BINARYHEAP_H
#define BINARYHEAP_H

#include <vector>
#include <algorithm>
#include <stdexcept>

// Binary Heap Implementation (Min-Heap)
template<typename T>
class BinaryHeap {
private:
    std::vector<T> heap;
    
    // 0-based indexing helper functions
    size_t parent(size_t i) const { return (i - 1) / 2; }
    size_t leftChild(size_t i) const { return 2 * i + 1; }
    size_t rightChild(size_t i) const { return 2 * i + 2; }
    
    void bubbleUp(size_t i) {
        while (i > 0) {
            size_t p = parent(i);
            // Min-Heap property check
            if (heap[i] >= heap[p]) break;
            std::swap(heap[i], heap[p]);
            i = p;
        }
    }
    
    void bubbleDown(size_t i) {
        while (true) {
            size_t smallest = i;
            size_t left = leftChild(i);
            size_t right = rightChild(i);
            
            // Check left child
            if (left < heap.size() && heap[left] < heap[smallest]) {
                smallest = left;
            }
            
            // Check right child
            if (right < heap.size() && heap[right] < heap[smallest]) {
                smallest = right;
            }
            
            if (smallest == i) break;
            
            std::swap(heap[i], heap[smallest]);
            i = smallest;
        }
    }
    
public:
    void insert(const T& value) {
        heap.push_back(value);
        bubbleUp(heap.size() - 1);
    }
    
    const T& findMin() const {
        if (isEmpty()) throw std::runtime_error("Heap is empty");
        return heap[0];
    }
    
    void deleteMin() {
        if (isEmpty()) throw std::runtime_error("Heap is empty");
        
        // Move last element to root and remove last element
        heap[0] = heap.back();
        heap.pop_back();
        
        // Restore heap property
        if (!heap.empty()) {
            bubbleDown(0);
        }
    }
    
    T extractMin() {
        T minValue = findMin();
        deleteMin();
        return minValue;
    }
    
    bool isEmpty() const { return heap.empty(); }
    size_t size() const { return heap.size(); }
    void clear() { heap.clear(); }
};

#endif // BINARYHEAP_H