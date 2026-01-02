#ifndef BASELINE_H
#define BASELINE_H

#include <vector>
#include <queue>
#include <functional>
#include <stdexcept>
#include <cmath>

// Baseline Priority Queue using STL
template<typename T>
class BaselinePriorityQueue {
private:
    std::priority_queue<T, std::vector<T>, std::greater<T>> pq;  // Min-heap
    
public:
    void insert(const T& value) {
        pq.push(value);
    }
    
    const T& findMin() const {
        if (isEmpty()) {
            throw std::runtime_error("Queue is empty");
        }
        return pq.top();
    }
    
    void deleteMin() {
        if (isEmpty()) {
            throw std::runtime_error("Queue is empty");
        }
        pq.pop();
    }
    
    T extractMin() {
        T minValue = findMin();
        deleteMin();
        return minValue;
    }
    
    bool isEmpty() const {
        return pq.empty();
    }
    
    size_t size() const {
        return pq.size();
    }
    
    void clear() {
        while (!isEmpty()) {
            deleteMin();
        }
    }
};

#endif // BASELINE_H
