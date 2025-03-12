#include <algorithm>
#include <array>
#include <iostream>
#include <utility>

const int NMAX = 128, QMAX = 1000000;

struct Node {
  int dimension, key;
  int index;
  Node *parent;
  Node *left, *right;
  Node *child;

  Node(int key = 0, int index = 1e9)
      : dimension(0), key(key), index(index), parent(nullptr), left(nullptr),
        right(nullptr), child(nullptr) {}

  bool operator<(const Node &other) const {
    return key < other.key || (key == other.key && index < other.index);
  }
};

class TwoThreeHeap {
public:
  void merge(Node *treeList);
  int extractMin();
  void mergeWith(TwoThreeHeap &other);
  void printHeap(bool show);

  TwoThreeHeap() : n(0), heaps() {}

private:
  static const int numbMaxTrees = 30;
  int n;
  std::array<Node *, numbMaxTrees> heaps;

  void addChild(Node *parent, Node *child);
  void replaceNode(Node *oldNode, Node *newNode);
  void mergeTrunks(Node **a, Node **b);
};

void TwoThreeHeap::addChild(Node *parent, Node *child) {
  /* Adds a node, with it's tree to another node. */

  if (!parent || !child)
    return;

  // Handle parent children
  if (parent->child) {
    // If the parent already has children, insert the new child into the
    // circular doubly linked list.
    child->left = parent->child;
    child->right = parent->child->right;
    Node *l = parent->child, *r = parent->child->right;
    r->left = l->right = child;
  } else {
    // If the parent has no children, make the child point to itself
    child->left = child->right = child;
  }

  // Maintains parent-child relationship
  child->parent = parent;
  parent->child = child;
}

void TwoThreeHeap::replaceNode(Node *oldNode, Node *newNode) {
  /* Replaces the old node with the new node while maintaining tree structure.
   */

  // Handle neighbours
  if (oldNode->left == oldNode) {
    // Old node was isolated
    newNode->left = newNode->right = newNode;
  } else {
    // Old node was not isolated
    newNode->left = oldNode->left;
    newNode->right = oldNode->right;
    oldNode->left->right = oldNode->right->left = newNode;
  }

  // Handle parent
  newNode->parent = oldNode->parent;
  // Primary child case
  if (oldNode->parent && oldNode->parent->child == oldNode) {
    oldNode->parent->child = newNode;
  }

  // Detach old node
  oldNode->parent = nullptr;
  oldNode->left = oldNode->right = oldNode;
}

void TwoThreeHeap::mergeTrunks(Node **a, Node **b) {
  /* Merged two trees (or trunks) with the same dimension while maintaining the
   * heap property. */

  Node *parent = nullptr, *son = nullptr;

  // Ensures the tree with the smaller root key becomes the parent
  if (**a < **b) {
    parent = *a;
    son = *b;
  } else {
    parent = *b;
    son = *a;
  }

  // Identify the child trunks (Trunk Nodes)
  Node *parentSon = parent->child, *sonSon = son->child;

  // If their dimensons don't match the dimensions of the current merging trees,
  // they are ignored (set to nullptr). Only matching dimensions can be merged.
  if (parentSon && parentSon->dimension != son->dimension)
    parentSon = nullptr;
  if (sonSon && sonSon->dimension != son->dimension)
    sonSon = nullptr;

  // Merge cases
  if (parentSon && sonSon) {
    // Both trees have children of the same dimension, so we merge them
    replaceNode(parentSon, son);
    parentSon->left = parentSon->right = parentSon;
    ++parent->dimension;
    // Carry the merged subtree to the next iteration
    *a = parentSon;
    *b = parent;
  } else if (parentSon) {
    // Only the parent has a child trunk
    if (parentSon->operator<(*son)) {
      // If the parent's child has a smaller key, attach son as its child
      addChild(parentSon, son);
    } else {
      // Otherwise, replace and swap structure
      replaceNode(parentSon, son);
      addChild(son, parentSon);
    }
    ++parent->dimension;
    *a = nullptr;
    *b = parent;
  } else if (sonSon) {
    // Only the son has a child trunk, attach it under the parent
    addChild(parent, son);
    ++parent->dimension;
    *a = nullptr;
    *b = parent;
  } else {
    // Neither has a child trunk
    addChild(parent, son);
    *a = parent;
    *b = nullptr;
  }
}

