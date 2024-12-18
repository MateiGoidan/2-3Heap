#include <algorithm>
#include <climits>
#include <iostream>
#include <vector>

struct Node {
  int leftKey, rightKey;
  Node *parent;
  Node *firstChild, *secondChild, *thirdChild;

  Node(int x) {
    leftKey = x;
    rightKey = INT_MAX;
    parent = firstChild = secondChild = thirdChild = nullptr;
  }

  bool isLeaf() const { return firstChild == nullptr; }

  bool notFull() const { return rightKey == INT_MAX; }
};

class TwoThreeHeap {
public:
  TwoThreeHeap();
  void insertIntoParent(Node *parent, int promotedKey, Node *leftChild,
                        Node *rightChild);
  void splitNode(Node *node, int x);
  void inserting(int x);

private:
  Node *root;
};

TwoThreeHeap::TwoThreeHeap() : root(nullptr) {}

void TwoThreeHeap::insertIntoParent(Node *parent, int promotedKey,
                                    Node *leftChild, Node *rightChild) {
  // Parent has one key
  if (parent->notFull()) {
    if (promotedKey < parent->leftKey) {
      parent->rightKey = parent->leftKey; // Shift key to the right
      parent->leftKey = promotedKey;
      parent->thirdChild = parent->secondChild; // Shift child pointer
      parent->firstChild = leftChild;
      parent->secondChild = rightChild;
    } else {
      parent->rightKey = promotedKey;
      parent->secondChild = leftChild;
      parent->thirdChild = rightChild;
    }
    leftChild->parent = parent;
    rightChild->parent = parent;
  }
  // Parent has two keys
  else {
    // Creating the 4-th child node (not linked)
    Node *children[4] = {parent->firstChild, parent->secondChild,
                         parent->thirdChild, nullptr};
    if (promotedKey < parent->leftKey) {
      children[3] = children[2];
      children[2] = children[1];
      children[1] = rightChild;
      children[0] = leftChild;
    } else if (promotedKey < parent->rightKey) {
      children[3] = children[2];
      children[2] = rightChild;
      children[1] = leftChild;
    } else {
      children[3] = rightChild;
      children[2] = leftChild;
    }

    // Sorting parents key with the promoted key
    int keys[3] = {parent->leftKey, parent->rightKey, promotedKey};
    std::sort(keys, keys + 3);

    // Splitting the parent node
    Node *left = new Node(keys[0]);
    Node *right = new Node(keys[2]);

    // Assigning child notes
    left->firstChild = children[0];
    left->secondChild = children[1];

    right->firstChild = children[2];
    right->secondChild = children[3];

    // Assigning children's parents
    if (left->firstChild)
      left->firstChild->parent = left;
    if (left->secondChild)
      left->secondChild->parent = left;

    if (right->firstChild)
      right->firstChild->parent = right;
    if (right->secondChild)
      right->secondChild->parent = right;

    if (parent->parent) {
      insertIntoParent(parent->parent, keys[1], left, right);
    } else {
      // Creating a new root
      root = new Node(keys[1]);
      root->firstChild = left;
      root->secondChild = right;
      left->parent = root;
      right->parent = root;
    }
  }
}

void TwoThreeHeap::splitNode(Node *node, int x) {
  // Sorting the keys
  int keys[3] = {node->leftKey, node->rightKey, x};
  std::sort(keys, keys + 3);

  // Spliting the node into two
  Node *left = new Node(keys[0]);
  Node *right = new Node(keys[2]);

  // Promoting
  if (node->parent) {
    insertIntoParent(node->parent, keys[1], left, right);
  } else {
    // Creating a new root
    root = new Node(keys[1]);
    root->firstChild = left;
    root->secondChild = right;
    left->parent = root;
    right->parent = root;
  }
}

void TwoThreeHeap::inserting(int x) {
  // Empty heap exception
  if (!root) {
    root = new Node(x);
    return;
  }

  // Traversing down the tree
  Node *current = root;
  while (!current->isLeaf()) {
    if (x < current->leftKey) {
      current = current->firstChild;
    } else if (current->rightKey == INT_MAX || x < current->rightKey) {
      current = current->secondChild;
    } else {
      current = current->thirdChild;
    }
  }

  // Ignore duplicates
  if (x == current->leftKey || x == current->rightKey) {
    std::cerr << "Duplicate key detected: " << x << '\n';
    return;
  }

  // Inserting the value
  if (current->notFull()) {
    if (x < current->leftKey) {
      current->rightKey = current->leftKey;
      current->leftKey = x;
    } else {
      current->rightKey = x;
    }
  } else {
    // Spliting the node if the Node already has two keys
    splitNode(current, x);
  }
}

std::vector<TwoThreeHeap> heaps;

void init(int N) { heaps.resize(N); }

void add(int i, int x) { heaps[i].inserting(x); };

int get_min(int i) { return 0; };

void decrease_key(int i, int x) { return; };

void merge_sets(int i, int j) { return; };

int main() {
  std::ios_base::sync_with_stdio(false);
  std::cin.tie(0);

  int N, _, op, i, j, x;

  std::cin >> N >> _;

  init(N);

  do {
    std::cin >> op >> i;
    switch (op) {
    case 1:
      std::cin >> x;
      add(i, x);
      break;
    case 2:
      std::cout << get_min(i) << '\n';
      break;
    case 4:
      std::cin >> j;
      merge_sets(i, j);
      break;
    }
  } while (_ -= 1);

  return 0;
}
