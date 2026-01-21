#include <gtest/gtest.h>
#include "graphlib/dag.h"
#include "graphlib/bipartite.h"
#include "graphlib/tree.h"
#include <vector>

using namespace graphlib;

TEST(StructureRobustness, DAGCycleDetection) {
    DAG dag(3);
    dag.add_edge(0, 1);
    dag.add_edge(1, 2);
    
    EXPECT_FALSE(dag.would_create_cycle(0, 2)); // 0->1->2, 0->2 OK
    EXPECT_TRUE(dag.would_create_cycle(2, 0));  // 0->1->2->0 Cycle!
    
    EXPECT_TRUE(dag.add_edge_checked(0, 2));
    EXPECT_FALSE(dag.add_edge_checked(2, 0)); // Should fail
}

TEST(StructureRobustness, BipartiteCheck) {
    // 3 Left (0,1,2), 2 Right (3,4)
    BipartiteGraph bg(3, 2);
    
    // Add edges to make it bipartite initially
    bg.add_edge(0, 3); 
    bg.add_edge(1, 4);
    
    EXPECT_TRUE(bg.is_bipartite());
    
    // To make it NON-bipartite, we need an odd cycle.
    // Adding 0-1 (path 3-0-1-4) is still bipartite.
    // Let's add 0-1, 1-2, 2-0 triangle.
    // We already have 0, 1, 2.
    // Clear edges or just add triangle.
    bg.add_edge(0, 1);
    bg.add_edge(1, 2);
    bg.add_edge(2, 0);
    
    EXPECT_FALSE(bg.is_bipartite());
}

TEST(StructureRobustness, TreeCycleHandling) {
    // TreeLCA usually expects a tree.
    // If we give it a cycle, build() might hang or crash if not protected.
    // 0-1-2-0
    TreeLCA t(3);
    t.add_edge(0, 1);
    t.add_edge(1, 2);
    t.add_edge(2, 0);
    
    // If build() is naive DFS without parent check for back-edges, it might infinite loop.
    // Or if it tracks parent, it might just ignore the back edge or treat it as tree edge (if undirected).
    // In undirected DFS, 2->0 sees 0 visited.
    // We patched TreeLCA to ignore back-edges (DFS tree).
    
    t.build(0); 
    
    // Graph edges: (0,1), (1,2), (2,0).
    // Adjacency list is usually LIFO.
    // Edges of 0: 2, 1.
    // DFS visits 2 first -> 0->2.
    // Edges of 2: 0, 1. 0 is parent. Visits 1 -> 2->1.
    // Tree structure: 0 -> 2 -> 1.
    
    // Check structure based on LIFO assumption
    EXPECT_EQ(t.kth_ancestor(1, 1), 2); // 1->2
    EXPECT_EQ(t.kth_ancestor(2, 1), 0); // 2->0
    EXPECT_EQ(t.kth_ancestor(1, 2), 0); // 1->2->0
    
    // Distances
    EXPECT_EQ(t.distance(0, 1), 2);
    EXPECT_EQ(t.distance(0, 2), 1);
}
