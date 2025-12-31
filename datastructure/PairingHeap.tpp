// PairingHeap.tpp - Template implementation file

#include <vector>
#include <stdexcept>
#include <algorithm>

// PairingNode constructor
template<typename T>
PairingNode<T>::PairingNode(const T& val) 
    : value(val), leftChild(nullptr), nextSibling(nullptr), parent(nullptr) {}

// PairingHeap private methods

template<typename T>
PairingNode<T>* PairingHeap<T>::merge(PairingNode<T>* h1, PairingNode<T>* h2) {
    if (h1 == nullptr) return h2;
    if (h2 == nullptr) return h1;
    
    // Min-heap property: smaller value becomes root
    if (h2->value < h1->value) {
        std::swap(h1, h2);
    }
    
    // Make h2 a child of h1
    h2->nextSibling = h1->leftChild;
    h1->leftChild = h2;
    h2->parent = h1;  // Set parent
    
    
    return h1;
}

template<typename T>
PairingNode<T>* PairingHeap<T>::mergePairs(PairingNode<T>* firstSibling) {
    if (firstSibling == nullptr || firstSibling->nextSibling == nullptr) {
        return firstSibling;
    }
    
    // Store siblings in a vector for two-pass merge
    std::vector<PairingNode<T>*> siblings;
    while (firstSibling != nullptr) {
        siblings.push_back(firstSibling);
        firstSibling->parent = nullptr;  // Clear parent during merge
        PairingNode<T>* next = firstSibling->nextSibling;
        firstSibling->nextSibling = nullptr;  // Disconnect
        firstSibling = next;
    }
    
    // First pass
    std::vector<PairingNode<T>*> pass1;
    for (size_t i = 0; i + 1 < siblings.size(); i += 2) {
        pass1.push_back(merge(siblings[i], siblings[i + 1]));
    }
    if (siblings.size() % 2 == 1) {
        pass1.push_back(siblings.back());
    }

    // Second pass
    PairingNode<T>* result = pass1.back();
    for (int i = (int)pass1.size() - 2; i >= 0; --i) {
        result = merge(pass1[i], result);
    }
    return result;
}

template<typename T>
void PairingHeap<T>::deleteTree(PairingNode<T>* node) {
    if (node == nullptr) return;
    
    deleteTree(node->leftChild);
    deleteTree(node->nextSibling);
    delete node;
}

template<typename T>
PairingNode<T>* PairingHeap<T>::cloneTree(PairingNode<T>* node, PairingNode<T>* par) {
    if (node == nullptr) return nullptr;
    
    PairingNode<T>* newNode = new PairingNode<T>(node->value);
    newNode->parent = par;
    newNode->leftChild = cloneTree(node->leftChild, newNode);
    newNode->nextSibling = cloneTree(node->nextSibling, par);
    
    return newNode;
}

template<typename T>
void PairingHeap<T>::cut(PairingNode<T>* node) {
    if (node == nullptr || node->parent == nullptr) return;  // Already root or invalid
    
    PairingNode<T>* par = node->parent;
    
    // Remove from parent's child list
    if (par->leftChild == node) {
        par->leftChild = node->nextSibling;
    } else {
        // Find previous sibling
        PairingNode<T>* prev = par->leftChild;
        while (prev != nullptr && prev->nextSibling != node) {
            prev = prev->nextSibling;
        }
        if (prev != nullptr) {
            prev->nextSibling = node->nextSibling;
        }
    }
    
    node->parent = nullptr;
    node->nextSibling = nullptr;
}

// PairingHeap public methods

template<typename T>
PairingHeap<T>::PairingHeap() : root(nullptr), heapSize(0) {}

template<typename T>
PairingHeap<T>::~PairingHeap() {
    deleteTree(root);
}

template<typename T>
PairingNode<T>* PairingHeap<T>::insert(const T& value) {
    PairingNode<T>* newNode = new PairingNode<T>(value);
    root = merge(root, newNode);
    heapSize++;
    return newNode;
}

template<typename T>
const T& PairingHeap<T>::findMin() const {
    if (isEmpty()) {
        throw std::runtime_error("Heap is empty");
    }
    return root->value;
}

template<typename T>
void PairingHeap<T>::deleteMin() {
    if (isEmpty()) {
        throw std::runtime_error("Heap is empty");
    }
    
    PairingNode<T>* oldRoot = root;
    
    // Merge all children
    if (root->leftChild == nullptr) {
        root = nullptr;
    } else {
        root = mergePairs(root->leftChild);
    }
    
    delete oldRoot;
    heapSize--;
}

template<typename T>
T PairingHeap<T>::extractMin() {
    T minValue = findMin();
    deleteMin();
    return minValue;
}

template<typename T>
void PairingHeap<T>::decreaseKey(PairingNode<T>* node, const T& newValue) {
    if (node == nullptr) {
        throw std::runtime_error("Invalid node");
    }
    
    if (newValue > node->value) {
        throw std::runtime_error("New value is greater than current value");
    }
    
    node->value = newValue;
    
    // If not root, cut from parent and merge with root
    if (node != root) {
        cut(node);
        root = merge(root, node);
    }
}

template<typename T>
bool PairingHeap<T>::isEmpty() const {
    return root == nullptr;
}

template<typename T>
size_t PairingHeap<T>::size() const {
    return heapSize;
}

template<typename T>
void PairingHeap<T>::merge(PairingHeap& other) {
    root = merge(root, other.root);
    heapSize += other.heapSize;
    other.root = nullptr;
    other.heapSize = 0;
}

template<typename T>
void PairingHeap<T>::clear() {
    deleteTree(root);
    root = nullptr;
    heapSize = 0;
}
