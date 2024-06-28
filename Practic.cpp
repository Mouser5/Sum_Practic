#include <iostream>
#include <cmath>
#include <vector>
#include <limits>
#include <iomanip>

// Узел для фибоначчиевой кучи
struct Node {
    int key;
    int degree;
    Node* parent;
    Node* child;
    Node* left;
    Node* right;
    bool mark;

    Node(int k) : key(k), degree(0), parent(nullptr), child(nullptr), left(this), right(this), mark(false) {}
};

// Фибоначчиева куча
class FibonacciHeap {
public:
    Node* minNode;
    int nodeCount;

    FibonacciHeap() : minNode(nullptr), nodeCount(0) {}

    void insert(int key) {
        Node* node = new Node(key);
        if (minNode == nullptr) {
            minNode = node;
        }
        else {
            mergeNodes(minNode, node);
            if (key < minNode->key) {
                minNode = node;
            }
        }
        ++nodeCount;
    }

    void merge(FibonacciHeap& other) {
        if (other.minNode == nullptr) return;

        if (minNode == nullptr) {
            minNode = other.minNode;
        }
        else {
            mergeNodes(minNode, other.minNode);
            if (other.minNode->key < minNode->key) {
                minNode = other.minNode;
            }
        }
        nodeCount += other.nodeCount;

        other.minNode = nullptr;
        other.nodeCount = 0;
    }

    int extractMin() {
        if (minNode == nullptr) throw std::runtime_error("Heap is empty.");

        Node* oldMinNode = minNode;
        if (oldMinNode->child != nullptr) {
            Node* child = oldMinNode->child;
            do {
                Node* nextChild = child->right;
                mergeNodes(minNode, child);
                child->parent = nullptr;
                child = nextChild;
            } while (child != oldMinNode->child);
        }

        removeNode(oldMinNode);

        if (oldMinNode->right == oldMinNode) {
            minNode = nullptr;
        }
        else {
            minNode = oldMinNode->right;
            consolidate();
        }

        --nodeCount;
        int minKey = oldMinNode->key;
        delete oldMinNode;
        return minKey;
    }

    void printHeap() const {
        if (minNode == nullptr) {
            std::cout << "Heap is empty." << std::endl;
            return;
        }
        printNode(minNode, 0);
        std::cout << std::endl;
    }

private:
    void mergeNodes(Node* a, Node* b) {
        a->left->right = b->right;
        b->right->left = a->left;
        a->left = b;
        b->right = a;
    }

    void removeNode(Node* node) {
        if (node->left != node) {
            node->left->right = node->right;
            node->right->left = node->left;
        }
    }

    void consolidate() {
        int maxDegree = static_cast<int>(std::log2(nodeCount)) + 1;
        std::vector<Node*> degreeTable(maxDegree, nullptr);

        std::vector<Node*> nodes;
        Node* current = minNode;
        do {
            nodes.push_back(current);
            current = current->right;
        } while (current != minNode);

        for (Node* node : nodes) {
            int degree = node->degree;
            while (degreeTable[degree] != nullptr) {
                Node* other = degreeTable[degree];
                if (node->key > other->key) {
                    std::swap(node, other);
                }
                link(other, node);
                degreeTable[degree] = nullptr;
                ++degree;
            }
            degreeTable[degree] = node;
        }

        minNode = nullptr;
        for (Node* node : degreeTable) {
            if (node != nullptr) {
                if (minNode == nullptr) {
                    minNode = node;
                }
                else {
                    mergeNodes(minNode, node);
                    if (node->key < minNode->key) {
                        minNode = node;
                    }
                }
            }
        }
    }

    void link(Node* y, Node* x) {
        removeNode(y);
        y->left = y->right = y;
        if (x->child == nullptr) {
            x->child = y;
        }
        else {
            mergeNodes(x->child, y);
        }
        y->parent = x;
        ++x->degree;
        y->mark = false;
    }

    void printNode(Node* node, int depth) const {
        Node* current = node;
        do {
            std::cout << std::setw(depth * 2) << "" << current->key << " (" << current->degree << ")" << std::endl;
            if (current->child != nullptr) {
                printNode(current->child, depth + 1);
            }
            current = current->right;
        } while (current != node);
    }
};

int main() {
    FibonacciHeap heap;

    heap.insert(3);
    heap.insert(7);
    heap.insert(1);
    heap.insert(9);
    heap.insert(5);

    std::cout << "Initial heap:" << std::endl;
    heap.printHeap();

    int minElem = heap.extractMin();
    std::cout << "Extracted min: " << minElem << std::endl;
    std::cout << "Heap after extracting min:" << std::endl;
    heap.printHeap();

    FibonacciHeap otherHeap;
    otherHeap.insert(2);
    otherHeap.insert(8);
    otherHeap.insert(6);

    std::cout << "Other heap:" << std::endl;
    otherHeap.printHeap();

    heap.merge(otherHeap);

    std::cout << "Merged heap:" << std::endl;
    heap.printHeap();

    return 0;
}
