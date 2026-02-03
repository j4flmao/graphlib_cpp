#include <gtest/gtest.h>
#include "graphlib/motif.h"
#include "graphlib/graph_core.h"
#include <cmath>
#include <set>
#include <tuple>

using namespace graphlib;

class MotifTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

// ============================================================================
// count_triangles tests
// ============================================================================

TEST_F(MotifTest, CountTrianglesK3) {
    // K3: Triangle 0-1-2-0
    Graph g(3);
    g.add_edge(0, 1); g.add_edge(1, 0);
    g.add_edge(1, 2); g.add_edge(2, 1);
    g.add_edge(2, 0); g.add_edge(0, 2);
    
    auto result = count_triangles(g);
    EXPECT_EQ(result.total_triangles, 1);
    EXPECT_EQ(result.per_vertex.size(), 3u);
    for (int i = 0; i < 3; ++i) {
        EXPECT_EQ(result.per_vertex[i], 1);
    }
}

TEST_F(MotifTest, CountTrianglesK4) {
    // K4: Complete graph on 4 vertices -> 4 triangles
    Graph g(4);
    for (int i = 0; i < 4; ++i) {
        for (int j = i + 1; j < 4; ++j) {
            g.add_edge(i, j);
            g.add_edge(j, i);
        }
    }
    
    auto result = count_triangles(g);
    EXPECT_EQ(result.total_triangles, 4);
    for (int i = 0; i < 4; ++i) {
        EXPECT_EQ(result.per_vertex[i], 3);
    }
}

TEST_F(MotifTest, CountTrianglesNoTriangle) {
    // Path: 0-1-2-3 (no triangles)
    Graph g(4);
    g.add_edge(0, 1); g.add_edge(1, 0);
    g.add_edge(1, 2); g.add_edge(2, 1);
    g.add_edge(2, 3); g.add_edge(3, 2);
    
    auto result = count_triangles(g);
    EXPECT_EQ(result.total_triangles, 0);
}

TEST_F(MotifTest, CountTrianglesEmptyGraph) {
    Graph g(5);
    auto result = count_triangles(g);
    EXPECT_EQ(result.total_triangles, 0);
}

// ============================================================================
// list_triangles tests
// ============================================================================

TEST_F(MotifTest, ListTrianglesK3) {
    Graph g(3);
    g.add_edge(0, 1); g.add_edge(1, 0);
    g.add_edge(1, 2); g.add_edge(2, 1);
    g.add_edge(2, 0); g.add_edge(0, 2);
    
    auto triangles = list_triangles(g);
    EXPECT_EQ(triangles.size(), 1u);
    
    // Verify triangle contains vertices 0, 1, 2
    auto& tri = triangles[0];
    std::set<int> vertices = {std::get<0>(tri), std::get<1>(tri), std::get<2>(tri)};
    EXPECT_EQ(vertices, (std::set<int>{0, 1, 2}));
}

TEST_F(MotifTest, ListTrianglesK4) {
    Graph g(4);
    for (int i = 0; i < 4; ++i) {
        for (int j = i + 1; j < 4; ++j) {
            g.add_edge(i, j);
            g.add_edge(j, i);
        }
    }
    
    auto triangles = list_triangles(g);
    EXPECT_EQ(triangles.size(), 4u);
}

TEST_F(MotifTest, ListTrianglesWithLimit) {
    // K4 has 4 triangles, limit to 2
    Graph g(4);
    for (int i = 0; i < 4; ++i) {
        for (int j = i + 1; j < 4; ++j) {
            g.add_edge(i, j);
            g.add_edge(j, i);
        }
    }
    
    auto triangles = list_triangles(g, 2);
    EXPECT_LE(triangles.size(), 2u);
}

TEST_F(MotifTest, ListTrianglesNoTriangle) {
    Graph g(4);
    g.add_edge(0, 1); g.add_edge(1, 0);
    g.add_edge(1, 2); g.add_edge(2, 1);
    g.add_edge(2, 3); g.add_edge(3, 2);
    
    auto triangles = list_triangles(g);
    EXPECT_EQ(triangles.size(), 0u);
}

// ============================================================================
// count_4cliques tests
// ============================================================================

TEST_F(MotifTest, Count4CliquesK4) {
    // K4 has exactly 1 4-clique
    Graph g(4);
    for (int i = 0; i < 4; ++i) {
        for (int j = i + 1; j < 4; ++j) {
            g.add_edge(i, j);
            g.add_edge(j, i);
        }
    }
    
    EXPECT_EQ(count_4cliques(g), 1);
}

