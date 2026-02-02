#include "graphlib/planarity.h"
#include <vector>
#include <list>
#include <algorithm>
#include <set>
#include <map>
#include <stack>
#include <functional>

namespace graphlib {

namespace {

struct EdgeRec {
    int u, v;
    bool operator<(const EdgeRec& other) const {
        if (u != other.u) return u < other.u;
        return v < other.v;
    }
    bool operator==(const EdgeRec& other) const {
        return u == other.u && v == other.v;
    }
};

using Face = std::vector<int>;

bool find_path_in_fragment(
    int start_node,
    const std::set<int>& contacts,
    const std::vector<std::vector<int>>& adj,
    const std::set<EdgeRec>& embedded_edges,
    std::vector<int>& path
) {
    std::vector<int> q;
    std::map<int, int> p;
    std::set<int> visited;
    
    q.push_back(start_node);
    visited.insert(start_node);
    p[start_node] = -1;

    int target = -1;

    int head = 0;
    while(head < static_cast<int>(q.size())){
        int u = q[head++];
        
        if (contacts.count(u) && u != start_node) {
            target = u;
            break;
        }

        for (int v : adj[u]) {
            EdgeRec e1 = {std::min(u,v), std::max(u,v)};
            if (embedded_edges.count(e1)) continue;

            if (visited.find(v) == visited.end()) {
                visited.insert(v);
                p[v] = u;
                q.push_back(v);
            }
        }
    }

    if (target != -1) {
        int curr = target;
        while (curr != -1) {
            path.push_back(curr);
            curr = p[curr];
        }
        std::reverse(path.begin(), path.end());
        return true;
    }
    return false;
}

struct Fragment {
    std::set<int> vertices;
    std::set<EdgeRec> edges;
    std::set<int> contacts;
};

} // namespace

bool is_planar(const Graph& g) {
    int n = g.vertex_count();
    if (n <= 4) return true;
    
    // Count edges
    int m = 0;
    for (int i = 0; i < n; ++i) {
        for (Edge* e = g.get_edges(i); e; e = e->next) {
            if (e->to > i) m++;
        }
    }
    
    // Euler's formula bound
    if (m > 3 * n - 6) return false;
    
    // For small graphs, use the full embedding algorithm
    // For now, use a simplified check
    auto faces = get_planar_faces(g);
    if (g.vertex_count() > 0 && faces.empty()) return false;
    return true;
}

std::vector<std::vector<int>> get_planar_faces(const Graph& g) {
    int n = g.vertex_count();
    if (n == 0) return {};
    
    // Count edges and build adjacency
    int m = 0;
    std::vector<std::vector<int>> adj(n);
    for (int i = 0; i < n; ++i) {
        Edge* e = g.get_edges(i);
        while (e) {
            if (e->to > i) {
                m++;
            }
            adj[i].push_back(e->to);
            e = e->next;
        }
    }
    
    // Remove duplicates in adj
    for(int i = 0; i < n; ++i) {
        std::sort(adj[i].begin(), adj[i].end());
        adj[i].erase(std::unique(adj[i].begin(), adj[i].end()), adj[i].end());
    }

    if (n > 4 && m > 3 * n - 6) return {};

    std::vector<int> global_visited(n, 0);
    std::vector<std::vector<int>> all_faces;
    
    for (int start_node = 0; start_node < n; ++start_node) {
        if (global_visited[start_node]) continue;

        // Extract component via BFS
        std::vector<int> component_nodes;
        std::vector<int> q_bfs;
        q_bfs.push_back(start_node);
        global_visited[start_node] = 1;
        component_nodes.push_back(start_node);
        
        int head = 0;
        while(head < static_cast<int>(q_bfs.size())){
            int u = q_bfs[head++];
            for(int v : adj[u]){
                if(!global_visited[v]){
                    global_visited[v] = 1;
                    component_nodes.push_back(v);
                    q_bfs.push_back(v);
                }
            }
        }

        // Component embedding lambda
        auto component_embedding = [&adj](const std::vector<int>& nodes) -> std::vector<Face> {
            int comp_n = static_cast<int>(nodes.size());
            
            // Count edges in component
            int comp_m = 0;
            for(int u : nodes) {
                for(int v : adj[u]) {
                    if (u < v) comp_m++;
                }
            }
            if (comp_n > 4 && comp_m > 3 * comp_n - 6) return {};

            // Find initial cycle using DFS
            std::map<int, int> visited_dfs;
            std::map<int, int> parent_dfs;
            for(int u : nodes) { 
                visited_dfs[u] = 0; 
                parent_dfs[u] = -1; 
            }
            
            std::vector<int> cycle;
            
            std::function<bool(int, int)> dfs_cycle = [&](int u, int p) -> bool {
                visited_dfs[u] = 1;
                parent_dfs[u] = p;
                for (int v : adj[u]) {
                    if (v == p) continue;
                    if (visited_dfs.count(v) == 0) continue; // Not in component
                    if (visited_dfs[v]) {
                        int curr = u;
                        while (curr != v) {
                            cycle.push_back(curr);
                            curr = parent_dfs[curr];
                            if (curr == -1) break;
                        }
                        cycle.push_back(v);
                        return true;
                    }
                    if (dfs_cycle(v, u)) return true;
                }
                return false;
            };

            if (!dfs_cycle(nodes[0], -1)) {
                // No cycle = Tree, planar
                std::vector<int> outer_face = nodes;
                return {outer_face};
            }

            // Initial Embedding
            std::vector<Face> faces;
            faces.push_back(cycle);
            std::vector<int> outer = cycle;
            std::reverse(outer.begin(), outer.end());
            faces.push_back(outer);

            std::set<EdgeRec> embedded_edges;
            std::set<int> embedded_vertices;
            for (size_t i = 0; i < cycle.size(); ++i) {
                int u = cycle[i];
                int v = cycle[(i + 1) % cycle.size()];
                embedded_edges.insert({std::min(u, v), std::max(u, v)});
                embedded_vertices.insert(u);
            }

            // Main Loop - embed remaining edges
            while (static_cast<int>(embedded_edges.size()) < comp_m) {
                std::vector<Fragment> fragments;
                std::set<EdgeRec> visited_edges;

                for (int i : nodes) {
                    for (int v : adj[i]) {
                        if (i > v) continue;
                        EdgeRec e = {i, v};
                        if (embedded_edges.count(e)) continue;
                        if (visited_edges.count(e)) continue;

                        Fragment frag;
                        std::vector<EdgeRec> q_edges;
                        q_edges.push_back(e);
                        visited_edges.insert(e);
                        frag.edges.insert(e);

                        int edge_head = 0;
                        while(edge_head < static_cast<int>(q_edges.size())) {
                            EdgeRec curr = q_edges[edge_head++];
                            int u1 = curr.u;
                            int v1 = curr.v;

                            if (embedded_vertices.count(u1)) frag.contacts.insert(u1);
                            else frag.vertices.insert(u1);

                            if (embedded_vertices.count(v1)) frag.contacts.insert(v1);
                            else frag.vertices.insert(v1);

                            auto process_node = [&](int node) {
                                if (!embedded_vertices.count(node)) {
                                    for (int neighbor : adj[node]) {
                                        EdgeRec next_e = {std::min(node, neighbor), std::max(node, neighbor)};
                                        if (!embedded_edges.count(next_e) && !visited_edges.count(next_e)) {
                                            visited_edges.insert(next_e);
                                            frag.edges.insert(next_e);
                                            q_edges.push_back(next_e);
                                        }
                                    }
                                }
                            };

                            if (!embedded_vertices.count(u1)) process_node(u1);
                            if (!embedded_vertices.count(v1)) process_node(v1);
                        }
                        fragments.push_back(frag);
                    }
                }

                if (fragments.empty()) break;

                // Determine Admissible Faces
                int min_admissible = 1000000000;
                int best_frag_idx = -1;
                std::vector<std::vector<int>> admissible_faces(fragments.size());

                for (size_t i = 0; i < fragments.size(); ++i) {
                    const auto& frag = fragments[i];
                    for (size_t f_idx = 0; f_idx < faces.size(); ++f_idx) {
                        const auto& face = faces[f_idx];
                        bool all_in = true;
                        
                        std::set<int> face_set(face.begin(), face.end());
                        for (int c : frag.contacts) {
                            if (face_set.find(c) == face_set.end()) {
                                all_in = false;
                                break;
                            }
                        }
                        if (all_in) {
                            admissible_faces[i].push_back(static_cast<int>(f_idx));
                        }
                    }

                    if (admissible_faces[i].empty()) return {}; // Non-planar
                    
                    if (static_cast<int>(admissible_faces[i].size()) < min_admissible) {
                        min_admissible = static_cast<int>(admissible_faces[i].size());
                        best_frag_idx = static_cast<int>(i);
                    }
                }

                const auto& chosen_frag = fragments[best_frag_idx];
                int chosen_face_idx = admissible_faces[best_frag_idx][0];
                const auto& face = faces[chosen_face_idx];

                // Find Path
                std::vector<int> path;
                if (chosen_frag.contacts.size() >= 2) {
                    int u = *chosen_frag.contacts.begin();
                    find_path_in_fragment(u, chosen_frag.contacts, adj, embedded_edges, path);
                } else {
                    if (chosen_frag.contacts.empty()) {
                        return {};
                    }
                    int u = *chosen_frag.contacts.begin();
                    for(const auto& edge : chosen_frag.edges) {
                        if(edge.u == u || edge.v == u) {
                            int other = (edge.u == u) ? edge.v : edge.u;
                            path = {u, other};
                            break;
                        }
                    }
                }

                // Embed Path
                int path_u = path.front();
                int path_v = path.back();

                for (size_t i = 0; i < path.size() - 1; ++i) {
                    int p1 = path[i];
                    int p2 = path[i+1];
                    embedded_edges.insert({std::min(p1, p2), std::max(p1, p2)});
                    embedded_vertices.insert(p1);
                    embedded_vertices.insert(p2);
                }

                int idx_u = -1, idx_v = -1;
                for (size_t i = 0; i < face.size(); ++i) {
                    if (face[i] == path_u) idx_u = static_cast<int>(i);
                    if (face[i] == path_v) idx_v = static_cast<int>(i);
                }

                std::vector<int> face_path1, face_path2;
                int curr_idx = idx_u;
                while (curr_idx != idx_v) {
                    face_path1.push_back(face[curr_idx]);
                    curr_idx = (curr_idx + 1) % static_cast<int>(face.size());
                }
                face_path1.push_back(path_v);

                curr_idx = idx_v;
                while (curr_idx != idx_u) {
                    face_path2.push_back(face[curr_idx]);
                    curr_idx = (curr_idx + 1) % static_cast<int>(face.size());
                }
                face_path2.push_back(path_u);

                std::vector<int> p_inner;
                for(size_t i = 1; i < path.size() - 1; ++i) {
                    p_inner.push_back(path[i]);
                }

                // Split face into two
                std::vector<int> new_face1 = face_path1;
                for(int i = static_cast<int>(p_inner.size()) - 1; i >= 0; --i) {
                    new_face1.push_back(p_inner[i]);
                }

                std::vector<int> new_face2 = face_path2;
                for(size_t i = 0; i < p_inner.size(); ++i) {
                    new_face2.push_back(p_inner[i]);
                }

                faces[chosen_face_idx] = new_face1;
                faces.push_back(new_face2);
            }
            return faces;
        };
        
        auto faces = component_embedding(component_nodes);
        if (faces.empty()) return {}; // Component not planar
        all_faces.insert(all_faces.end(), faces.begin(), faces.end());
    }
    
    return all_faces;
}

} // namespace graphlib
