#include <gtest/gtest.h>
#include <graphlib/graphlib.h>
#include <vector>
#include <algorithm>
#include <random>
#include <set>
#include <map>

using graphlib::SCC;
using graphlib::Connectivity;
using graphlib::TwoSAT;
using graphlib::DAG;
using graphlib::DynamicSCC;

TEST(SCCTest, SimpleGraph) {
    SCC g(5);
    g.add_edge(0, 1);
    g.add_edge(1, 2);
    g.add_edge(2, 0);
    g.add_edge(1, 3);
    g.add_edge(3, 4);
    g.add_edge(4, 3);

    std::vector<int> comp_k;
    std::vector<int> comp_t;
    int ck = g.kosaraju(comp_k);
    int ct = g.tarjan(comp_t);

    EXPECT_EQ(ck, ct);
    EXPECT_EQ(comp_k.size(), comp_t.size());
}

TEST(ConnectivityTest, ComponentsAndBridges) {
    Connectivity g(5);
    g.add_edge(0, 1);
    g.add_edge(1, 2);
    g.add_edge(2, 0);
    g.add_edge(1, 3);
    g.add_edge(3, 4);

    std::vector<int> comp;
    int cc = g.connected_components(comp);
    EXPECT_EQ(cc, 1);

    std::vector<std::pair<int, int>> bridges;
    g.bridges(bridges);
    EXPECT_FALSE(bridges.empty());
}

TEST(ConnectivityTest, BiconnectedComponentsSimple) {
    Connectivity g(5);
    g.add_edge(0, 1);
    g.add_edge(1, 2);
    g.add_edge(2, 0);
    g.add_edge(1, 3);
    g.add_edge(3, 4);

    std::vector<std::vector<int>> comps;
    g.biconnected_components(comps);

    EXPECT_GE(comps.size(), 2u);

    bool found_cycle_comp = false;
    bool found_bridge_comp = false;

    for (const auto& comp : comps) {
        std::vector<int> sorted_comp = comp;
        std::sort(sorted_comp.begin(), sorted_comp.end());
        if (sorted_comp.size() == 3 &&
            sorted_comp[0] == 0 &&
            sorted_comp[1] == 1 &&
            sorted_comp[2] == 2) {
            found_cycle_comp = true;
        }
        if (sorted_comp.size() == 2 &&
            ((sorted_comp[0] == 1 && sorted_comp[1] == 3) ||
             (sorted_comp[0] == 3 && sorted_comp[1] == 4))) {
            found_bridge_comp = true;
        }
    }

    EXPECT_TRUE(found_cycle_comp);
    EXPECT_TRUE(found_bridge_comp);
}

TEST(SCCTest, CondensationDagHelper) {
    SCC g(5);
    g.add_edge(0, 1);
    g.add_edge(1, 2);
    g.add_edge(2, 0);
    g.add_edge(2, 3);
    g.add_edge(3, 4);

    std::vector<int> comp(5);
    int cnt = g.tarjan(comp);

    DAG dag = graphlib::build_scc_condensation_dag(g, comp, cnt);

    bool has_cycle = false;
    std::vector<int> topo = dag.topological_sort_kahn(has_cycle);

    EXPECT_FALSE(has_cycle);
    EXPECT_EQ(dag.vertex_count(), cnt);

    int c0 = comp[0];
    int c2 = comp[2];
    int c3 = comp[3];
    int c4 = comp[4];

    EXPECT_EQ(c0, comp[1]);
    EXPECT_EQ(c0, c2);

    bool has_edge_c0_c3 = false;
    bool has_edge_c3_c4 = false;

    for (int u = 0; u < dag.vertex_count(); u++) {
        graphlib::Edge* e = dag.get_edges(u);
        while (e) {
            int v = e->to;
            if (u == c0 && v == c3) {
                has_edge_c0_c3 = true;
            }
            if (u == c3 && v == c4) {
                has_edge_c3_c4 = true;
            }
            e = e->next;
        }
    }

    EXPECT_TRUE(has_edge_c0_c3);
    EXPECT_TRUE(has_edge_c3_c4);
}

TEST(DynamicSCCTest, IncrementalSCCQueries) {
    DynamicSCC g(4);

    EXPECT_EQ(g.component_count(), 4);
    EXPECT_FALSE(g.strongly_connected(0, 1));

    g.add_edge(0, 1);
    g.add_edge(1, 0);

    EXPECT_TRUE(g.strongly_connected(0, 1));
    int c0 = g.component_id(0);
    int c1 = g.component_id(1);
    EXPECT_GE(c0, 0);
    EXPECT_EQ(c0, c1);

    g.add_edge(2, 3);
    g.add_edge(3, 2);

    EXPECT_EQ(g.component_count(), 2);
    EXPECT_TRUE(g.strongly_connected(2, 3));
    EXPECT_FALSE(g.strongly_connected(0, 2));
}

TEST(TwoSATTest, SimpleSatisfiable) {
    TwoSAT sat(2);
    sat.add_unit_clause(0, true);
    sat.add_clause(0, false, 1, true);
    sat.add_clause(1, false, 0, true);

    std::vector<bool> assignment;
    bool ok = sat.solve(assignment);

    EXPECT_TRUE(ok);
    EXPECT_EQ(assignment.size(), 2u);
    EXPECT_TRUE(assignment[0]);
    EXPECT_TRUE(assignment[1]);
}

