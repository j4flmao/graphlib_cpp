#ifndef GRAPHLIB_SPLAY_TREE_H
#define GRAPHLIB_SPLAY_TREE_H

#include "export.h"

namespace graphlib {

struct SplayNode {
    int key;
    SplayNode* left;
    SplayNode* right;
    SplayNode* parent;
    int size;
    long long sum;

    SplayNode(int key);
};

class GRAPHLIB_API SplayTree {
private:
    SplayNode* root_;

    void rotate_left(SplayNode* x);
    void rotate_right(SplayNode* x);
    void splay(SplayNode* x);
    void update(SplayNode* x);
    void destroy(SplayNode* x);
    SplayNode* subtree_min(SplayNode* x);
    SplayNode* find_node(int key);

public:
    SplayTree();
    ~SplayTree();

    SplayTree(const SplayTree&) = delete;
    SplayTree& operator=(const SplayTree&) = delete;
    SplayTree(SplayTree&& other) noexcept;
    SplayTree& operator=(SplayTree&& other) noexcept;

    void insert(int key);
    bool search(int key);
    void remove(int key);

    void split(int key, SplayTree& left, SplayTree& right);
    static void join(SplayTree& left, SplayTree& right);
    int range_sum(int left, int right);
};

}

#endif

