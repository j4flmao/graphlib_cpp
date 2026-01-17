#include "graphlib/splay_tree.h"

namespace graphlib {

SplayNode::SplayNode(int key_value)
    : key(key_value), left(nullptr), right(nullptr), parent(nullptr), size(1), sum(key_value) {
}

SplayTree::SplayTree()
    : root_(nullptr) {
}

SplayTree::~SplayTree() {
    destroy(root_);
}

SplayTree::SplayTree(SplayTree&& other) noexcept
    : root_(other.root_) {
    other.root_ = nullptr;
}

SplayTree& SplayTree::operator=(SplayTree&& other) noexcept {
    if (this != &other) {
        destroy(root_);
        root_ = other.root_;
        other.root_ = nullptr;
    }
    return *this;
}

void SplayTree::destroy(SplayNode* x) {
    if (!x) {
        return;
    }
    destroy(x->left);
    destroy(x->right);
    delete x;
}

void SplayTree::update(SplayNode* x) {
    if (!x) {
        return;
    }
    x->size = 1;
    x->sum = x->key;
    if (x->left) {
        x->size += x->left->size;
        x->sum += x->left->sum;
    }
    if (x->right) {
        x->size += x->right->size;
        x->sum += x->right->sum;
    }
}

void SplayTree::rotate_left(SplayNode* x) {
    SplayNode* y = x->right;
    if (!y) {
        return;
    }
    x->right = y->left;
    if (y->left) {
        y->left->parent = x;
    }
    y->parent = x->parent;
    if (!x->parent) {
        root_ = y;
    } else if (x == x->parent->left) {
        x->parent->left = y;
    } else {
        x->parent->right = y;
    }
    y->left = x;
    x->parent = y;
    update(x);
    update(y);
}

void SplayTree::rotate_right(SplayNode* x) {
    SplayNode* y = x->left;
    if (!y) {
        return;
    }
    x->left = y->right;
    if (y->right) {
        y->right->parent = x;
    }
    y->parent = x->parent;
    if (!x->parent) {
        root_ = y;
    } else if (x == x->parent->left) {
        x->parent->left = y;
    } else {
        x->parent->right = y;
    }
    y->right = x;
    x->parent = y;
    update(x);
    update(y);
}

void SplayTree::splay(SplayNode* x) {
    if (!x) {
        return;
    }
    while (x->parent) {
        SplayNode* p = x->parent;
        SplayNode* g = p->parent;
        if (!g) {
            if (x == p->left) {
                rotate_right(p);
            } else {
                rotate_left(p);
            }
        } else if (x == p->left && p == g->left) {
            rotate_right(g);
            rotate_right(p);
        } else if (x == p->right && p == g->right) {
            rotate_left(g);
            rotate_left(p);
        } else if (x == p->right && p == g->left) {
            rotate_left(p);
            rotate_right(g);
        } else {
            rotate_right(p);
            rotate_left(g);
        }
    }
}

SplayNode* SplayTree::subtree_min(SplayNode* x) {
    if (!x) {
        return nullptr;
    }
    while (x->left) {
        x = x->left;
    }
    return x;
}

SplayNode* SplayTree::find_node(int key) {
    SplayNode* cur = root_;
    SplayNode* last = nullptr;
    while (cur) {
        last = cur;
        if (key < cur->key) {
            cur = cur->left;
        } else if (key > cur->key) {
            cur = cur->right;
        } else {
            break;
        }
    }
    if (last) {
        splay(last);
    }
    if (root_ && root_->key == key) {
        return root_;
    }
    return nullptr;
}

void SplayTree::insert(int key) {
    if (!root_) {
        root_ = new SplayNode(key);
        return;
    }
    SplayNode* cur = root_;
    SplayNode* parent = nullptr;
    while (cur) {
        parent = cur;
        if (key < cur->key) {
            cur = cur->left;
        } else if (key > cur->key) {
            cur = cur->right;
        } else {
            splay(cur);
            return;
        }
    }
    SplayNode* node = new SplayNode(key);
    node->parent = parent;
    if (key < parent->key) {
        parent->left = node;
    } else {
        parent->right = node;
    }
    splay(node);
}

bool SplayTree::search(int key) {
    SplayNode* node = find_node(key);
    return node != nullptr;
}

void SplayTree::remove(int key) {
    if (!root_) {
        return;
    }
    if (!search(key)) {
        return;
    }
    SplayNode* target = root_;
    SplayNode* left = target->left;
    SplayNode* right = target->right;
    if (left) {
        left->parent = nullptr;
    }
    if (right) {
        right->parent = nullptr;
    }
    delete target;
    if (!left) {
        root_ = right;
        return;
    }
    SplayNode* max_left = left;
    while (max_left->right) {
        max_left = max_left->right;
    }
    root_ = left;
    splay(max_left);
    root_->right = right;
    if (right) {
        right->parent = root_;
    }
    update(root_);
}

void SplayTree::split(int key, SplayTree& left_tree, SplayTree& right_tree) {
    if (!root_) {
        return;
    }

    find_node(key);
    if (!root_) {
        return;
    }

    if (root_->key <= key) {
        left_tree.root_ = root_;
        right_tree.root_ = root_->right;
        if (left_tree.root_) {
            left_tree.root_->right = nullptr;
            if (right_tree.root_) {
                right_tree.root_->parent = nullptr;
            }
            left_tree.update(left_tree.root_);
        }
    } else {
        right_tree.root_ = root_;
        left_tree.root_ = root_->left;
        if (right_tree.root_) {
            right_tree.root_->left = nullptr;
            if (left_tree.root_) {
                left_tree.root_->parent = nullptr;
            }
            right_tree.update(right_tree.root_);
        }
    }
    root_ = nullptr;
}

void SplayTree::join(SplayTree& left_tree, SplayTree& right_tree) {
    if (!left_tree.root_) {
        left_tree.root_ = right_tree.root_;
        right_tree.root_ = nullptr;
        return;
    }
    if (!right_tree.root_) {
        return;
    }
    SplayNode* max_left = left_tree.root_;
    while (max_left->right) {
        max_left = max_left->right;
    }
    left_tree.splay(max_left);
    left_tree.root_->right = right_tree.root_;
    right_tree.root_->parent = left_tree.root_;
    left_tree.update(left_tree.root_);
    right_tree.root_ = nullptr;
}

int SplayTree::range_sum(int left, int right) {
    if (!root_ || left > right) {
        return 0;
    }

    SplayTree left_tree;
    SplayTree right_tree;
    split(right, left_tree, right_tree);

    SplayTree left_tree2;
    SplayTree middle_tree;
    if (left_tree.root_) {
        left_tree.split(left - 1, left_tree2, middle_tree);
    }

    long long result = 0;
    if (middle_tree.root_) {
        result = middle_tree.root_->sum;
    }

    join(left_tree2, middle_tree);
    join(left_tree2, right_tree);

    root_ = left_tree2.root_;
    left_tree2.root_ = nullptr;

    if (result < 0) {
        return 0;
    }
    if (result > static_cast<long long>(2147483647)) {
        return 2147483647;
    }
    return static_cast<int>(result);
}

}

