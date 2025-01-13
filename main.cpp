#include <algorithm>
#include <array>
#include <fstream>
#include <iostream>
#include <limits>
#include <map>
#include <utility>

std::ofstream fout("doc/out.txt");
std::ifstream fin("doc/in.txt");

const int NMAX = 128;

struct Node {
  int dimension, key;
  Node *parent;
  Node *left, *right;
  Node *child;
};

class TwoThreeHeap {
public:
  void merge(Node *treeList);
  int extractMin();
  void mergeWith(TwoThreeHeap &other);
  void printHeaps(bool show);

private:
  static const int numbMaxTrees = 30;
  int n;
  std::array<Node *, numbMaxTrees> heaps;

  void addChild(Node *parent, Node *child);
  void replaceNode(Node *oldNode, Node *newNode);
  void mergeTrunks(Node **a, Node **b);
};

void TwoThreeHeap::addChild(Node *parent, Node *child) {
  // Adds a node, with it's tree to another node
  if (!parent || !child)
    return;

  // Handle neighbours
  Node *highest = parent->child;

  if (highest != nullptr) {
    Node *lower = highest->right;
    child->left = highest;
    child->right = lower;
    lower->left = child;
    highest->right = child;
  } else {
    child->left = child->right = child;
  }

  parent->child = child;
  child->parent = parent;
}

void TwoThreeHeap::replaceNode(Node *oldNode, Node *newNode) {
  // Replace the old node with the new node while also changing the trees

  // Handle neighbours
  Node *left = oldNode->left;
  Node *right = oldNode->right;

  if (right == oldNode) {
    newNode->right = newNode->left = newNode;
  } else {
    left->right = newNode;
    right->left = newNode;
    newNode->left = left;
    newNode->right = right;
  }

  // Handle parent
  Node *parent = oldNode->parent;
  newNode->parent = parent;
  if (parent->child == oldNode) {
    parent->child = newNode;
  }
}

void TwoThreeHeap::mergeTrunks(Node **a, Node **b) {
  // Merges two trees (or trunks) with the same dimension while maintaining the
  // heap property.

  // Ensure the tree with the smaller root key is the parent
  Node *tree = nullptr;
  Node *son = nullptr;
  if ((*a)->key <= (*b)->key) {
    tree = (*a);
    son = (*b);
  } else {
    tree = (*b);
    son = (*a);
  }

  // Identify the Next Children (Trunk Nodes)
  Node *nextTree = tree->child;
  Node *nextSon = son->child;

  // If their dimensions don't match the dimension of the current merging trees,
  // they are ignored (set to nullptr).
  if (nextTree && nextTree->dimension != son->dimension) {
    nextTree = nullptr;
  }

  if (nextSon && nextSon->dimension != son->dimension) {
    nextSon = nullptr;
  }

  if (nextTree == nullptr) {
    // The tree has no valid child trunk
    addChild(tree, son);

    if (nextSon) {
      // Next child
      tree->dimension += 1;
      *a = nullptr;
      *b = tree;
    } else {
      *a = tree;
      *b = nullptr;
    }
  } else if (nextSon == nullptr) {
    // The tree has two nodes in the trunk and the son has only one
    if (nextTree->key <= son->key) {
      // Make son child of nextTree
      addChild(nextTree, son);
    } else {
      // Make nextTree child of son
      replaceNode(nextTree, son);
      addChild(son, nextTree);
    }

    tree->dimension += 1;
    *a = nullptr;
    *b = tree;

  } else {
    // Both of them have two nodes
    replaceNode(nextTree, son);
    // Isolate nextTree
    nextTree->left = nextTree->right = nextTree;
    tree->dimension += 1;
    *a = nextTree;
    *b = tree;
  }
}

void TwoThreeHeap::merge(Node *treeList) {
  // We go through all the trees and we multiply them. If we get a tree with a
  // bigger dimension we use it as carry for next position while keeping heap
  // property

  Node *carry = nullptr;  // Temporarily store a tree when dimensions clash
  Node *added = treeList; // First tree in the treeList

  do {
    Node *next = nullptr;
    if (added != nullptr) {
      // Isolate the node from the list
      next = added->right;
      added->right = added->left = added;
      added->parent = nullptr;
    } else {
      // Move carry into added if we have no more nodes in treeList
      added = carry;
      carry = nullptr;
    }

    if (carry != nullptr) {
      // Merge same dimension trees
      mergeTrunks(&added, &carry);
    }

    // Inserting into heap array
    if (added) {
      int dimension = added->dimension;
      if (this->heaps[dimension] != nullptr) {
        // Merge trees of the same dimension
        mergeTrunks(&(this->heaps[dimension]), &added);
      } else {
        this->heaps[dimension] = added;
        added = nullptr;
        this->n += (1 << dimension); // effectively compute 2^d
      }
    }

    carry = added;
    added = next;
  } while (added != nullptr || carry != nullptr);
}

