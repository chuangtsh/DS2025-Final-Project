#ifndef POOLED_PAIRING_HEAP_H
#define POOLED_PAIRING_HEAP_H

#include <vector>
#include <stdexcept>
#include <algorithm>
#include "PairingHeap.h" 
#include "MemoryPool.h"

// Reuse PairingNode from PairingHeap.h since it's just a struct
// or re-define if we want to be independent.
// Re-using is fine if T is the same.

template<typename T>
class PooledPairingHeap {
private:
    PairingNode<T>* root;
    size_t heapSize;
    MemoryPool<PairingNode<T>> pool;
    
    // Helper function to merge two pairing heaps
    PairingNode<T>* merge(PairingNode<T>* h1, PairingNode<T>* h2) {
        if (h1 == nullptr) return h2;
        if (h2 == nullptr) return h1;
        
        if (h2->value < h1->value) {
            std::swap(h1, h2);
        }
        
        h2->nextSibling = h1->leftChild;
        if (h1->leftChild != nullptr) {
            h1->leftChild->prev = h2;
        }
        h1->leftChild = h2;
        h2->prev = h1;

        return h1;
    }
    
    // PairingNode<T>* mergePairs(PairingNode<T>* firstSibling) {
    //     if (firstSibling == nullptr || firstSibling->nextSibling == nullptr) {
    //         return firstSibling;
    //     }
        
    //     std::vector<PairingNode<T>*> siblings;
    //     while (firstSibling != nullptr) {
    //         siblings.push_back(firstSibling);
    //         firstSibling->parent = nullptr;
    //         PairingNode<T>* next = firstSibling->nextSibling;
    //         firstSibling->nextSibling = nullptr;
    //         firstSibling->prevSibling = nullptr;
    //         firstSibling = next;
    //     }
        
    //     size_t i = 0;
    //     while (i + 1 < siblings.size()) {
    //         siblings[i] = merge(siblings[i], siblings[i + 1]);
    //         i += 2;
    //     }
        
    //     size_t start = (siblings.size() % 2 == 0) ? siblings.size() - 2 : siblings.size() - 1;
        
    //     for (int j = start - 2; j >= 0; j -= 2) {
    //         siblings[start] = merge(siblings[j], siblings[start]);
    //     }
        
    //     return siblings[start];
    // }

    PairingNode<T>* mergePairs(PairingNode<T>* firstSibling) {
        if (firstSibling == nullptr) {
            return nullptr;
        }
        
        if (firstSibling->nextSibling == nullptr) {
            return firstSibling;
        }
        
        PairingNode<T>* secondSibling = firstSibling->nextSibling;
        PairingNode<T>* rest = secondSibling->nextSibling;
        
        firstSibling->nextSibling = nullptr; 
        firstSibling->prev = nullptr;
        secondSibling->nextSibling = nullptr;
        secondSibling->prev = nullptr;

        PairingNode<T>* mergedPair = merge(firstSibling, secondSibling); // 左至右產生兩兩一組的小heap
        PairingNode<T>* mergedRest = mergePairs(rest); // 右至左遞迴合併小heaps

        return merge(mergedPair, mergedRest);
    }
    
    void deleteTree(PairingNode<T>* node) {
        if (node == nullptr) return;
        
        // Non-recursive deletion to avoid stack overflow and use pool dealloc
        std::vector<PairingNode<T>*> stack;
        stack.push_back(node);
        
        while (!stack.empty()) {
            PairingNode<T>* current = stack.back();
            stack.pop_back();
            
            if (current->leftChild) stack.push_back(current->leftChild);
            if (current->nextSibling) stack.push_back(current->nextSibling);
            
            pool.deallocate(current);
        }
    }
    
    void cut(PairingNode<T>* node) {
        if (node == nullptr || node == root) return;

        if (node->prev->leftChild == node) node->prev->leftChild = node->nextSibling;
        else node->prev->nextSibling = node->nextSibling;

        if (node->nextSibling) node->nextSibling->prev = node->prev;

        node->nextSibling = node->prev = nullptr;
    }
    
public:
    PooledPairingHeap() : root(nullptr), heapSize(0) {}
    
    ~PooledPairingHeap() {
        // Pool destructor will handle memory, but we might want to be explicit
        // Actually, MemoryPool destructor frees blocks, but doesn't call destructors of content
        // unless we individually deallocate. 
        // However, PairingNode is POD-ish (just pointers and T).
        // If T has a destructor, we MUST call it.
        // So we should traverse and deallocate.
        deleteTree(root);
    }
    
    // Disable copy
    PooledPairingHeap(const PooledPairingHeap&) = delete;
    PooledPairingHeap& operator=(const PooledPairingHeap&) = delete;
    
    PairingNode<T>* insert(const T& value) {
        PairingNode<T>* newNode = pool.allocate(value);
        root = merge(root, newNode);
        heapSize++;
        return newNode;
    }
    
    const T& findMin() const {
        if (isEmpty()) throw std::runtime_error("Heap is empty");
        return root->value;
    }
    
    void deleteMin() {
        if (isEmpty()) throw std::runtime_error("Heap is empty");
        
        PairingNode<T>* oldRoot = root;
        
        if (root->leftChild == nullptr) {
            root = nullptr;
        } else {
            root = mergePairs(root->leftChild);
        }
        
        pool.deallocate(oldRoot);
        heapSize--;
    }
    
    T extractMin() {
        T minValue = findMin();
        deleteMin();
        return minValue;
    }
    
    void decreaseKey(PairingNode<T>* node, const T& newValue) {
        if (node == nullptr) throw std::runtime_error("Invalid node");
        if (newValue > node->value) throw std::runtime_error("New value is greater");
        
        node->value = newValue;
        
        if (node != root) {
            cut(node);
            root = merge(root, node);
        }
    }
    
    bool isEmpty() const { return root == nullptr; }
    size_t size() const { return heapSize; }
    
    void clear() {
        deleteTree(root);
        root = nullptr;
        heapSize = 0;
        // pool.clear(); // If pool supported fast clear
    }
};

#endif // POOLED_PAIRING_HEAP_H
