#include "graphlib/contest_algorithms.h"
#include <algorithm>
#include <functional>
#include <limits>
#include <queue>
#include <stdexcept>
namespace graphlib {
namespace { void valid_source(const Graph& g, int s) { if (s < 0 || s >= g.vertex_count()) throw std::out_of_range("Source vertex out of range"); } }
std::vector<long long> widest_paths(const Graph& g, int s) {
    valid_source(g, s); const long long bad = std::numeric_limits<long long>::min();
    std::vector<long long> best(g.vertex_count(), bad); std::priority_queue<std::pair<long long,int>> q;
    best[s] = std::numeric_limits<long long>::max(); q.push({best[s], s});
    while (!q.empty()) { auto [c,u]=q.top(); q.pop(); if(c!=best[u]) continue;
        for(Edge* e=g.get_edges(u);e;e=e->next){ if(e->weight<0) throw std::invalid_argument("Widest paths require non-negative weights"); long long x=std::min(c,e->weight); if(x>best[e->to]){best[e->to]=x;q.push({x,e->to});} }
    } return best;
}
std::vector<long long> count_shortest_paths(const Graph& g, int s, long long mod) {
    valid_source(g,s); if(mod<=0) throw std::invalid_argument("Modulo must be positive"); const long long inf=std::numeric_limits<long long>::max();
    std::vector<long long>d(g.vertex_count(),inf),w(g.vertex_count()); using P=std::pair<long long,int>; std::priority_queue<P,std::vector<P>,std::greater<P>>q;
    d[s]=0;w[s]=1;q.push({0,s}); while(!q.empty()){auto [du,u]=q.top();q.pop();if(du!=d[u])continue;for(Edge*e=g.get_edges(u);e;e=e->next){if(e->weight<0)throw std::invalid_argument("Shortest path counting requires non-negative weights");if(du>inf-e->weight)continue;long long nd=du+e->weight;if(nd<d[e->to]){d[e->to]=nd;w[e->to]=w[u];q.push({nd,e->to});}else if(nd==d[e->to])w[e->to]=(w[e->to]+w[u])%mod;}}return w;
}
std::vector<long long> shortest_paths_at_most_k_edges(const Graph& g,int s,int k){
    valid_source(g,s);if(k<0)throw std::invalid_argument("Maximum edge count must be non-negative");const long long inf=std::numeric_limits<long long>::max();std::vector<long long>d(g.vertex_count(),inf);d[s]=0;
    for(int step=0;step<k;++step){auto next=d;for(int u=0;u<g.vertex_count();++u)if(d[u]!=inf)for(Edge*e=g.get_edges(u);e;e=e->next)if(d[u]<=inf-e->weight)next[e->to]=std::min(next[e->to],d[u]+e->weight);d.swap(next);}return d;
}
std::vector<int> lexicographically_smallest_topological_order(const Graph& g,bool& cycle){int n=g.vertex_count();std::vector<int>deg(n),out;for(int u=0;u<n;++u)for(Edge*e=g.get_edges(u);e;e=e->next)++deg[e->to];std::priority_queue<int,std::vector<int>,std::greater<int>>q;for(int i=0;i<n;++i)if(!deg[i])q.push(i);while(!q.empty()){int u=q.top();q.pop();out.push_back(u);for(Edge*e=g.get_edges(u);e;e=e->next)if(!--deg[e->to])q.push(e->to);}cycle=static_cast<int>(out.size())!=n;if(cycle)out.clear();return out;}
long long count_topological_orders(const Graph& g){int n=g.vertex_count();if(n>22)throw std::invalid_argument("Topological-order counting supports at most 22 vertices");std::vector<unsigned long long>pre(n);for(int u=0;u<n;++u)for(Edge*e=g.get_edges(u);e;e=e->next)pre[e->to]|=1ULL<<u;std::vector<long long>dp(1ULL<<n);dp[0]=1;for(unsigned long long mask=0;mask<(1ULL<<n);++mask)if(dp[mask])for(int v=0;v<n;++v)if(!(mask&(1ULL<<v))&&((pre[v]&mask)==pre[v]))dp[mask|1ULL<<v]+=dp[mask];return dp[(1ULL<<n)-1];}
}
