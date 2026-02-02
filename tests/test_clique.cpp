#include <gtest/gtest.h>
#include "graphlib/clique.h"
#include <set>

using namespace graphlib;

TEST(CliqueTest, Triangle) {
    // 0-1, 1-2, 2-0 (Clique size 3)
    Graph g(3);
    g.add_edge(0, 1); g.add_edge(1, 0);
    g.add_edge(1, 2); g.add_edge(2, 1);
    g.add_edge(2, 0); g.add_edge(0, 2);
    
    auto clique = maximum_clique(g);
    EXPECT_EQ(clique.size(), 3);
}

TEST(CliqueTest, MaxCliqueValues) {
    // 0-1, 1-2, 2-0 (C3)
    // 0-3, 1-3, 2-3 (C4 with 3) -> 0,1,2,3 is C4.
    Graph g(4);
    g.add_edge(0, 1); g.add_edge(1, 0);
    g.add_edge(1, 2); g.add_edge(2, 1);
    g.add_edge(2, 0); g.add_edge(0, 2);
    
    g.add_edge(0, 3); g.add_edge(3, 0);
    g.add_edge(1, 3); g.add_edge(3, 1);
    g.add_edge(2, 3); g.add_edge(3, 2);
    
    auto clique = maximum_clique(g);
    EXPECT_EQ(clique.size(), 4);
}

TEST(CliqueTest, TwoCliques) {
    // 0-1-2 (C3) - isolated - 3-4 (C2)
    Graph g(5);
    g.add_edge(0, 1); g.add_edge(1, 0);
    g.add_edge(1, 2); g.add_edge(2, 1);
    g.add_edge(2, 0); g.add_edge(0, 2);
    
    g.add_edge(3, 4); g.add_edge(4, 3);
    
    auto clique = maximum_clique(g);
    EXPECT_EQ(clique.size(), 3);
}

TEST(CliqueTest, AllMaximalCliques) {
    // Bowtie: 0-1, 1-2, 2-0 (Triangle 1)
    // 2 is center used in both?
    // 2-3, 3-4, 4-2 (Triangle 2)
    // Cliques: {0,1,2} and {2,3,4}.
    Graph g(5);
    // T1
    g.add_edge(0, 1); g.add_edge(1, 0);
    g.add_edge(1, 2); g.add_edge(2, 1);
    g.add_edge(2, 0); g.add_edge(0, 2);
    // T2
    g.add_edge(2, 3); g.add_edge(3, 2);
    g.add_edge(3, 4); g.add_edge(4, 3);
    g.add_edge(4, 2); g.add_edge(2, 4);
    
    auto all = find_all_maximal_cliques(g);
    EXPECT_EQ(all.size(), 2);
    
    // Verify contents
    // Need to sort to check
    // Could just check sizes
}
