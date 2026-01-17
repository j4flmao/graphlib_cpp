#ifndef GRAPHLIB_MST_H
#define GRAPHLIB_MST_H

#include "export.h"
#include <vector>

namespace graphlib {

struct MstEdge {
    int u;
    int v;
    long long weight;

    bool operator<(const MstEdge& other) const {
        return weight < other.weight;
    }
};

class GRAPHLIB_API UnionFind {
private:
    int n_;
    int* parent_;
    int* rank_;

public:
    explicit UnionFind(int n);
    ~UnionFind();

    UnionFind(const UnionFind&) = delete;
    UnionFind& operator=(const UnionFind&) = delete;
    UnionFind(UnionFind&& other) noexcept;
    UnionFind& operator=(UnionFind&& other) noexcept;

    int find(int x);
    bool unite(int x, int y);
};

class GRAPHLIB_API MST {
public:
    static long long kruskal(int n, std::vector<MstEdge>& edges);
    static long long prim(int n, int start, const std::vector<std::vector<std::pair<int, long long>>>& adj, long long inf);
};

}

#endif

