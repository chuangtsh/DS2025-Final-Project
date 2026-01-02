#ifndef FIBONACCIHEAP_H
#define FIBONACCIHEAP_H

#include <vector>
#include <cmath>
#include <algorithm>
#include <stdexcept>

// Fibonacci Heap Node
template<typename T>
struct FibNode {
    T value;
    FibNode* parent;
    FibNode* child;
    FibNode* left;
    FibNode* right;
    int degree;
    bool marked;
    
    FibNode(const T& val) 
        : value(val), parent(nullptr), child(nullptr), 
          left(this), right(this), degree(0), marked(false) {}
};

// Fibonacci Heap Implementation
template<typename T>
class FibonacciHeap {
private:
    FibNode<T>* minNode;
    size_t heapSize;
    
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
            // Only node in list, nothing to do for neighbors
        } else {
            node->left->right = node->right;
            node->right->left = node->left;
        }
        // Always isolate the node
        node->left = node;
        node->right = node;
    }
    
    void consolidate() {
        // Use a safer upper bound for max degree. 2*log2(n) + 2 is generous.
        // For n=10^6, log2=20, so 42 is plenty.
        int maxDegree = 2 * static_cast<int>(std::log2(heapSize + 1)) + 5;
        std::vector<FibNode<T>*> degreeTable(maxDegree + 1, nullptr);
        
        // Flatten the root list for iteration
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
                
                // Make y a child of x
                removeFromList(y); // Remove y from root list (structure)
                y->left = y->right = y; // Isolate y
                
                if (x->child == nullptr) {
                    x->child = y;
                } else {
                    // Add y to x's child list
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
        
        // Rebuild the root list from scratch
        minNode = nullptr;
        for (FibNode<T>* node : degreeTable) {
            if (node != nullptr) {
                // Isolate node to cleanly add to fresh root list
                node->left = node->right = node;
                node->parent = nullptr; // Should already be null, but safe
                
                addToRootList(node);
                
                if (minNode == nullptr || node->value < minNode->value) {
                    minNode = node;
                }
            }
        }
    }
    
    void cut(FibNode<T>* x, FibNode<T>* y) {
        // Remove x from child list of y
        if (y->child == x) {
            y->child = (x->right == x) ? nullptr : x->right;
        }
        removeFromList(x);
        y->degree--;
        
        // Add x to root list
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
        // The root list is a circular doubly linked list.
        // We need to break the circle to iterate, then process children.
        
        // Push all nodes in the root list of this tree first
        FibNode<T>* start = node;
        FibNode<T>* current = start;
        do {
            stack.push_back(current);
            current = current->right;
        } while (current != start);
        
        while (!stack.empty()) {
            FibNode<T>* curr = stack.back();
            stack.pop_back();
            
            // Add children to stack
            if (curr->child != nullptr) {
                FibNode<T>* childStart = curr->child;
                FibNode<T>* child = childStart;
                do {
                    stack.push_back(child);
                    child = child->right;
                } while (child != childStart);
            }
            
            delete curr;
        }
    }
    
public:
    FibonacciHeap() : minNode(nullptr), heapSize(0) {}
    
    ~FibonacciHeap() {
        if (minNode != nullptr) {
            deleteTree(minNode);
        }
    }
    
    FibNode<T>* insert(const T& value) {
        FibNode<T>* newNode = new FibNode<T>(value);
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
        
        // Add all children to root list
        if (z->child != nullptr) {
            std::vector<FibNode<T>*> children;
            FibNode<T>* child = z->child;
            do {
                children.push_back(child);
                child = child->right;
            } while (child != z->child);
            
            for (FibNode<T>* c : children) {
                // Isolate child before adding to root list
                c->left = c->right = c;
                c->parent = nullptr;
                addToRootList(c);
            }
        }
        
        // Remove min from root list
        // Save execution state before removal because removeFromList isolates z
        bool wasOnlyNode = (z->right == z);
        FibNode<T>* nextMin = z->right;
        
        removeFromList(z);
        
        if (wasOnlyNode) {
            minNode = nullptr;
        } else {
            minNode = nextMin;
            consolidate();
        }
        
        delete z;
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
    void merge(FibonacciHeap& other) {
        if (other.minNode == nullptr) return; // Other heap is empty
        
        if (minNode == nullptr) {
            minNode = other.minNode;
            heapSize = other.heapSize;
        } else {
            // Concatenate root lists
            FibNode<T>* myRight = minNode->right;
            FibNode<T>* otherLeft = other.minNode->left;
            
            minNode->right = other.minNode;
            other.minNode->left = minNode;
            
            myRight->left = otherLeft;
            otherLeft->right = myRight;
            
            if (other.minNode->value < minNode->value) {
                minNode = other.minNode;
            }
            
            heapSize += other.heapSize;
        }
        
        other.minNode = nullptr;
        other.heapSize = 0;
    }
};

#endif // FIBONACCIHEAP_H