TEST(TwoSATTest, UnsatisfiableFormula) {
    TwoSAT sat(1);
    sat.add_unit_clause(0, true);
    sat.add_unit_clause(0, false);

    std::vector<bool> assignment;
    bool ok = sat.solve(assignment);

    EXPECT_FALSE(ok);
}

TEST(SCCTest, SingleNode) {
    SCC g(1);
    std::vector<int> comp(1);
    int c = g.tarjan(comp);
    EXPECT_EQ(c, 1);
    EXPECT_EQ(comp[0], 0);
}

TEST(SCCTest, DisconnectedComponents) {
    SCC g(6);
    // 0-1-0 cycle
    g.add_edge(0, 1); g.add_edge(1, 0);
    // 2-3-4-2 cycle
    g.add_edge(2, 3); g.add_edge(3, 4); g.add_edge(4, 2);
    // 5 isolated
    
    std::vector<int> comp(6);
    int c = g.tarjan(comp);
    EXPECT_EQ(c, 3);
    EXPECT_EQ(comp[0], comp[1]);
    EXPECT_EQ(comp[2], comp[3]);
    EXPECT_EQ(comp[3], comp[4]);
    EXPECT_NE(comp[0], comp[2]);
    EXPECT_NE(comp[0], comp[5]);
    
    // Check Kosaraju too
    std::vector<int> comp_k(6);
    int ck = g.kosaraju(comp_k);
    EXPECT_EQ(ck, 3);
}

TEST(SCCTest, StressRandom) {
    int n = 100;
    int m = 500;
    SCC g(n);
    std::mt19937 rng(42);
    std::uniform_int_distribution<int> dist(0, n - 1);
    
    for(int i=0; i<m; ++i) {
        g.add_edge(dist(rng), dist(rng));
    }
    
    std::vector<int> comp_t(n), comp_k(n);
    int ct = g.tarjan(comp_t);
    int ck = g.kosaraju(comp_k);
    
    EXPECT_EQ(ct, ck);
    
    // Check if components are effectively the same (ids might differ)
    // Map comp_t id -> set of nodes, comp_k id -> set of nodes
    // Then check if sets match.
    
    std::vector<std::vector<int>> sets_t(ct), sets_k(ck);
    for(int i=0; i<n; ++i) {
        sets_t[comp_t[i]].push_back(i);
        sets_k[comp_k[i]].push_back(i);
    }
    
    for(auto& s : sets_t) std::sort(s.begin(), s.end());
    for(auto& s : sets_k) std::sort(s.begin(), s.end());
    
    std::sort(sets_t.begin(), sets_t.end());
    std::sort(sets_k.begin(), sets_k.end());
    
    EXPECT_EQ(sets_t, sets_k);
}

TEST(ConnectivityTest, ArticulationPoints) {
    // 0-1-2-0, 1-3
    // 1 is articulation point.
    Connectivity g(4);
    g.add_edge(0, 1); g.add_edge(1, 0);
    g.add_edge(1, 2); g.add_edge(2, 1);
    g.add_edge(2, 0); g.add_edge(0, 2);
    g.add_edge(1, 3); g.add_edge(3, 1);
    
    std::vector<std::vector<int>> comps;
    g.biconnected_components(comps);
    
    // Components should be {0,1,2} and {1,3} (edges) or vertices?
    // graphlib::biconnected_components returns vertices in each biconnected component.
    // AP is vertex in >1 component.
    
    std::map<int, int> count;
    for(const auto& c : comps) {
        for(int u : c) count[u]++;
    }
    
    EXPECT_GE(count[1], 2);
    EXPECT_EQ(count[0], 1);
    EXPECT_EQ(count[3], 1);
}

TEST(TwoSATTest, StressRandom) {
    // Generate random 2SAT instance.
    // If n is small (e.g. 10), we can brute force to verify feasibility.
    int n = 10;
    int clauses = 30;
    TwoSAT sat(n);
    std::mt19937 rng(123);
    std::uniform_int_distribution<int> dist_var(0, n - 1);
    std::uniform_int_distribution<int> dist_bool(0, 1);
    
    struct Clause { int u; bool bu; int v; bool bv; };
    std::vector<Clause> clause_list;
    
    for(int i=0; i<clauses; ++i) {
        int u = dist_var(rng);
        bool bu = dist_bool(rng);
        int v = dist_var(rng);
        bool bv = dist_bool(rng);
        sat.add_clause(u, bu, v, bv);
        clause_list.push_back({u, bu, v, bv});
    }
    
    std::vector<bool> assignment;
    bool solvable = sat.solve(assignment);
    
    if (solvable) {
        // Verify assignment satisfies all clauses
        for(const auto& c : clause_list) {
            bool val_u = (assignment[c.u] == c.bu);
            bool val_v = (assignment[c.v] == c.bv);
            EXPECT_TRUE(val_u || val_v);
        }
    } else {
        // Verify with brute force
        bool found = false;
        for(int mask=0; mask<(1<<n); ++mask) {
            bool all_ok = true;
            for(const auto& c : clause_list) {
                bool val_u = ((mask >> c.u) & 1) == c.bu;
                bool val_v = ((mask >> c.v) & 1) == c.bv;
                if (!val_u && !val_v) {
                    all_ok = false;
                    break;
                }
            }
            if (all_ok) {
                found = true;
                break;
            }
        }
        EXPECT_FALSE(found);
    }
}
