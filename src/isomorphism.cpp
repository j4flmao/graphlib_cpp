#include "graphlib/isomorphism.h"
#include <algorithm>
#include <vector>
#include <numeric>

namespace graphlib {

namespace {

long long count_edges(const Graph& g) {
    long long count = 0;
    for (int i = 0; i < g.vertex_count(); ++i) {
        for (Edge* e = g.get_edges(i); e != nullptr; e = e->next) {
            count++;
        }
    }
    // Graph stores directed edges.
    // If undirected, each edge is stored twice.
    // get_num_edges() usually returns number of edges.
    // If graph is undirected, internal representation has 2*E edges.
    // But API usually expects E.
    // However, for isomorphism check, comparing total internal edges is fine.
    return count;
}

// Helper class for VF2 algorithm state
class VF2State {
public:
    VF2State(const Graph& g1, const Graph& g2, bool induced_subgraph_check = false)
        : g1_(g1), g2_(g2), n1_(g1.vertex_count()), n2_(g2.vertex_count()),
          induced_(induced_subgraph_check) {
        
        core_1_.assign(n1_, -1);
        core_2_.assign(n2_, -1);
        
        in_1_.assign(n1_, 0);
        in_2_.assign(n2_, 0);
        
        t1_len_ = 0;
        t2_len_ = 0;
    }

    bool is_feasible(int u, int v) {
        // 1. Check edges with already mapped nodes
        for (int i = 0; i < n1_; ++i) {
            if (core_1_[i] != -1) {
                int u_prev = i;
                int v_prev = core_1_[i];
                
                // Check forward edges
                bool edge_1_fwd = has_edge(g1_, u, u_prev);
                bool edge_2_fwd = has_edge(g2_, v, v_prev);
                
                if (induced_) {
                    if (edge_1_fwd != edge_2_fwd) return false;
                } else {
                    if (edge_1_fwd && !edge_2_fwd) return false;
                }

                // Check backward edges
                bool edge_1_bwd = has_edge(g1_, u_prev, u);
                bool edge_2_bwd = has_edge(g2_, v_prev, v);
                
                if (induced_) {
                    if (edge_1_bwd != edge_2_bwd) return false;
                } else {
                    if (edge_1_bwd && !edge_2_bwd) return false;
                }
            }
        }

        // 2. Look-ahead / Semantic Feasibility (Degrees, etc.)
        int term_out_1 = 0, term_out_2 = 0;
        int new_out_1 = 0, new_out_2 = 0;
        
        // Count neighbors of u in G1 classified by state
        for (Edge* e = g1_.get_edges(u); e != nullptr; e = e->next) {
            int neighbor = e->to;
            if (neighbor == u) continue;
            if (core_1_[neighbor] != -1) continue;
            
            if (in_1_[neighbor] > 0) {
                term_out_1++;
            } else {
                new_out_1++;
            }
        }
        
        // Count neighbors of v in G2
        for (Edge* e = g2_.get_edges(v); e != nullptr; e = e->next) {
            int neighbor = e->to;
            if (neighbor == v) continue;
            if (core_2_[neighbor] != -1) continue;
            
            if (in_2_[neighbor] > 0) {
                term_out_2++;
            } else {
                new_out_2++;
            }
        }

        if (induced_) {
             if (term_out_1 != term_out_2) return false;
             if (new_out_1 != new_out_2) return false;
        } else {
             if (term_out_1 > term_out_2) return false;
             if (new_out_1 > new_out_2) return false;
        }

        return true;
    }

    void add_pair(int u, int v) {
        core_1_[u] = v;
        core_2_[v] = u;
        
        // Update In/Terminal sets
        if (in_1_[u] == 0) {
            in_1_[u] = depth_;
            t1_len_++;
        }
        if (in_2_[v] == 0) {
            in_2_[v] = depth_;
            t2_len_++;
        }
        
        // Add neighbors to Terminal sets
        for (Edge* e = g1_.get_edges(u); e != nullptr; e = e->next) {
            int neighbor = e->to;
            if (in_1_[neighbor] == 0) {
                in_1_[neighbor] = depth_;
                t1_len_++;
            }
        }
        for (Edge* e = g2_.get_edges(v); e != nullptr; e = e->next) {
            int neighbor = e->to;
            if (in_2_[neighbor] == 0) {
                in_2_[neighbor] = depth_;
                t2_len_++;
            }
        }
        depth_++;
    }

