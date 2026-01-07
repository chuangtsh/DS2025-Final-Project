// PairingHeap.tpp - Template implementation file

#include <vector>
#include <stdexcept>
#include <algorithm>
#include <iomanip>
#include <fstream>

// PairingNode constructor
template<typename T>
PairingNode<T>::PairingNode(const T& val) 
    : value(val), leftChild(nullptr), nextSibling(nullptr), prev(nullptr) {}

// PairingHeap private methods

template<typename T>
PairingNode<T>* PairingHeap<T>::merge(PairingNode<T>* h1, PairingNode<T>* h2) {
    if (h1 == nullptr) return h2;
    if (h2 == nullptr) return h1;
    
    // set h1 to be the smaller root
    if (h2->value < h1->value) {
        std::swap(h1, h2);
    }
    
    // Make h2 a child of h1
    h2->nextSibling = h1->leftChild;
    if (h1->leftChild != nullptr) {
        h1->leftChild->prev = h2;
    }
    h1->leftChild = h2;
    h2->prev = h1;  // Set parent

    return h1;
}

template<typename T>
PairingNode<T>* PairingHeap<T>::mergePairs(PairingNode<T>* firstSibling) {
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

    // merge two siblings to form a small pair heap
    PairingNode<T>* mergedPair = merge(firstSibling, secondSibling);

    // recursively merge the rest siblings (left to right)
    PairingNode<T>* mergedRest = mergePairs(rest);

    // merge the small pair heap with the merged rest (right to left)
    return merge(mergedPair, mergedRest);
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
void PairingHeap<T>::cut(PairingNode<T>* node) {
    if (node == nullptr || node == root) return;  // Already root or invalid

    if (node->prev->leftChild == node) node->prev->leftChild = node->nextSibling;
    else node->prev->nextSibling = node->nextSibling;

    if (node->nextSibling) node->nextSibling->prev = node->prev;

    node->nextSibling = node->prev = nullptr;
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

template<typename T>
void PairingHeap<T>::deleteNode(PairingNode<T>* node) {
    if (node == nullptr) return;
    if (node != root) {
        cut(node);
        PairingNode<T>* children = node->leftChild;
        delete node;
        heapSize--;
        if (children != nullptr) {
            PairingNode<T>* newSubTree = mergePairs(children);
            root = merge(root, newSubTree);
        }
    }else{
        deleteMin();
    }
}

template<typename T>
void PairingHeap<T>::dumpDOTRecursive(PairingNode<T>* node, std::ostream& out) {
    if (!node) return;
    out << "  node" << node << " [label=\"" << node->value << "\"];\n";

    if (node->leftChild) {
        out << "  node" << node << " -> node" << node->leftChild 
            << " [label=\"child\"];\n";
        dumpDOTRecursive(node->leftChild, out);
    }

    if (node->nextSibling) {
        out << "  node" << node << " -> node" << node->nextSibling 
            << " [label=\"sibling\", color=red, style=dashed, constraint=false];\n";
        out << "  { rank=same; node" << node << "; node" << node->nextSibling << "; }\n";
        dumpDOTRecursive(node->nextSibling, out);
    }
}

template<typename T>
void PairingHeap<T>::printVisualization(const std::string& title) {
    static int fileCounter = 0;

    std::stringstream ss;
    ss << "graph_dots/heap_" << std::setw(2) << std::setfill('0') << fileCounter++ 
        << "_" << title << ".dot";
    std::string filename = ss.str();
    
    std::ofstream outFile(filename);
    if (!outFile) {
        std::cerr << "Error creating file: " << filename << std::endl;
        return;
    }

    outFile << "digraph PairingHeap {\n";
    outFile << "  label=\"" << title << "\";\n";
    outFile << "  labelloc=\"t\";\n";
    outFile << "  node [shape=circle];\n";
    
    if (root != nullptr) {
        dumpDOTRecursive(root, outFile);
    }
    outFile << "}\n";
    
    outFile.close();
    // std::cout << "Generated visualization: " << filename << std::endl << std::endl;
}