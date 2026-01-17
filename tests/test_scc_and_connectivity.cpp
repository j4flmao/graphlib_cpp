#include <gtest/gtest.h>
#include <graphlib/graphlib.h>
#include <vector>
#include <algorithm>

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
