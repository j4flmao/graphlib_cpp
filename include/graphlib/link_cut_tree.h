#ifndef GRAPHLIB_LINK_CUT_TREE_H
#define GRAPHLIB_LINK_CUT_TREE_H

#include "export.h"
#include <vector>

namespace graphlib {

class GRAPHLIB_API LinkCutTree {
private:
    struct Node {
        int id;
        int children[2]; // 0: left, 1: right
        int parent;
        long long val;
        long long sum;
        long long max_val;
        bool reverse;

        Node() : id(-1), parent(-1), val(0), sum(0), max_val(0), reverse(false) {
            children[0] = children[1] = -1;
        }
    };

    std::vector<Node> nodes_;

    bool is_root(int u);
    void push(int u);
    void update(int u);
    void rotate(int u);
    void splay(int u);
    void connect(int ch, int p, int dir);

public:
    explicit LinkCutTree(int n);
    ~LinkCutTree();

    // Sets value of vertex u
    void set_value(int u, long long val);
    
    // Gets value of vertex u
    long long get_value(int u);

    // Returns sum of values on path between u and v
    long long query_path_sum(int u, int v);
    
    // Returns max value on path between u and v
    long long query_path_max(int u, int v);

    // Adds an edge between u and v (making u a child of v if u is root of its tree)
    // Precondition: u and v are in different trees, and u is root of its tree (via make_root)
    void link(int u, int v);

    // Removes edge between u and v
    void cut(int u, int v);

    // Checks if u and v are in the same tree
    bool is_connected(int u, int v);

    // Finds the root of the tree containing u
    int find_root(int u);

    // Makes u the root of its tree (reverses path from u to original root)
    void make_root(int u);

    // Exposes path from root to u in the splay tree
    void access(int u);
};

} // namespace graphlib

#endif