void TwoThreeHeap::merge(Node *treeList) {
  /* Integrates a tree (or list of trees) into the heap structure.
   * Follows the base-3 addition principle: merging trees of the same dimension
   * recursively. If a merge results in a larger tree, it is carried over to the
   * next dimension */

  Node *add = treeList;

  do {
    if (heaps[add->dimension] == nullptr) {
      // If there is no existing tree in this dimension, insert it directly
      heaps[add->dimension] = add;
      n |= 1 << (add->dimension);
      add = nullptr;
    } else {
      // Otherwise, merge with the existing tree at this dimension
      int dim = add->dimension;
      mergeTrunks(&heaps[dim], &add);

      // If after merging, the heap at this dimension is cleared, update n
      if (!heaps[dim]) {
        n ^= 1 << dim;
      }
    }
  } while (add);
}

void TwoThreeHeap::mergeWith(TwoThreeHeap &other) {
  /* Merges another TwoThreeHeap into the current heap.
   * Transfers all trees from `other` into `this`, merging them as necessary.
   * Clears the `other` heap after merging. */

  for (int i = 0; (1 << i) <= other.n; ++i) {
    if (other.heaps[i]) {
      // Disconnect the tree
      other.heaps[i]->left = other.heaps[i]->right = other.heaps[i]->parent =
          nullptr;

      merge(other.heaps[i]);

      other.heaps[i] = nullptr;
    }
  }
  other.n = 0;
}

Node *nodeOrder[QMAX];
int nodeCounter = 0;

int TwoThreeHeap::extractMin() {
  /* Extracts the minimum key from the heap while maintaining heap properties.
   * Finds the root with the minimum value, removes it, and reinserts its
   * children. */

  Node *best = nullptr;
  int minPos;

  // Find the smallest root in the heap array
  for (int i = 0; (1 << i) <= n; ++i) {
    if (heaps[i]) {
      if (!best || heaps[i]->operator<(*best)) {
        minPos = i;
        best = heaps[i];
      }
    }
  }

  // Remove the minimum tree from the heap tracking structure
  n ^= 1 << minPos;
  heaps[minPos] = nullptr;

  int answer = best->key;
  // Reintegrate the children of the removed node back into the heap
  while (best->child) {
    Node *child = best->child;
    Node *l = child->left, *r = child->right;
    child->right->left = l;
    child->left->right = r;

    // Update best child
    if (best->child == l) {
      // No more children left
      best->child = nullptr;
    } else {
      // Next child
      best->child = l;
    }

    // Isolate before merge
    child->left = child->right = child->parent = nullptr;

    merge(child);
  }

  delete best;

  return answer;
}

void printTree(Node *node, int depth = 0) {
  /* Recursively prints the structure of the heap tree with indentation
   * to show hierarchy. */

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

void TwoThreeHeap::printHeap(bool show) {
  /* Prints all trees in the heap structure. */

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

void add(int i, int x, bool visualize = false) {
  Node *newNode = new Node(x, nodeCounter);

  nodeOrder[nodeCounter++] = newNode;
  heaps[i].merge(newNode);

  heaps[i].printHeap(visualize);
}

int getMin(int i, bool visualize = false) {
  int result = heaps[i].extractMin();

  heaps[i].printHeap(visualize);

  return result;
}

void decreaseKey(int i, int x) {
  Node *targetNode = nodeOrder[i];

  targetNode->key -= x;

  while (targetNode->parent && targetNode->operator<(*(targetNode->parent))) {
    std::swap(targetNode->key, targetNode->parent->key);
    std::swap(targetNode->index, targetNode->parent->index);
    std::swap(nodeOrder[targetNode->index],
              nodeOrder[targetNode->parent->index]);
    targetNode = targetNode->parent;
  }
}

void mergeSets(int i, int j, bool visualize = false) {
  heaps[i].mergeWith(heaps[j]);

  heaps[i].printHeap(visualize);
}

int main() {
  std::ios_base::sync_with_stdio(false);
  std::cin.tie(0);

  int N, _, op, i, j, x;

  std::cin >> N >> _;

  do {
    std::cin >> op >> i;
    switch (op) {
    case 1:
      std::cin >> x;
      add(i, x);
      break;
    case 2:
      std::cout << getMin(i) << '\n';
      break;
    case 3:
      std::cin >> x;
      decreaseKey(i, x);
      break;
    case 4:
      std::cin >> j;
      mergeSets(i, j);
      break;
    }
  } while (_ -= 1);

  return 0;
}
