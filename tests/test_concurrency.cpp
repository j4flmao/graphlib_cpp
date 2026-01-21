#include <gtest/gtest.h>
#include "graphlib/shortest_path.h"
#include "graphlib/mst.h"
#include "graphlib/max_flow.h"
#include <future>
#include <vector>
#include <numeric>

using namespace graphlib;

TEST(ConcurrencyTest, IndependentGraphs) {
    auto task1 = std::async(std::launch::async, []() {
        ShortestPath sp(100);
        for(int i=0; i<99; ++i) sp.add_edge(i, i+1, 1);
        return sp.dijkstra(0, 1e18)[99];
    });

    auto task2 = std::async(std::launch::async, []() {
        ShortestPath sp(100);
        for(int i=0; i<99; ++i) sp.add_edge(i, i+1, 2); // Different weight
        return sp.dijkstra(0, 1e18)[99];
    });

    EXPECT_EQ(task1.get(), 99);
    EXPECT_EQ(task2.get(), 198);
}

TEST(ConcurrencyTest, MixedAlgorithms) {
    auto task_mst = std::async(std::launch::async, []() {
        std::vector<MstEdge> edges;
        for(int i=0; i<10; ++i) edges.push_back({i, (i+1)%10, 1});
        return MST::kruskal(10, edges);
    });

    auto task_flow = std::async(std::launch::async, []() {
        MaxFlow mf(4);
        mf.add_edge(0, 1, 10);
        mf.add_edge(1, 2, 10);
        mf.add_edge(2, 3, 10);
        return mf.dinic(0, 3);
    });

    EXPECT_EQ(task_mst.get(), 9); // Cycle of 10 nodes, MST uses 9 edges
    EXPECT_EQ(task_flow.get(), 10);
}

TEST(ConcurrencyTest, SameAlgoParallel) {
    std::vector<std::future<long long>> futures;
    for(int i=0; i<10; ++i) {
        futures.push_back(std::async(std::launch::async, [i]() {
            ShortestPath sp(10);
            sp.add_edge(0, 1, i);
            return sp.dijkstra(0, 1e18)[1];
        }));
    }

    for(int i=0; i<10; ++i) {
        EXPECT_EQ(futures[i].get(), i);
    }
}
