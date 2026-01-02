// PairingHeap.tpp - Template implementation file

#include <vector>
#include <stdexcept>
#include <algorithm>

// PairingNode constructor
template<typename T>
PairingNode<T>::PairingNode(const T& val) 
    : value(val), leftChild(nullptr), nextSibling(nullptr), prevSibling(nullptr), parent(nullptr) {}

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
    h2->prevSibling = nullptr;
    if (h1->leftChild != nullptr) {
        h1->leftChild->prevSibling = h2;
    }
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
        firstSibling->prevSibling = nullptr;  // Clear prev
        firstSibling = next;
    }
    

    // First pass: merge pairs from left to right
    size_t i = 0;
    while (i + 1 < siblings.size()) {
        siblings[i] = merge(siblings[i], siblings[i + 1]);
        i += 2;
    }
    
    // Start from the last merged pair (or last single node)
    size_t start = (siblings.size() % 2 == 0) ? siblings.size() - 2 : siblings.size() - 1;
    
    // Second pass: merge from right to left
    for (int j = start - 2; j >= 0; j -= 2) {
        siblings[start] = merge(siblings[j], siblings[start]);
    }
    
    return siblings[start];
    
}

template<typename T>
void PairingHeap<T>::deleteTree(PairingNode<T>* node) {
    if (node == nullptr) return;
    
    std::vector<PairingNode<T>*> stack;
    stack.push_back(node);
    
    while (!stack.empty()) {
        PairingNode<T>* current = stack.back();
        stack.pop_back();
        
        if (current->leftChild) stack.push_back(current->leftChild);
        if (current->nextSibling) stack.push_back(current->nextSibling);
        
        delete current;
    }
}

template<typename T>
PairingNode<T>* PairingHeap<T>::cloneTree(PairingNode<T>* node, PairingNode<T>* par) {
    if (node == nullptr) return nullptr;
    
    PairingNode<T>* newNode = new PairingNode<T>(node->value);
    newNode->parent = par;
    newNode->leftChild = cloneTree(node->leftChild, newNode);
    PairingNode<T>* clonedSibling = cloneTree(node->nextSibling, par);
    newNode->nextSibling = clonedSibling;
    if (clonedSibling != nullptr) {
        clonedSibling->prevSibling = newNode;
    }
    
    return newNode;
}

template<typename T>
void PairingHeap<T>::cut(PairingNode<T>* node) {
    if (node == nullptr || node->parent == nullptr) return;  // Already root or invalid
    
    PairingNode<T>* par = node->parent;
    
    // Remove from parent's child list using doubly-linked siblings - O(1)
    if (par->leftChild == node) {
        par->leftChild = node->nextSibling;
        if (node->nextSibling != nullptr) {
            node->nextSibling->prevSibling = nullptr;
        }
    } else {
        // Use prevSibling for O(1) removal
        if (node->prevSibling != nullptr) {
            node->prevSibling->nextSibling = node->nextSibling;
        }
        if (node->nextSibling != nullptr) {
            node->nextSibling->prevSibling = node->prevSibling;
        }
    }
    
    node->parent = nullptr;
    node->nextSibling = nullptr;
    node->prevSibling = nullptr;
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
