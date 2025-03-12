# Two-Three Heap Data Structure

### **Table of content**
- [Problem Statement](#problem-statement)
- [Implementation Details](#implementation-details)
    - [Insertion](#insertion-(`-1-i-x`))
    - [Extract Minimum](#extract-minimum-(`2-i`)) 
    - [Decrease Key](#decrease-key-(`3-i-x`))
    - [Merge](#merge-(`4-i-j`))
- [Performance Complexity](#performance-complexity) 
- [Usage Instructions](#usage-instructions) 

## Problem Statement

This project implements a **Two-Three Heap** data structure that supports efficient operations on multiple integer sets. The structure allows performing the following operations:

### Operations
- **`1 i x`** – Inserts the element `x` into the sub-multiset `i` (duplicates are allowed).
- **`2 i`** – Extracts and prints the smallest element in sub-multiset `i`.  
  - If multiple elements have the same minimum value, the one inserted first is removed.
- **`3 i x`** – Decreases the value of the `i`-th inserted element by `x`.
- **`4 i j`** – Merges all elements from sub-multiset `i` into sub-multiset `j`.  
  - After this operation, `i` becomes empty.

### Constraints
- **Initial Conditions:** There are `N` sub-multisets, all initially empty.
- **Guaranteed Conditions:**
  - There is at least one element in sub-multiset `i` for operation `2 i`.
  - There is no underflow when performing operation `3 i x`.

---

## Implementation Details
The Two-Three Heap is a **mergeable heap** optimized for operations that require frequent merging. The key components of the implementation are:

### Insertion (`1 i x`)
- Each new node is wrapped inside a **tree** of dimension `0` and added to the heap.
- If the heap already contains a tree of the same dimension, merging occurs.
- The **heap property** ensures that the minimum value is always accessible.

### Extract Minimum (`2 i`)
- The **smallest root** across all trees in `heaps[i]` is selected and removed.
- If multiple roots have the same value, the one with the smallest insertion order is chosen.
- The removed node's children are **reintegrated** into the heap.

### Decrease Key (`3 i x`)
- The value of a specific node is decreased by `x`.
- If the node violates the heap property (i.e., it becomes smaller than its parent), **a swap is performed** to restore the property.
- This propagates up the tree until the heap order is restored.

### Merge (`4 i j`)
- Trees from `heaps[i]` are merged into `heaps[j]` while maintaining the heap structure.
- The **binomial heap** representation ensures efficient merging.

---

## Performance Complexity
| Operation      | Complexity |
|---------------|------------|
| Insert (`1 i x`) | **O(log N)** |
| Extract Min (`2 i`) | **O(log N)** |
| Decrease Key (`3 i x`) | **O(log N)** |
| Merge (`4 i j`) | **O(log N)** |

---

## Usage Instructions

### Compilation
Compile the C++ source file using `g++`:
```sh
g++ -O2 -std=c++17 main.cpp -o heap
```

### Running the Program
```sh
./heap < input.txt
```

### Input Format
```sh
N Q
<operation> <arguments>
...
```
