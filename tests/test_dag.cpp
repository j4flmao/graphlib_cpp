#include <gtest/gtest.h>
#include <graphlib/graphlib.h>

using graphlib::DAG;

TEST(DagTest, TopologicalSortKahnAndDfs) {
    DAG dag(3);
    dag.add_edge(0, 1);
    dag.add_edge(1, 2);

    bool has_cycle_kahn = false;
    bool has_cycle_dfs = false;
    std::vector<int> order_kahn = dag.topological_sort_kahn(has_cycle_kahn);
    std::vector<int> order_dfs = dag.topological_sort_dfs(has_cycle_dfs);

    EXPECT_FALSE(has_cycle_kahn);
    EXPECT_FALSE(has_cycle_dfs);
    EXPECT_EQ(order_kahn.size(), 3u);
    EXPECT_EQ(order_dfs.size(), 3u);
    EXPECT_EQ(order_kahn.front(), 0);
    EXPECT_EQ(order_kahn.back(), 2);
    EXPECT_EQ(order_dfs.front(), 0);
    EXPECT_EQ(order_dfs.back(), 2);
}

TEST(DagTest, LongestAndShortestPath) {
    DAG dag(4);
    dag.add_edge(0, 1, 1);
    dag.add_edge(1, 2, 2);
    dag.add_edge(0, 3, 4);
    dag.add_edge(3, 2, 1);

    long long minus_inf = std::numeric_limits<long long>::min() / 4;
    long long inf = std::numeric_limits<long long>::max() / 4;

    std::vector<long long> longest = dag.longest_path(0, minus_inf);
    std::vector<long long> shortest = dag.shortest_path(0, inf);

    EXPECT_EQ(longest[2], 5);
    EXPECT_EQ(shortest[2], 3);
}

TEST(DagTest, CountPaths) {
    DAG dag(4);
    dag.add_edge(0, 1);
    dag.add_edge(1, 3);
    dag.add_edge(0, 2);
    dag.add_edge(2, 3);

    long long ways = dag.count_paths(0, 3);
    EXPECT_EQ(ways, 2);
}

TEST(DagTest, WouldCreateCycleDetectsBackEdge) {
    DAG dag(3);
    dag.add_edge(0, 1);
    dag.add_edge(1, 2);

    bool c1 = dag.would_create_cycle(0, 2);
    bool c2 = dag.would_create_cycle(2, 0);

    EXPECT_FALSE(c1);
    EXPECT_TRUE(c2);
}

TEST(DagTest, AddEdgeCheckedPreventsCycle) {
    DAG dag(3);
    dag.add_edge(0, 1);
    dag.add_edge(1, 2);

    bool ok1 = dag.add_edge_checked(0, 2);
    bool ok2 = dag.add_edge_checked(2, 0);

    EXPECT_TRUE(ok1);
    EXPECT_FALSE(ok2);

    bool has_cycle = false;
    std::vector<int> order = dag.topological_sort_kahn(has_cycle);
    EXPECT_FALSE(has_cycle);
    EXPECT_EQ(order.size(), 3u);
}
