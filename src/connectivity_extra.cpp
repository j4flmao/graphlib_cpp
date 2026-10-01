#include "graphlib/connectivity_extra.h"
#include "graphlib/max_flow.h"
#include <algorithm>
#include <limits>
#include <stdexcept>
#include <vector>
namespace graphlib {
namespace {
void require_undirected(const Graph& g){if(g.is_directed())throw std::invalid_argument("Connectivity requires an undirected graph");}
long long flow_between(const Graph& g,int s,int t){MaxFlow f(g.vertex_count());for(int u=0;u<g.vertex_count();++u)for(Edge*e=g.get_edges(u);e;e=e->next)if(u<e->to&&e->to!=u)f.add_undirected_edge(u,e->to,1);return f.dinic(s,t);}
}
int edge_connectivity(const Graph& g){require_undirected(g);int n=g.vertex_count();if(n<2)return 0;long long best=std::numeric_limits<long long>::max();for(int s=1;s<n;++s)best=std::min(best,flow_between(g,0,s));return static_cast<int>(best);}
int vertex_connectivity(const Graph& g){require_undirected(g);int n=g.vertex_count();if(n<2)return 0;int best=n-1;for(int s=0;s<n;++s)for(int t=s+1;t<n;++t){int in=2*n,out=2*n+1;MaxFlow f(2*n+2);for(int v=0;v<n;++v){long long cap=(v==s||v==t)?n:n-1;f.add_edge(2*v,2*v+1,cap);}for(int u=0;u<n;++u)for(Edge*e=g.get_edges(u);e;e=e->next)if(u<e->to){f.add_edge(2*u+1,2*e->to,n);f.add_edge(2*e->to+1,2*u,n);}f.add_edge(in,2*s,n);f.add_edge(2*t+1,out,n);best=std::min(best,static_cast<int>(f.dinic(in,out)));}return best;}
}
