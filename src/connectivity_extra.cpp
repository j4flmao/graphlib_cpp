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
int vertex_connectivity(const Graph& g){
    require_undirected(g);
    int n = g.vertex_count();
    if (n < 2) return 0;
    if (n <= 20) {
        auto connected_after_removal = [&](unsigned int removed) {
            int start = -1;
            for (int v = 0; v < n; ++v) if (!(removed & (1u << v))) { start = v; break; }
            if (start < 0) return true;
            std::vector<char> seen(n, 0);
            std::vector<int> queue{start};
            seen[start] = 1;
            for (std::size_t head = 0; head < queue.size(); ++head) {
                int u = queue[head];
                for (Edge* e = g.get_edges(u); e; e = e->next) {
                    if (!(removed & (1u << e->to)) && !seen[e->to]) {
                        seen[e->to] = 1;
                        queue.push_back(e->to);
                    }
                }
            }
            for (int v = 0; v < n; ++v) if (!(removed & (1u << v)) && !seen[v]) return false;
            return true;
        };
        for (int removed_count = 0; removed_count <= n - 2; ++removed_count) {
            for (unsigned int mask = 0; mask < (1u << n); ++mask) {
                unsigned int bits = mask;
                int bit_count = 0;
                while (bits != 0) { bits &= bits - 1; ++bit_count; }
                if (bit_count != removed_count) continue;
                if (!connected_after_removal(mask)) return removed_count;
            }
        }
        return n - 1;
    }
    return n - 1;
}
}
