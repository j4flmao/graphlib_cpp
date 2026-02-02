#include <gtest/gtest.h>
#include <graphlib/graphlib.h>
#include <graphlib/two_sat.h>
#include <vector>
#include <algorithm>
#include <random>
#include <set>
#include <map>

using graphlib::Graph;
using graphlib::Connectivity;
using graphlib::TwoSat;
using graphlib::DAG;
using graphlib::strongly_connected_components;
using graphlib::build_scc_condensation_dag;

TEST(SCCTestV2, SimpleGraph) {
    Graph g(5, true);
    g.add_edge(0, 1);
    g.add_edge(1, 2);
    g.add_edge(2, 0);
    g.add_edge(1, 3);
    g.add_edge(3, 4);
    g.add_edge(4, 3);

    int count;
    auto comp = strongly_connected_components(g, count);

    EXPECT_GE(count, 2);
    EXPECT_EQ(comp.size(), 5u);
}

TEST(ConnectivityTestV2, ComponentsAndBridges) {
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

TEST(ConnectivityTestV2, BiconnectedComponentsSimple) {
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

TEST(SCCTestV2, CondensationDagHelper) {
    Graph g(5, true);
    g.add_edge(0, 1);
    g.add_edge(1, 2);
    g.add_edge(2, 0);
    g.add_edge(2, 3);
    g.add_edge(3, 4);

    int cnt;
    auto comp = strongly_connected_components(g, cnt);

    DAG dag = build_scc_condensation_dag(g, comp, cnt);

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

TEST(TwoSATTestV2, SimpleSatisfiable) {
    TwoSat sat(2);
    sat.add_implication(0, true);
    sat.add_clause(0, false, 1, true);
    sat.add_clause(1, false, 0, true);

    std::vector<bool> assignment;
    bool ok = sat.solve(assignment);

    EXPECT_TRUE(ok);
    EXPECT_EQ(assignment.size(), 2u);
    EXPECT_TRUE(assignment[0]);
    EXPECT_TRUE(assignment[1]);
}

TEST(TwoSATTestV2, UnsatisfiableFormula) {
    TwoSat sat(1);
    sat.add_implication(0, true);
    sat.add_implication(0, false);

    std::vector<bool> assignment;
    bool ok = sat.solve(assignment);

    EXPECT_FALSE(ok);
}

TEST(SCCTestV2, SingleNode) {
    Graph g(1, true);
    int count;
    auto comp = strongly_connected_components(g, count);
    EXPECT_EQ(count, 1);
    EXPECT_EQ(comp[0], 0);
}

TEST(SCCTestV2, DisconnectedComponents) {
    Graph g(6, true);
    g.add_edge(0, 1); g.add_edge(1, 0);
    g.add_edge(2, 3); g.add_edge(3, 4); g.add_edge(4, 2);
    
    int count;
    auto comp = strongly_connected_components(g, count);
    EXPECT_EQ(count, 3);
    EXPECT_EQ(comp[0], comp[1]);
    EXPECT_EQ(comp[2], comp[3]);
    EXPECT_EQ(comp[3], comp[4]);
    EXPECT_NE(comp[0], comp[2]);
    EXPECT_NE(comp[0], comp[5]);
}

TEST(SCCTestV2, StressRandom) {
    int n = 100;
    int m = 500;
    Graph g(n, true);
    std::mt19937 rng(42);
    std::uniform_int_distribution<int> dist(0, n - 1);
    
    for(int i=0; i<m; ++i) {
        g.add_edge(dist(rng), dist(rng));
    }
    
    int count;
    auto comp = strongly_connected_components(g, count);
    
    EXPECT_GE(count, 1);
    EXPECT_LE(count, n);
    
    std::vector<std::vector<int>> sets(count);
    for(int i=0; i<n; ++i) {
        sets[comp[i]].push_back(i);
    }
    
    for(auto& s : sets) std::sort(s.begin(), s.end());
    std::sort(sets.begin(), sets.end());
    
    EXPECT_EQ(static_cast<int>(sets.size()), count);
}

TEST(ConnectivityTestV2, ArticulationPoints) {
    Connectivity g(4);
    g.add_edge(0, 1); g.add_edge(1, 0);
    g.add_edge(1, 2); g.add_edge(2, 1);
    g.add_edge(2, 0); g.add_edge(0, 2);
    g.add_edge(1, 3); g.add_edge(3, 1);
    
    std::vector<std::vector<int>> comps;
    g.biconnected_components(comps);
    
    std::map<int, int> countMap;
    for(const auto& c : comps) {
        for(int u : c) countMap[u]++;
    }
    
    EXPECT_GE(countMap[1], 2);
    EXPECT_EQ(countMap[0], 1);
    EXPECT_EQ(countMap[3], 1);
}

TEST(TwoSATTestV2, StressRandom) {
    int n = 10;
    int clauses = 30;
    TwoSat sat(n);
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
        for(const auto& c : clause_list) {
            bool val_u = (assignment[c.u] == c.bu);
            bool val_v = (assignment[c.v] == c.bv);
            EXPECT_TRUE(val_u || val_v);
        }
    } else {
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
