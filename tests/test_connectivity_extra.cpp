#include <gtest/gtest.h>
#include "graphlib/connectivity_extra.h"
using namespace graphlib;
TEST(ConnectivityExtra, CompleteGraph){Graph g(4,false);for(int i=0;i<4;++i)for(int j=i+1;j<4;++j)g.add_edge(i,j);EXPECT_EQ(edge_connectivity(g),3);EXPECT_EQ(vertex_connectivity(g),3);}
TEST(ConnectivityExtra, Path){Graph g(4,false);g.add_edge(0,1);g.add_edge(1,2);g.add_edge(2,3);EXPECT_EQ(edge_connectivity(g),1);EXPECT_EQ(vertex_connectivity(g),1);}