    void backtrack_pair(int u, int v) {
        depth_--;
        core_1_[u] = -1;
        core_2_[v] = -1;
        
        auto restore_in = [&](std::vector<int>& in_vec, int& t_len, const Graph& g, int node) {
            if (in_vec[node] == depth_) {
                in_vec[node] = 0;
                t_len--;
            }
            for (Edge* e = g.get_edges(node); e != nullptr; e = e->next) {
                int neighbor = e->to;
                if (in_vec[neighbor] == depth_) {
                    in_vec[neighbor] = 0;
                    t_len--;
                }
            }
        };

        restore_in(in_1_, t1_len_, g1_, u);
        restore_in(in_2_, t2_len_, g2_, v);
    }
    
    int next_pair_u() {
        int best_u = -1;
        
        // 1. Try to find u in T1
        for (int i = 0; i < n1_; ++i) {
            if (core_1_[i] == -1 && in_1_[i] > 0) {
                best_u = i;
                break;
            }
        }
        
        // 2. If T1 is empty, pick any uncovered node
        if (best_u == -1) {
            for (int i = 0; i < n1_; ++i) {
                if (core_1_[i] == -1) {
                    best_u = i;
                    break;
                }
            }
        }
        
        return best_u;
    }

    const std::vector<int>& get_mapping() const { return core_1_; }

protected:
    const Graph& g1_;
    const Graph& g2_;
    int n1_, n2_;
    bool induced_;
    
    std::vector<int> core_1_;
    std::vector<int> core_2_;
    
    std::vector<int> in_1_;
    std::vector<int> in_2_;
    
    int t1_len_;
    int t2_len_;
    
    int depth_ = 1;

    bool has_edge(const Graph& g, int from, int to) {
        for (Edge* e = g.get_edges(from); e != nullptr; e = e->next) {
            if (e->to == to) return true;
        }
        return false;
    }
};

} // namespace

class VF2Solver : public VF2State {
public:
    VF2Solver(const Graph& g1, const Graph& g2, bool induced) 
        : VF2State(g1, g2, induced) {}

    bool solve(std::vector<std::vector<int>>* all_mappings, bool find_all) {
        int u = next_pair_u();
        
        if (u == -1) {
            if (all_mappings) {
                all_mappings->push_back(get_mapping());
            }
            return true;
        }
        
        bool found_any = false;
        
        for (int v = 0; v < n2_; ++v) {
            if (core_2_[v] == -1) {
                if (is_feasible(u, v)) {
                    add_pair(u, v);
                    
                    bool res = solve(all_mappings, find_all);
                    
                    if (res && !find_all) return true;
                    if (res) found_any = true;
                    
                    backtrack_pair(u, v);
                }
            }
        }
        
        return found_any;
    }
};

GRAPHLIB_API bool is_isomorphic(const Graph& pattern, const Graph& target, std::vector<int>* mapping) {
    if (pattern.vertex_count() != target.vertex_count()) return false;
    if (count_edges(pattern) != count_edges(target)) return false;
    
    VF2Solver solver(pattern, target, true);
    
    std::vector<std::vector<int>> mappings;
    bool found = solver.solve(&mappings, false);
    
    if (found && mapping) {
        *mapping = mappings[0];
    }
    return found;
}

GRAPHLIB_API bool is_subgraph_isomorphic(const Graph& pattern, const Graph& target, std::vector<int>* mapping) {
    if (pattern.vertex_count() > target.vertex_count()) return false;
    if (count_edges(pattern) > count_edges(target)) return false;
    
    VF2Solver solver(pattern, target, false);
    
    std::vector<std::vector<int>> mappings;
    bool found = solver.solve(&mappings, false);
    
    if (found && mapping) {
        *mapping = mappings[0];
    }
    return found;
}

GRAPHLIB_API std::vector<std::vector<int>> find_all_subgraph_isomorphisms(const Graph& pattern, const Graph& target) {
    std::vector<std::vector<int>> mappings;
    if (pattern.vertex_count() > target.vertex_count()) return mappings;
    
    VF2Solver solver(pattern, target, false);
    solver.solve(&mappings, true);
    
    return mappings;
}

} // namespace graphlib
