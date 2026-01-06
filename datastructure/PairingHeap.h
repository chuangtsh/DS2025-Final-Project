#ifndef PAIRING_HEAP_H
#define PAIRING_HEAP_H

#include <vector>
#include <stdexcept>
#include <algorithm>

// Pairing Heap Node
template<typename T>
struct PairingNode {
    T value;
    PairingNode* leftChild;
    PairingNode* nextSibling;
    PairingNode* prevSibling; // For O(1) cut operation in decrease-key
    PairingNode* parent;
    
    explicit PairingNode(const T& val);
};

// Pairing Heap Class with decrease-key support
template<typename T>
class PairingHeap {
private:
    PairingNode<T>* root;
    size_t heapSize;
    
    // Helper function to merge two pairing heaps
    PairingNode<T>* merge(PairingNode<T>* h1, PairingNode<T>* h2);
    
    // Two-pass merging of all siblings
    PairingNode<T>* mergePairs(PairingNode<T>* firstSibling);
    
    // Helper function to recursively delete all nodes
    void deleteTree(PairingNode<T>* node);
    
    // Helper function to clone a tree
    PairingNode<T>* cloneTree(PairingNode<T>* node, PairingNode<T>* par = nullptr);
    
    // Cut a node from its parent (for decrease-key)
    void cut(PairingNode<T>* node);
    
public:
    // Constructor
    PairingHeap();
    
    // Destructor
    ~PairingHeap();
    
    // Delete copy constructor and assignment operator
    PairingHeap(const PairingHeap&) = delete;
    PairingHeap& operator=(const PairingHeap&) = delete;
    
    // Insert a new element and return a handle to the node
    PairingNode<T>* insert(const T& value);
    
    // Find minimum element (peek)
    const T& findMin() const;
    
    // Delete minimum element
    void deleteMin();
    
    // Extract and return minimum element
    T extractMin();
    
    // Decrease the key of a node (critical for Dijkstra's algorithm)
    void decreaseKey(PairingNode<T>* node, const T& newValue);
    
    // Check if heap is empty
    bool isEmpty() const;
    
    // Get size of heap
    size_t size() const;
    
    // Merge with another pairing heap
    void merge(PairingHeap& other);
    
    // Clear the heap
    void clear();

    // Delete arbitrary node
    void deleteNode(PairingNode<T>* node);

    void dumpDOTRecursive(PairingNode<T>* node) {
        if (!node) return;
        std::cout << "  node" << node << " [label=\"" << node->value << "\"];\n";
        if (node->leftChild) {
            std::cout << "  node" << node << " -> node" << node->leftChild 
                    << " [label=\"child\"];\n";
            dumpDOTRecursive(node->leftChild);
        }
        if (node->nextSibling) {
            std::cout << "  node" << node << " -> node" << node->nextSibling 
                    << " [label=\"sibling\", color=red, style=dashed, constraint=false];\n";
            
            std::cout << "  { rank=same; node" << node << "; node" << node->nextSibling << "; }\n";

            dumpDOTRecursive(node->nextSibling);
        }
    }

    void printVisualization() {
        std::cout << "digraph PairingHeap {\n";
        std::cout << "  node [shape=circle];\n";
        if (root != nullptr) {
            dumpDOTRecursive(root);
        }
        std::cout << "}\n";
    }
};

// Include template implementation
#include "PairingHeap.tpp"

#endif // PAIRING_HEAP_H
