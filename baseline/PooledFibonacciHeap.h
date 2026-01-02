#ifndef POOLED_FIBONACCIHEAP_H
#define POOLED_FIBONACCIHEAP_H

#include <vector>
#include <cmath>
#include <algorithm>
#include <stdexcept>
#include "FibonacciHeap.h"
#include "../datastructure/MemoryPool.h"

// Reuse FibNode from FibonacciHeap.h

// Pooled Fibonacci Heap Implementation
template<typename T>
class PooledFibonacciHeap {
private:
    FibNode<T>* minNode;
    size_t heapSize;
    MemoryPool<FibNode<T>> pool;
    
    void addToRootList(FibNode<T>* node) {
        if (minNode == nullptr) {
            minNode = node;
            node->left = node->right = node;
        } else {
            node->right = minNode->right;
            node->left = minNode;
            minNode->right->left = node;
            minNode->right = node;
        }
    }
    
    void removeFromList(FibNode<T>* node) {
        if (node->right == node) {
            // Only node in list
        } else {
            node->left->right = node->right;
            node->right->left = node->left;
        }
        node->left = node;
        node->right = node;
    }
    
    void consolidate() {
        int maxDegree = 2 * static_cast<int>(std::log2(heapSize + 1)) + 5;
        std::vector<FibNode<T>*> degreeTable(maxDegree + 1, nullptr);
        
        std::vector<FibNode<T>*> rootList;
        FibNode<T>* current = minNode;
        if (current != nullptr) {
            do {
                rootList.push_back(current);
                current = current->right;
            } while (current != minNode);
        }
        
        for (FibNode<T>* w : rootList) {
            FibNode<T>* x = w;
            int d = x->degree;
            
            while (degreeTable[d] != nullptr) {
                FibNode<T>* y = degreeTable[d];
                if (x->value > y->value) std::swap(x, y);
                
                removeFromList(y);
                y->left = y->right = y;
                
                if (x->child == nullptr) {
                    x->child = y;
                } else {
                    y->right = x->child->right;
                    y->left = x->child;
                    x->child->right->left = y;
                    x->child->right = y;
                }
                
                y->parent = x;
                y->marked = false;
                x->degree++;
                
                degreeTable[d] = nullptr;
                d++;
            }
            degreeTable[d] = x;
        }
        
        minNode = nullptr;
        for (FibNode<T>* node : degreeTable) {
            if (node != nullptr) {
                node->left = node->right = node;
                node->parent = nullptr;
                
                addToRootList(node);
                
                if (minNode == nullptr || node->value < minNode->value) {
                    minNode = node;
                }
            }
        }
    }
    
    void cut(FibNode<T>* x, FibNode<T>* y) {
        if (y->child == x) {
            y->child = (x->right == x) ? nullptr : x->right;
        }
        removeFromList(x);
        y->degree--;
        
        addToRootList(x);
        x->parent = nullptr;
        x->marked = false;
    }
    
    void cascadingCut(FibNode<T>* y) {
        FibNode<T>* z = y->parent;
        if (z != nullptr) {
            if (!y->marked) {
                y->marked = true;
            } else {
                cut(y, z);
                cascadingCut(z);
            }
        }
    }
    
    void deleteTree(FibNode<T>* node) {
        if (node == nullptr) return;
        
        std::vector<FibNode<T>*> stack;
        
        FibNode<T>* start = node;
        FibNode<T>* current = start;
        do {
            stack.push_back(current);
            current = current->right;
        } while (current != start);
        
        while (!stack.empty()) {
            FibNode<T>* curr = stack.back();
            stack.pop_back();
            
            if (curr->child != nullptr) {
                FibNode<T>* childStart = curr->child;
                FibNode<T>* child = childStart;
                do {
                    stack.push_back(child);
                    child = child->right;
                } while (child != childStart);
            }
            
            pool.deallocate(curr);
        }
    }
    
public:
    PooledFibonacciHeap() : minNode(nullptr), heapSize(0) {}
    
    ~PooledFibonacciHeap() {
        if (minNode != nullptr) {
            deleteTree(minNode);
        }
    }
    
    FibNode<T>* insert(const T& value) {
        FibNode<T>* newNode = pool.allocate(value);
        addToRootList(newNode);
        
        if (minNode == nullptr || value < minNode->value) {
            minNode = newNode;
        }
        
        heapSize++;
        return newNode;
    }
    
    const T& findMin() const {
        if (isEmpty()) throw std::runtime_error("Heap is empty");
        return minNode->value;
    }
    
    void deleteMin() {
        if (isEmpty()) throw std::runtime_error("Heap is empty");
        
        FibNode<T>* z = minNode;
        
        if (z->child != nullptr) {
            std::vector<FibNode<T>*> children;
            FibNode<T>* child = z->child;
            do {
                children.push_back(child);
                child = child->right;
            } while (child != z->child);
            
            for (FibNode<T>* c : children) {
                c->left = c->right = c;
                c->parent = nullptr;
                addToRootList(c);
            }
        }
        
        bool wasOnlyNode = (z->right == z);
        FibNode<T>* nextMin = z->right;
        
        removeFromList(z);
        
        if (wasOnlyNode) {
            minNode = nullptr;
        } else {
            minNode = nextMin;
            consolidate();
        }
        
        pool.deallocate(z);
        heapSize--;
    }
    
    T extractMin() {
        T minValue = findMin();
        deleteMin();
        return minValue;
    }
    
    void decreaseKey(FibNode<T>* x, const T& newValue) {
        if (newValue > x->value) {
            throw std::runtime_error("New value is greater than current value");
        }
        
        x->value = newValue;
        FibNode<T>* y = x->parent;
        
        if (y != nullptr && x->value < y->value) {
            cut(x, y);
            cascadingCut(y);
        }
        
        if (x->value < minNode->value) {
            minNode = x;
        }
    }
    
    bool isEmpty() const { return minNode == nullptr; }
    size_t size() const { return heapSize; }
    
    void clear() {
        if (minNode != nullptr) {
            deleteTree(minNode);
            minNode = nullptr;
            heapSize = 0;
        }
    }
};

#endif // POOLED_FIBONACCIHEAP_H
