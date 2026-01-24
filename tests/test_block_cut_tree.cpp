#include "graphlib/block_cut_tree.h"
#include <gtest/gtest.h>
#include <algorithm>

using namespace graphlib;

class BlockCutTreeTest : public ::testing::Test {
protected:
    void SetUp() override {}
};

TEST_F(BlockCutTreeTest, FindArticulationPoints_SimplePath) {
    // 0-1-2
    Graph g(3, false);
    g.add_edge(0, 1);
    g.add_edge(1, 2);

    std::vector<int> aps = find_articulation_points(g);
    ASSERT_EQ(aps.size(), 1);
    EXPECT_EQ(aps[0], 1);
}

TEST_F(BlockCutTreeTest, FindArticulationPoints_Cycle) {
    // 0-1-2-0 (Triangle)
    Graph g(3, false);
    g.add_edge(0, 1);
    g.add_edge(1, 2);
    g.add_edge(2, 0);

    std::vector<int> aps = find_articulation_points(g);
    EXPECT_TRUE(aps.empty());
}

TEST_F(BlockCutTreeTest, FindBridges_SimplePath) {
    // 0-1-2
    Graph g(3, false);
    g.add_edge(0, 1);
    g.add_edge(1, 2);

    std::vector<std::pair<int, int>> bridges = find_bridges(g);
    ASSERT_EQ(bridges.size(), 2);
    // Bridges should be sorted
    EXPECT_EQ(bridges[0], std::make_pair(0, 1));
    EXPECT_EQ(bridges[1], std::make_pair(1, 2));
}

TEST_F(BlockCutTreeTest, FindBridges_CycleWithTail) {
    // 0-1-2-0 and 2-3
    Graph g(4, false);
    g.add_edge(0, 1);
    g.add_edge(1, 2);
    g.add_edge(2, 0);
    g.add_edge(2, 3);

    std::vector<std::pair<int, int>> bridges = find_bridges(g);
    ASSERT_EQ(bridges.size(), 1);
    EXPECT_EQ(bridges[0], std::make_pair(2, 3));
}

TEST_F(BlockCutTreeTest, BuildBlockCutTree_Butterfly) {
    // Two triangles sharing a vertex
    // 0-1-2-0 and 2-3-4-2
    // AP: 2
    Graph g(5, false);
    g.add_edge(0, 1);
    g.add_edge(1, 2);
    g.add_edge(2, 0);
    
    g.add_edge(2, 3);
    g.add_edge(3, 4);
    g.add_edge(4, 2);

    BlockCutTreeResult res = build_block_cut_tree(g);
    
    // Expected: 1 AP (node 2), 2 Blocks (triangles)
    EXPECT_EQ(res.num_articulations, 1);
    EXPECT_EQ(res.num_blocks, 2);
    
    // Tree nodes: 0 (AP 2), 1 (Block 1), 2 (Block 2)
    EXPECT_EQ(res.tree.vertex_count(), 3);
    
    // Check AP details
    EXPECT_EQ(res.node_type[0], 1);
    EXPECT_EQ(res.original_id[0], 2);
    
    // Check connections
    // Node 0 should be connected to Node 1 and Node 2
    Edge* e = res.tree.get_edges(0);
    int neighbors = 0;
    while(e) {
        neighbors++;
        EXPECT_TRUE(e->to == 1 || e->to == 2);
        e = e->next;
    }
    EXPECT_EQ(neighbors, 2);
}

TEST_F(BlockCutTreeTest, BuildBlockCutTree_Line) {
    // 0-1-2-3
    // APs: 1, 2
    // Blocks: (0,1), (1,2), (2,3)
    Graph g(4, false);
    g.add_edge(0, 1);
    g.add_edge(1, 2);
    g.add_edge(2, 3);
    
    BlockCutTreeResult res = build_block_cut_tree(g);
    
    EXPECT_EQ(res.num_articulations, 2);
    EXPECT_EQ(res.num_blocks, 3);
    EXPECT_EQ(res.tree.vertex_count(), 5);
}
