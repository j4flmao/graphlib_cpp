#include <gtest/gtest.h>
#include "graphlib/max_flow.h"
#include "graphlib/mst.h"
#include "graphlib/graph_core.h"
#include "graphlib/shortest_path.h"
#include "graphlib/tree.h"
#include <random>
#include <vector>
#include <algorithm>
#include <numeric>
#include <set>

using namespace graphlib;

class InvariantsExtendedTest : public ::testing::Test {
protected:
    std::mt19937 rng{42};

    int rand_int(int min, int max) {
        return std::uniform_int_distribution<int>(min, max)(rng);
    }
};

TEST_F(InvariantsExtendedTest, MaxFlowMinCutTheorem) {
    // For any network, the max flow value equals the capacity of the min cut.
    int n = 20;
    MaxFlow mf(n);
    
    // Random graph
    // Keep track of edges to calculate cut capacity manually later
    struct EdgeInfo { int u, v; long long cap; };
    std::vector<EdgeInfo> edges;

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            if (i != j && rand_int(0, 100) < 30) {
                long long cap = rand_int(1, 20);
                mf.add_edge(i, j, cap);
                edges.push_back({i, j, cap});
            }
        }
    }
    
    int s = 0, t = n - 1;
    long long flow = mf.dinic(s, t);
    
    std::vector<char> reachable;
    mf.min_cut_reachable_from_source(s, reachable);
    
    long long cut_capacity = 0;
    for (const auto& e : edges) {
        if (reachable[e.u] && !reachable[e.v]) {
            cut_capacity += e.cap;
        }
    }
    
    EXPECT_EQ(flow, cut_capacity);
}

TEST_F(InvariantsExtendedTest, MSTCutProperty) {
    int n = 20;
    std::vector<MstEdge> all_edges;
    
    // Make connected graph with UNIQUE weights
    int weight_counter = 1;
    for (int i = 0; i < n-1; ++i) {
        all_edges.push_back({i, i+1, weight_counter++});
    }
    // Add random edges
    for (int i = 0; i < n; ++i) {
        for (int j = i+1; j < n; ++j) {
            if (std::abs(i - j) > 1 && rand_int(0, 100) < 20) {
                all_edges.push_back({i, j, weight_counter++});
            }
        }
    }
    
    // Copy for library call
    std::vector<MstEdge> lib_edges = all_edges;
    long long lib_weight = MST::kruskal(n, lib_edges);
    
    // My Kruskal to get edges
    std::sort(all_edges.begin(), all_edges.end());
    UnionFind uf(n);
    std::vector<MstEdge> mst_edges;
    long long my_weight = 0;
    
    for (const auto& e : all_edges) {
        if (uf.unite(e.u, e.v)) {
            mst_edges.push_back(e);
            my_weight += e.weight;
        }
    }
    
    EXPECT_EQ(lib_weight, my_weight);
    EXPECT_EQ(mst_edges.size(), n - 1);
    
    // Check Connectivity
    Graph mst_g(n, false);
    for (const auto& e : mst_edges) {
        mst_g.add_edge(e.u, e.v, e.weight);
    }
    EXPECT_TRUE(is_connected(mst_g));
    
    // Check Cycle Property
    // For every edge (u, v) NOT in MST, adding it creates a cycle.
    // The edge (u, v) must be heavier than any edge on the path in MST between u and v.
    
    // Build TreeLCA for path queries
    TreeLCA tree(n);
    for (const auto& e : mst_edges) {
        tree.add_edge(e.u, e.v);
    }
    tree.build(0);
    
    // To query max edge on path, we need to know edge weights.
    // TreeLCA doesn't store weights. 
    // But we can just use a simple BFS/DFS on mst_g as before.
    
    for (const auto& e : all_edges) {
        // Check if e is in MST
        bool in_mst = false;
        for (const auto& me : mst_edges) {
            if (me.u == e.u && me.v == e.v && me.weight == e.weight) {
                in_mst = true;
                break;
            }
        }
        
        if (!in_mst) {
            // Find max weight on path u -> v in MST
            // BFS
            std::vector<int> q;
            q.push_back(e.u);
            std::vector<int> parent(n, -1);
            std::vector<long long> dist_w(n, 0);
            std::vector<bool> vis(n, false);
            vis[e.u] = true;
            
            bool found = false;
            int head = 0;
            while(head < q.size()) {
                int curr = q[head++];
                if (curr == e.v) { found = true; break; }
                
                Edge* edge = mst_g.get_edges(curr);
                while (edge) {
                    if (!vis[edge->to]) {
                        vis[edge->to] = true;
                        parent[edge->to] = curr;
                        dist_w[edge->to] = edge->weight;
                        q.push_back(edge->to);
                    }
                    edge = edge->next;
                }
            }
            
            EXPECT_TRUE(found);
            
            long long max_on_path = 0;
            int curr = e.v;
            while (curr != e.u) {
                max_on_path = std::max(max_on_path, dist_w[curr]);
                curr = parent[curr];
            }
            
            EXPECT_GT(e.weight, max_on_path); // Strictly greater because weights are unique
        }
    }
}

