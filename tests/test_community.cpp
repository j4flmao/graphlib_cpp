#include <gtest/gtest.h>
#include "graphlib/community.h"
#include <vector>
#include <map>
#include <iostream>

using namespace graphlib;

TEST(CommunityTest, DisconnectedCliques) {
    // Two disconnected K3s
    Graph g(6);
    // Clique 0-1-2
    g.add_edge(0, 1); g.add_edge(1, 2); g.add_edge(2, 0);
    // Clique 3-4-5
    g.add_edge(3, 4); g.add_edge(4, 5); g.add_edge(5, 3);
    
    auto com = louvain_communities(g);
    
    EXPECT_EQ(com[0], com[1]);
    EXPECT_EQ(com[1], com[2]);
    EXPECT_EQ(com[3], com[4]);
    EXPECT_EQ(com[4], com[5]);
    
    // Likely different identifiers
    EXPECT_NE(com[0], com[3]); 
}

TEST(CommunityTest, ConnectedCliques) {
    // Two K4s connected by a single edge
    Graph g(8);
    // Left: 0,1,2,3
    for(int i=0; i<4; ++i)
        for(int j=i+1; j<4; ++j)
            g.add_edge(i, j);
            
    // Right: 4,5,6,7
    for(int i=4; i<8; ++i)
        for(int j=i+1; j<8; ++j)
            g.add_edge(i, j);
            
    // Bridge: 3-4
    g.add_edge(3, 4);
    
    auto com = louvain_communities(g);
    
    // Should still identify 2 communities clearly because bond is weak
    EXPECT_EQ(com[0], com[1]);
    EXPECT_EQ(com[0], com[2]);
    EXPECT_EQ(com[0], com[3]);
    
    EXPECT_EQ(com[4], com[5]);
    EXPECT_EQ(com[4], com[6]);
    EXPECT_EQ(com[4], com[7]);
    
    EXPECT_NE(com[0], com[7]);
}

TEST(CommunityTest, RingOfCliques) {
    // 4 cliques of size 4, connected in a ring.
    // C1: 0,1,2,3
    // C2: 4,5,6,7
    // C3: 8,9,10,11
    // C4: 12,13,14,15
    // Edges: 3-4, 7-8, 11-12, 15-0
    
    Graph g(16);
    auto make_clique = [&](int start) {
        for(int i=0; i<4; ++i)
            for(int j=i+1; j<4; ++j)
                g.add_edge(start+i, start+j);
    };
    
    make_clique(0);
    make_clique(4);
    make_clique(8);
    make_clique(12);
    
    g.add_edge(3, 4);
    g.add_edge(7, 8);
    g.add_edge(11, 12);
    g.add_edge(15, 0);
    
    auto com = louvain_communities(g);
    
    // Check if cliques are preserved
    for(int start=0; start<16; start+=4) {
        int c = com[start];
        for(int k=1; k<4; ++k) {
            EXPECT_EQ(com[start+k], c);
        }
    }
    
    // Should be 4 communities
    std::map<int, int> counts;
    for(int x : com) counts[x]++;
    EXPECT_EQ(counts.size(), 4);
}

TEST(CommunityTest, ZacharyKarateClubish) {
    // Simplified structure resembling ZKC
    // Two hubs (0 and 33) with strong local connections
    Graph g(34);
    
    // Hub 0 connections
    for(int i=1; i<=8; ++i) g.add_edge(0, i);
    for(int i=1; i<=8; ++i)
        for(int j=i+1; j<=8; ++j) if(rand()%3==0) g.add_edge(i,j);
        
    // Hub 33 connections
    for(int i=25; i<=32; ++i) g.add_edge(33, i);
     for(int i=25; i<=32; ++i)
        for(int j=i+1; j<=32; ++j) if(rand()%3==0) g.add_edge(i,j);
        
    // Weak connection
    g.add_edge(8, 25);
    
    auto com = louvain_communities(g);
    
    // 0 and 33 should probably be distinct
    EXPECT_NE(com[0], com[33]);
}
