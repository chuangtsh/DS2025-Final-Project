#include <iostream>
#include <vector>

struct Node {
    int key;
    Node *child, *sibling, *prev; // prev 指向父節點或左兄弟

    Node(int k) : key(k), child(nullptr), sibling(nullptr), prev(nullptr) {}
};

class PairingHeap {
private:
    Node* root;

    // 核心操作：合併兩個堆積
    Node* merge(Node* A, Node* B) {
        if (!A) return B;
        if (!B) return A;
        if (A->key < B->key) {
            B->sibling = A->child;
            if (A->child) A->child->prev = B;
            A->child = B;
            B->prev = A;
            return A;
        } else {
            A->sibling = B->child;
            if (B->child) B->child->prev = A;
            B->child = A;
            A->prev = B;
            return B;
        }
    }

    // 用於 extractMin 的兩階段配對合併
    Node* combineSiblings(Node* first) {
        if (!first || !first->sibling) return first;
        Node* n1 = first;
        Node* n2 = n1->sibling;
        Node* nextIter = n2->sibling;

        n1->sibling = n2->sibling = n1->prev = n2->prev = nullptr;
        return merge(merge(n1, n2), combineSiblings(nextIter));
    }

public:
    PairingHeap() : root(nullptr) {}

    Node* insert(int key) {
        Node* newNode = new Node(key);
        root = merge(root, newNode);
        return newNode;
    }

    void decreaseKey(Node* node, int newVal) {
        if (newVal > node->key) return; // 僅限減少
        node->key = newVal;
        if (node == root) return;

        // 將節點從兄弟鏈中切斷
        if (node->prev->child == node) node->prev->child = node->sibling;
        else node->prev->sibling = node->sibling;
        
        if (node->sibling) node->sibling->prev = node->prev;

        node->sibling = node->prev = nullptr;
        root = merge(root, node); // 重新與根合併
    }

    int extractMin() {
        int minVal = root->key;
        Node* oldRoot = root;
        root = combineSiblings(root->child);
        delete oldRoot;
        return minVal;
    }
};

int main() {
    PairingHeap ph;
    auto n1 = ph.insert(20);
    auto n2 = ph.insert(10);
    auto n3 = ph.insert(30);
    
    std::cout << "初始最小: " << ph.extractMin() << std::endl; // 應輸出 10
    
    ph.decreaseKey(n3, 5);
    std::cout << "DecreaseKey(30->5) 後最小: " << ph.extractMin() << std::endl; // 應輸出 5
    
    return 0;
}
