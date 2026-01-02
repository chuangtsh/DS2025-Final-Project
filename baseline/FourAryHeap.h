#ifndef FOURARYHEAP_H
#define FOURARYHEAP_H

#include <vector>
#include <algorithm>
#include <stdexcept>

// 4-ary Heap Implementation
template<typename T>
class FourAryHeap {
private:
    std::vector<T> heap;
    
    size_t parent(size_t i) const { return (i - 1) / 4; }
    size_t child(size_t i, int k) const { return 4 * i + k + 1; }  // k in [0,3]
    
    void bubbleUp(size_t i) {
        while (i > 0) {
            size_t p = parent(i);
            if (heap[i] >= heap[p]) break;
            std::swap(heap[i], heap[p]);
            i = p;
        }
    }
    
    void bubbleDown(size_t i) {
        while (true) {
            size_t smallest = i;
            
            // Check all 4 children
            for (int k = 0; k < 4; k++) {
                size_t c = child(i, k);
                if (c < heap.size() && heap[c] < heap[smallest]) {
                    smallest = c;
                }
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
        heap[0] = heap.back();
        heap.pop_back();
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

#endif // FOURARYHEAP_H