int TwoThreeHeap::extractMin() {
  int minDimension = -1;
  int minValue = std::numeric_limits<int>::max();
  Node *minNode = nullptr;

  // Find the smallest root
  for (int d = 0; d < numbMaxTrees; ++d) {
    if (heaps[d] && heaps[d]->key < minValue) {
      minValue = heaps[d]->key;
      minDimension = d;
      minNode = heaps[d];
    }
  }

  // Remove the tree from heaps
  heaps[minDimension] = nullptr;
  n -= (1 << minDimension);

  // Reintegrate children into the heap
  Node *child = minNode->child;
  if (child) {
    Node *start = child;
    do {
      Node *next = child->right;
      child->parent = nullptr;
      child->left = child->right = nullptr; // Isolate the child
      this->merge(child);
      child = next;
    } while (child != start);
  }

  int result = minNode->key;
  delete minNode;

  return result;
}

void TwoThreeHeap::mergeWith(TwoThreeHeap &other) {
  // Merge all the trees from 'other' into 'this'
  for (int d = 0; d < numbMaxTrees; ++d) {
    if (other.heaps[d] != nullptr) {
      Node *tree = other.heaps[d];
      tree->parent = nullptr;
      tree->left = tree->right = nullptr;

      this->merge(tree);

      other.heaps[d] = nullptr;
    }
  }
  other.n = 0;
}

void printTree(Node *node, int depth = 0) {
  if (!node)
    return;

  for (int i = 0; i < depth; ++i) {
    std::cout << "  ";
  }

  std::cout << "Node(key=" << node->key << ", dim=" << node->dimension << ")";
  if (node->child) {
    std::cout << " -> [child=" << node->child->key
              << " with dim= " << node->child->dimension << "]";
  }
  std::cout << std::endl;

  Node *child = node->child;
  if (child) {
    Node *start = child;
    do {
      printTree(child, depth + 1);
      child = child->right;
    } while (child != start);
  }
}

void TwoThreeHeap::printHeaps(bool show) {
  if (show) {
    std::cout << "TwoThreeHeap Contents:" << std::endl;
    for (int i = 0; i < numbMaxTrees; ++i) {
      if (heaps[i]) {
        std::cout << "Tree at dimension " << i << ":" << std::endl;
        printTree(heaps[i]);
      }
    }
  }
}

TwoThreeHeap heaps[NMAX];
std::map<int, std::pair<Node *, int>> nodeOrder;
int nodeCounter = 0;

void init(int N) {
  for (int i = 0; i < N; i++) {
    heaps[i] = TwoThreeHeap();
  }
}

void add(int i, int x) {
  Node *newNode = new Node();
  newNode->key = x;
  newNode->dimension = 0;
  newNode->child = nullptr;
  newNode->left = newNode->right = nullptr;
  newNode->parent = nullptr;

  heaps[i].merge(newNode);
  nodeOrder[nodeCounter++] = {newNode, i};

  heaps[i].printHeaps(false);
}

int getMin(int i) {
  int result = heaps[i].extractMin();
  fout << result << '\n';

  heaps[i].printHeaps(false);

  return result;
}

void decreaseKey(int i, int x) {
  if (nodeOrder.find(i) == nodeOrder.end()) {
    return;
  }

  Node *targetNode = nodeOrder[i].first;

  targetNode->key -= x;

  while (targetNode->parent != nullptr &&
         targetNode->key < targetNode->parent->key) {
    auto itTarget =
        std::find_if(nodeOrder.begin(), nodeOrder.end(), [&](const auto &p) {
          return p.second.first == targetNode;
        });
    auto itParent =
        std::find_if(nodeOrder.begin(), nodeOrder.end(), [&](const auto &p) {
          return p.second.first == targetNode->parent;
        });

    std::swap(targetNode->key, targetNode->parent->key);

    if (itTarget != nodeOrder.end() && itParent != nodeOrder.end()) {
      std::swap(itTarget->second.first, itParent->second.first);
    }

    targetNode = targetNode->parent;
  }
}

void mergeSets(int i, int j) {
  heaps[i].mergeWith(heaps[j]);
  heaps[i].printHeaps(false);
}

int main() {
  std::ios_base::sync_with_stdio(false);
  std::cin.tie(0);

  int N, _, op, i, j, x;

  // std::cin >> N >> _;
  fin >> N >> _;

  init(N);

  do {
    // std::cin >> op >> i;
    fin >> op >> i;
    switch (op) {
    case 1:
      // std::cin >> x;
      fin >> x;
      add(i, x);
      break;
    case 2:
      std::cout << getMin(i) << '\n';
      break;
    case 3:
      // std::cin >> x;
      fin >> x;
      decreaseKey(i, x);
      break;
    case 4:
      // std::cin >> j;
      fin >> j;
      mergeSets(i, j);
      break;
    }
  } while (_ -= 1);

  return 0;
}
