#include <gtest/gtest.h>
#include <graphlib/graphlib.h>
#include <graphlib/splay_tree.h>
#include <graphlib/bipartite.h>
#include <graphlib/max_flow.h>
#include <vector>
#include <algorithm>
#include <set>
#include <random>

using namespace graphlib;

// Helper for random numbers
namespace {
    std::mt19937 rng(42);
    int rand_int(int min, int max) {
        return std::uniform_int_distribution<int>(min, max)(rng);
    }
}

// --- Splay Tree Tests ---

TEST(SplayTreeAdvanced, StressTest) {
    SplayTree tree;
    std::set<int> elements;
    int n_ops = 1000;
    int max_val = 500;

    for (int i = 0; i < n_ops; ++i) {
        int op = rand_int(0, 2);
        int val = rand_int(1, max_val);

        if (op == 0) { // Insert
            tree.insert(val);
            elements.insert(val);
        } else if (op == 1) { // Remove
            tree.remove(val);
            elements.erase(val);
        } else { // Search
            bool found = tree.search(val);
            bool expected = elements.count(val);
            EXPECT_EQ(found, expected) << "Search mismatch for " << val;
        }
    }

    // Verify all elements present
    for (int x : elements) {
        EXPECT_TRUE(tree.search(x)) << "Missing element " << x;
    }
    
    // Verify non-elements
    for (int i = 1; i <= max_val; ++i) {
        if (elements.find(i) == elements.end()) {
            EXPECT_FALSE(tree.search(i)) << "False positive for " << i;
        }
    }
}

TEST(SplayTreeAdvanced, RangeSumCorrectness) {
    SplayTree tree;
    std::vector<int> nums;
    int n = 100;
    for (int i = 0; i < n; ++i) {
        int val = rand_int(1, 1000);
        // Ensure uniqueness for simplicity in verification if set logic used,
        // but SplayTree might handle duplicates depending on impl. 
        // Standard SplayTree usually unique keys or multiset.
        // Assuming unique keys based on basic tests.
        if (!tree.search(val)) {
            tree.insert(val);
            nums.push_back(val);
        }
    }
    std::sort(nums.begin(), nums.end());

    int queries = 50;
    for (int k = 0; k < queries; ++k) {
        int l = rand_int(0, 1000);
        int r = rand_int(l, 1000);
        
        long long expected_sum = 0;
        for (int x : nums) {
            if (x >= l && x <= r) {
                expected_sum += x;
            }
        }
        
        // range_sum returns int? Check header.
        // Assuming int or long long.
        long long actual_sum = tree.range_sum(l, r);
        EXPECT_EQ(actual_sum, expected_sum) << "Range sum mismatch [" << l << ", " << r << "]";
    }
}

// --- Bipartite Tests ---

TEST(BipartiteAdvanced, MatchingAgainstMaxFlow) {
    int n_left = 20;
    int n_right = 20;
    BipartiteGraph bg(n_left, n_right);
    
    // Build equivalent MaxFlow network
    // Source: 0, Sink: n_left + n_right + 1
    // Left nodes: 1..n_left
    // Right nodes: n_left+1..n_left+n_right
    int source = 0;
    int sink = n_left + n_right + 1;
    MaxFlow mf(sink + 1);

    for (int i = 0; i < n_left; ++i) {
        mf.add_edge(source, i + 1, 1);
        for (int j = 0; j < n_right; ++j) {
            if (rand_int(0, 100) < 30) { // 30% density
                bg.add_edge(i, n_left + j); // BipartiteGraph: u in left (0..n_left-1), v in right (n_left..total-1)
                mf.add_edge(i + 1, n_left + 1 + j, 1);
            }
        }
    }
    for (int j = 0; j < n_right; ++j) {
        mf.add_edge(n_left + 1 + j, sink, 1);
    }

    int matching_size = bg.maximum_matching();
    long long max_flow_val = mf.dinic(source, sink);
    
    EXPECT_EQ(matching_size, max_flow_val);
}

