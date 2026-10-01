#include "graphlib/combinatorial_algorithms.h"
#include <algorithm>
#include <numeric>
#include <stdexcept>
namespace graphlib {
FunctionalGraphInfo analyze_functional_graph(const std::vector<int>& next) {
    int n=static_cast<int>(next.size()); for(int v:next)if(v<0||v>=n)throw std::out_of_range("Functional-graph successor out of range");
    FunctionalGraphInfo r; r.cycle_id.assign(n,-1); r.distance_to_cycle.assign(n,-1); std::vector<int> state(n),pos(n,-1),stack;
    for(int start=0;start<n;++start) if(!state[start]) { int u=start; while(!state[u]){state[u]=1;pos[u]=static_cast<int>(stack.size());stack.push_back(u);u=next[u];}
        if(state[u]==1 && pos[u]>=0){int cid=static_cast<int>(r.cycle_lengths.size()),len=0;for(int i=pos[u];i<static_cast<int>(stack.size());++i){r.cycle_id[stack[i]]=cid;r.distance_to_cycle[stack[i]]=0;++len;}r.cycle_lengths.push_back(len);}
        while(!stack.empty()){int v=stack.back();stack.pop_back();state[v]=2;pos[v]=-1;if(r.distance_to_cycle[v]==-1){r.cycle_id[v]=r.cycle_id[next[v]];r.distance_to_cycle[v]=r.distance_to_cycle[next[v]]+1;}}
    }
    return r;
}
bool is_graphical_degree_sequence(const std::vector<int>& degrees){for(int d:degrees)if(d<0||d>=static_cast<int>(degrees.size()))return false;long long sum=std::accumulate(degrees.begin(),degrees.end(),0LL);if(sum%2)return false;std::vector<int>d=degrees;std::sort(d.rbegin(),d.rend());while(!d.empty()&&d[0]>0){int k=d[0];d.erase(d.begin());if(k>static_cast<int>(d.size()))return false;for(int i=0;i<k;++i)if(--d[i]<0)return false;std::sort(d.rbegin(),d.rend());}return true;}
std::vector<std::pair<int,int>> realize_degree_sequence(const std::vector<int>& degrees){if(!is_graphical_degree_sequence(degrees))return{};std::vector<std::pair<int,int>> edges;std::vector<std::pair<int,int>> d;for(int i=0;i<static_cast<int>(degrees.size());++i)d.push_back({degrees[i],i});while(true){std::sort(d.rbegin(),d.rend());if(d.empty()||d[0].first==0)break;auto [k,u]=d[0];d.erase(d.begin());if(k>static_cast<int>(d.size()))return{};for(int i=0;i<k;++i){if(d[i].first<=0)return{};edges.push_back({u,d[i].second});--d[i].first;}}return edges;}
}
