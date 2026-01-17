# GraphLib – Splay Tree (`splay_tree.h`)

This document explains the `SplayTree` data structure:

- Self-adjusting binary search tree.
- Amortized logarithmic operations.
- Supports range-sum, split, and join.

---

## 1. Overview

Header:

```cpp
#include <graphlib/splay_tree.h>
```

Main types:

- `graphlib::SplayTree`.
- `graphlib::SplayNode` (internal nodes).

---

## 2. Basic Operations

### 2.1 Construction

```cpp
graphlib::SplayTree tree;
```

Creates an empty splay tree.

### 2.2 Insert, Search, Remove

```cpp
tree.insert(5);
tree.insert(3);
tree.insert(7);

bool found = tree.search(5);
tree.remove(3);
```

- `void insert(int key)`:
  - Inserts `key` into the tree if not already present.
- `bool search(int key)`:
  - Returns `true` if `key` is present.
  - Splays the node to the root on success.
- `void remove(int key)`:
  - Removes `key` if it exists.

Properties:

- Recently accessed keys stay near the root.
- Average performance is good when access patterns exhibit locality.

---

## 3. Range Sum

```cpp
int sum = tree.range_sum(3, 7);
```

- `int range_sum(int left, int right)`:
  - Returns the sum of all keys `k` with `left <= k <= right`.

Use this for:

- Small to medium-sized data structures where you want a simple ordered set with aggregate queries.

---

## 4. Split and Join

### 4.1 Split

```cpp
graphlib::SplayTree left;
graphlib::SplayTree right;

tree.split(5, left, right);
```

- `void split(int key, SplayTree& left, SplayTree& right)`:
  - Splits the tree into:
    - `left`: all keys `<= key`.
    - `right`: all keys `> key`.

### 4.2 Join

```cpp
graphlib::SplayTree::join(left, right);
```

- `static void join(SplayTree& left, SplayTree& right)`:
  - Joins two trees assuming all keys in `left` are less than those in `right`.

Use cases:

- Building more complex data structures.
- Efficiently handling batched inserts or range splits.