TEST(BipartiteAdvanced, HungarianMatrix) {
    // 3 workers, 3 jobs
    // Cost matrix:
    //    J0 J1 J2
    // W0  2  3  3
    // W1  3  2  3
    // W2  3  3  2
    // Min cost assignment: (0,0), (1,1), (2,2) => 2+2+2 = 6
    
    int n = 3;
    BipartiteGraph bg(n, n);
    // Left nodes 0,1,2. Right nodes 3,4,5.
    
    bg.add_edge(0, 3 + 0, 2);
    bg.add_edge(0, 3 + 1, 3);
    bg.add_edge(0, 3 + 2, 3);
    
    bg.add_edge(1, 3 + 0, 3);
    bg.add_edge(1, 3 + 1, 2);
    bg.add_edge(1, 3 + 2, 3);
    
    bg.add_edge(2, 3 + 0, 3);
    bg.add_edge(2, 3 + 1, 3);
    bg.add_edge(2, 3 + 2, 2);
    
    long long inf = 1e9;
    long long min_cost = bg.hungarian_min_cost(inf);
    EXPECT_EQ(min_cost, 6);
}

// --- Gomory-Hu Tree Tests ---

TEST(GomoryHuAdvanced, AllPairsMinCutVerification) {
    int n = 8;
    Graph g(n, false); // Undirected
    
    // Create random weighted graph
    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            if (rand_int(0, 100) < 50) {
                g.add_edge(i, j, rand_int(1, 20));
            }
        }
    }
    // Ensure connected
    for (int i = 0; i < n-1; ++i) {
        g.add_edge(i, i+1, 10);
    }

    std::vector<int> parent;
    std::vector<long long> min_cut;
    gomory_hu_tree(g, parent, min_cut);

    // Build the GH tree structure to query min cuts
    struct GHEdge {
        int to;
        long long weight;
    };
    std::vector<std::vector<GHEdge>> gh_adj(n);
    for (int i = 0; i < n; ++i) {
        if (parent[i] != -1) {
            gh_adj[i].push_back({parent[i], min_cut[i]});
            gh_adj[parent[i]].push_back({i, min_cut[i]});
        }
    }

    // Helper to find min edge on path in GH tree
    auto get_gh_min_cut = [&](int u, int v) -> long long {
        // BFS to find path
        std::vector<int> p(n, -1);
        std::vector<long long> w(n, 0);
        std::vector<bool> vis(n, false);
        std::vector<int> q;
        q.push_back(u);
        vis[u] = true;
        
        int head = 0;
        bool found = false;
        while(head < q.size()){
            int curr = q[head++];
            if (curr == v) { found = true; break; }
            for (auto& edge : gh_adj[curr]) {
                if (!vis[edge.to]) {
                    vis[edge.to] = true;
                    p[edge.to] = curr;
                    w[edge.to] = edge.weight;
                    q.push_back(edge.to);
                }
            }
        }
        if (!found) return 0; // Should be connected
        
        long long min_val = 1e18; // Infinity
        int curr = v;
        while (curr != u) {
            min_val = std::min(min_val, w[curr]);
            curr = p[curr];
        }
        return min_val;
    };

    // Verify against actual max flow
    for (int u = 0; u < n; ++u) {
        for (int v = u + 1; v < n; ++v) {
            long long gh_val = get_gh_min_cut(u, v);
            
            // Compute Max Flow in original graph
            MaxFlow mf(n);
            // Copy edges. Since MaxFlow is directed, add both ways for undirected
            for (int i = 0; i < n; ++i) {
                Edge* e = g.get_edges(i);
                while (e) {
                    if (e->to > i) { // Add once per pair
                         mf.add_edge(i, e->to, e->weight);
                         mf.add_edge(e->to, i, e->weight);
                    }
                    e = e->next;
                }
            }
            long long actual_flow = mf.dinic(u, v);
            
            EXPECT_EQ(gh_val, actual_flow) << "Mismatch for pair (" << u << ", " << v << ")";
        }
    }
}
