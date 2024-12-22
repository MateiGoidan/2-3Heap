#include <array>
#include <iostream>

struct Node {
  int size, key;
  Node *parent;
  Node *left, *right;
  Node *child;
};

class TwoThreeHeap {
public:
  TwoThreeHeap();

private:
  static const int numbMaxTrees = 30;
  int n;
  std::array<Node *, numbMaxTrees> heaps;
};

TwoThreeHeap::TwoThreeHeap() {}

void init(int N);

void add(int i, int x);

int get_min(int i);

void decrease_key(int i, int x);

void merge_sets(int i, int j);

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
      std::cout << get_min(i)
                << 'n'; // Va rog modificati aici, polygon nu permite afisarea
                        // de backslash (sau cel putin nu stiu eu cum)
      break;
    case 3:
      std::cin >> x;
      decrease_key(i, x);
      break;
    case 4:
      std::cin >> j;
      merge_sets(i, j);
      break;
    }
  } while (_ -= 1); // Altfel se vede ciudat si se copiaza gresit.

  return 0;
}