TEST_F(MotifTest, Count4CliquesK5) {
    // K5 has C(5,4) = 5 4-cliques
    Graph g(5);
    for (int i = 0; i < 5; ++i) {
        for (int j = i + 1; j < 5; ++j) {
            g.add_edge(i, j);
            g.add_edge(j, i);
        }
    }
    
    EXPECT_EQ(count_4cliques(g), 5);
}

TEST_F(MotifTest, Count4CliquesNo4Clique) {
    // K3 has no 4-clique
    Graph g(3);
    g.add_edge(0, 1); g.add_edge(1, 0);
    g.add_edge(1, 2); g.add_edge(2, 1);
    g.add_edge(2, 0); g.add_edge(0, 2);
    
    EXPECT_EQ(count_4cliques(g), 0);
}

// ============================================================================
// transitivity tests
// ============================================================================

TEST_F(MotifTest, TransitivityK3) {
    // Complete graph has transitivity = 1.0
    Graph g(3);
    g.add_edge(0, 1); g.add_edge(1, 0);
    g.add_edge(1, 2); g.add_edge(2, 1);
    g.add_edge(2, 0); g.add_edge(0, 2);
    
    EXPECT_DOUBLE_EQ(transitivity(g), 1.0);
}

TEST_F(MotifTest, TransitivityPath) {
    // Path has transitivity = 0.0 (no triangles)
    Graph g(4);
    g.add_edge(0, 1); g.add_edge(1, 0);
    g.add_edge(1, 2); g.add_edge(2, 1);
    g.add_edge(2, 3); g.add_edge(3, 2);
    
    EXPECT_DOUBLE_EQ(transitivity(g), 0.0);
}

TEST_F(MotifTest, TransitivityStar) {
    // Star graph (center connects to all) has transitivity = 0.0
    Graph g(5);
    for (int i = 1; i < 5; ++i) {
        g.add_edge(0, i);
        g.add_edge(i, 0);
    }
    
    EXPECT_DOUBLE_EQ(transitivity(g), 0.0);
}

TEST_F(MotifTest, TransitivityK4) {
    // K4 has transitivity = 1.0
    Graph g(4);
    for (int i = 0; i < 4; ++i) {
        for (int j = i + 1; j < 4; ++j) {
            g.add_edge(i, j);
            g.add_edge(j, i);
        }
    }
    
    EXPECT_DOUBLE_EQ(transitivity(g), 1.0);
}

// ============================================================================
// count_paths_of_length tests
// ============================================================================

TEST_F(MotifTest, CountPathsOfLength1) {
    // Simple edge 0-1
    Graph g(2);
    g.add_edge(0, 1);
    g.add_edge(1, 0);
    
    EXPECT_EQ(count_paths_of_length(g, 0, 1, 1), 1);
    EXPECT_EQ(count_paths_of_length(g, 1, 0, 1), 1);
}

TEST_F(MotifTest, CountPathsOfLength2) {
    // Path 0-1-2
    Graph g(3);
    g.add_edge(0, 1); g.add_edge(1, 0);
    g.add_edge(1, 2); g.add_edge(2, 1);
    
    EXPECT_EQ(count_paths_of_length(g, 0, 2, 2), 1);
    EXPECT_EQ(count_paths_of_length(g, 0, 2, 1), 0);
}

TEST_F(MotifTest, CountPathsOfLengthK3) {
    // K3: multiple paths
    Graph g(3);
    g.add_edge(0, 1); g.add_edge(1, 0);
    g.add_edge(1, 2); g.add_edge(2, 1);
    g.add_edge(2, 0); g.add_edge(0, 2);
    
    // Length 1: direct edge
    EXPECT_EQ(count_paths_of_length(g, 0, 1, 1), 1);
    
    // Length 2: 0->2->1
    EXPECT_EQ(count_paths_of_length(g, 0, 1, 2), 1);
}

TEST_F(MotifTest, CountPathsOfLengthNoPath) {
    // Disconnected graph
    Graph g(4);
    g.add_edge(0, 1); g.add_edge(1, 0);
    g.add_edge(2, 3); g.add_edge(3, 2);
    
    EXPECT_EQ(count_paths_of_length(g, 0, 2, 1), 0);
    EXPECT_EQ(count_paths_of_length(g, 0, 2, 2), 0);
    EXPECT_EQ(count_paths_of_length(g, 0, 2, 10), 0);
}

TEST_F(MotifTest, CountPathsOfLengthSameVertex) {
    Graph g(3);
    g.add_edge(0, 1); g.add_edge(1, 0);
    g.add_edge(1, 2); g.add_edge(2, 1);
    g.add_edge(2, 0); g.add_edge(0, 2);
    
    // Paths of length 0 from v to v should be 1
    EXPECT_EQ(count_paths_of_length(g, 0, 0, 0), 1);
}
