#include "graphlib/link_cut_tree.h"
#include <algorithm>
#include <limits>
#include <stdexcept>

namespace graphlib {

LinkCutTree::LinkCutTree(int n) {
    nodes_.resize(n);
    for (int i = 0; i < n; ++i) {
        nodes_[i].id = i;
        nodes_[i].parent = -1;
        nodes_[i].children[0] = nodes_[i].children[1] = -1;
        nodes_[i].val = 0;
        nodes_[i].sum = 0;
        nodes_[i].max_val = std::numeric_limits<long long>::min();
        nodes_[i].reverse = false;
    }
}

LinkCutTree::~LinkCutTree() = default;

bool LinkCutTree::is_root(int u) {
    int p = nodes_[u].parent;
    return p == -1 || (nodes_[p].children[0] != u && nodes_[p].children[1] != u);
}

void LinkCutTree::push(int u) {
    if (nodes_[u].reverse) {
        std::swap(nodes_[u].children[0], nodes_[u].children[1]);
        if (nodes_[u].children[0] != -1) nodes_[nodes_[u].children[0]].reverse ^= true;
        if (nodes_[u].children[1] != -1) nodes_[nodes_[u].children[1]].reverse ^= true;
        nodes_[u].reverse = false;
    }
}

void LinkCutTree::update(int u) {
    nodes_[u].sum = nodes_[u].val;
    nodes_[u].max_val = nodes_[u].val;
    if (nodes_[u].children[0] != -1) {
        nodes_[u].sum += nodes_[nodes_[u].children[0]].sum;
        nodes_[u].max_val = std::max(nodes_[u].max_val, nodes_[nodes_[u].children[0]].max_val);
    }
    if (nodes_[u].children[1] != -1) {
        nodes_[u].sum += nodes_[nodes_[u].children[1]].sum;
        nodes_[u].max_val = std::max(nodes_[u].max_val, nodes_[nodes_[u].children[1]].max_val);
    }
}

void LinkCutTree::connect(int ch, int p, int dir) {
    if (ch != -1) nodes_[ch].parent = p;
    if (p != -1) nodes_[p].children[dir] = ch;
}

void LinkCutTree::rotate(int x) {
    int p = nodes_[x].parent;
    int g = nodes_[p].parent;
    bool is_root_p = is_root(p);
    int dir = (nodes_[p].children[1] == x);
    
    connect(nodes_[x].children[dir ^ 1], p, dir);
    connect(p, x, dir ^ 1);
    
    nodes_[x].parent = g;
    if (!is_root_p) {
        nodes_[g].children[nodes_[g].children[1] == p] = x;
    }
    
    update(p);
    update(x);
}

void LinkCutTree::splay(int x) {
    // First pass: push down reverse flags from root to x
    std::vector<int> stack;
    int curr = x;
    stack.push_back(curr);
    while (!is_root(curr)) {
        curr = nodes_[curr].parent;
        stack.push_back(curr);
    }
    while (!stack.empty()) {
        push(stack.back());
        stack.pop_back();
    }
    
    while (!is_root(x)) {
        int p = nodes_[x].parent;
        if (!is_root(p)) {
            int g = nodes_[p].parent;
            if ((nodes_[p].children[1] == x) == (nodes_[g].children[1] == p)) {
                rotate(p);
            } else {
                rotate(x);
            }
        }
        rotate(x);
    }
}

void LinkCutTree::access(int u) {
    int last = -1;
    for (int v = u; v != -1; v = nodes_[v].parent) {
        splay(v);
        nodes_[v].children[1] = last;
        update(v);
        last = v;
    }
    splay(u);
}

void LinkCutTree::make_root(int u) {
    access(u);
    nodes_[u].reverse ^= true;
    push(u); // Optional here but good practice
}

int LinkCutTree::find_root(int u) {
    access(u);
    while (nodes_[u].children[0] != -1) {
        u = nodes_[u].children[0];
        push(u);
    }
    splay(u);
    return u;
}

void LinkCutTree::link(int u, int v) {
    make_root(u);
    if (find_root(v) == u) return; // Already connected
    nodes_[u].parent = v;
}

void LinkCutTree::cut(int u, int v) {
    make_root(u);
    if (find_root(v) != u || nodes_[v].parent != u || nodes_[v].children[0] != -1) {
        return; // No edge or not directly connected in the represented tree
    }
    nodes_[v].parent = -1;
    nodes_[u].children[1] = -1;
    update(u);
}

bool LinkCutTree::is_connected(int u, int v) {
    if (u == v) return true;
    make_root(u);
    return find_root(v) == u;
}

void LinkCutTree::set_value(int u, long long val) {
    make_root(u); // Bring to root of splay to update easily
    nodes_[u].val = val;
    update(u);
}

long long LinkCutTree::get_value(int u) {
    return nodes_[u].val;
}

long long LinkCutTree::query_path_sum(int u, int v) {
    make_root(u);
    access(v);
    return nodes_[v].sum;
}

long long LinkCutTree::query_path_max(int u, int v) {
    make_root(u);
    access(v);
    return nodes_[v].max_val;
}

} // namespace graphlib
