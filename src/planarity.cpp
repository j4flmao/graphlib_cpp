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

// Demoucron-Malgrange-Pertuiset Algorithm for Planarity Testing
// O(V^2) implementation.

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

// Represents a face in the embedding (sequence of vertices)
using Face = std::vector<int>;

// Find a path between two sets of vertices (contact points) within a fragment
// Returns the path vertices including endpoints.
// used_edges tracks edges already embedded.
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
    while(head < q.size()){
        int u = q[head++];
        
        if (contacts.count(u) && u != start_node) {
            target = u;
            break;
        }

        for (int v : adj[u]) {
            // Check if edge (u,v) is embedded
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
        // Path is target -> start. Reverse it if needed, but for edges it doesn't matter.
        // Let's reverse to be nice.
        std::reverse(path.begin(), path.end());
        return true;
    }
    return false;
}

// Identify fragments
// A fragment is a connected component of G \ G_embedded.
// Plus all edges incident to that component from G_embedded.
// Contact vertices are vertices in the fragment that are already embedded.
struct Fragment {
    std::set<int> vertices; // vertices strictly inside the fragment (not embedded)
    std::set<EdgeRec> edges; // edges belonging to this fragment
    std::set<int> contacts; // vertices in fragment that are already embedded
};

} // namespace

bool is_planar(const Graph& g) {
    int n = g.vertex_count();
    if (n == 0) return true;
    
    // 1. Pre-check Euler
    int m = 0;
    std::vector<std::vector<int>> adj(n);
    for (int i = 0; i < n; ++i) {
        Edge* e = g.get_edges(i);
        while (e) {
            if (e->to > i) {
                m++;
            }
            adj[i].push_back(e->to);
            if (e->to < n) {
                 adj[e->to].push_back(i);
            }
            e = e->next;
        }
    }
    // Remove duplicates in adj
    for(int i=0; i<n; ++i) {
        std::sort(adj[i].begin(), adj[i].end());
        adj[i].erase(std::unique(adj[i].begin(), adj[i].end()), adj[i].end());
    }

    if (n > 4 && m > 3 * n - 6) return false;
    if (n <= 4) return true; // K4 is planar

    // Handle disconnected graphs by processing each component separately
    std::vector<int> global_visited(n, 0);
    
    for (int start_node = 0; start_node < n; ++start_node) {
        if (global_visited[start_node]) continue;

        // Extract component
        std::vector<int> component_nodes;
        std::vector<int> q_bfs;
        q_bfs.push_back(start_node);
        global_visited[start_node] = 1;
        component_nodes.push_back(start_node);
        
        int head = 0;
        while(head < q_bfs.size()){
            int u = q_bfs[head++];
            for(int v : adj[u]){
                if(!global_visited[v]){
                    global_visited[v] = 1;
                    component_nodes.push_back(v);
                    q_bfs.push_back(v);
                }
            }
        }

        // If component is small, it's planar
        if (component_nodes.size() <= 4) continue;

        auto is_planar_component = [&](const std::vector<int>& nodes) -> bool {
            // Count edges in component for Euler check
            int comp_n = (int)nodes.size();
            int comp_m = 0;
            for(int u : nodes) {
                for(int v : adj[u]) {
                    if (u < v) { // Count each edge once
                        comp_m++;
                    }
                }
            }
            if (comp_n > 4 && comp_m > 3 * comp_n - 6) return false;
            if (comp_n <= 4) return true;

            // 2. Find initial cycle
            std::map<int, int> visited_dfs;
            std::map<int, int> parent_dfs;
            for(int u : nodes) { visited_dfs[u] = 0; parent_dfs[u] = -1; }
            
            std::vector<int> cycle;
            
            // Helper for DFS in component
            std::function<bool(int, int)> dfs_cycle = [&](int u, int p) -> bool {
                visited_dfs[u] = 1;
                parent_dfs[u] = p;
                for (int v : adj[u]) {
                    if (v == p) continue;
                    if (visited_dfs[v]) {
                        // Cycle detected
                        int curr = u;
                        while (curr != v) {
                            cycle.push_back(curr);
                            curr = parent_dfs[curr];
                        }
                        cycle.push_back(v);
                        return true;
                    }
                    if (dfs_cycle(v, u)) return true;
                }
                return false;
            };

            if (!dfs_cycle(nodes[0], -1)) {
                return true; // Tree
            }

            // 3. Initial Embedding
            std::vector<Face> faces;
            faces.push_back(cycle);
            faces.push_back(cycle); // Outer face is same as inner initially

            std::set<EdgeRec> embedded_edges;
            std::set<int> embedded_vertices;
            for (size_t i = 0; i < cycle.size(); ++i) {
                int u = cycle[i];
                int v = cycle[(i + 1) % cycle.size()];
                embedded_edges.insert({std::min(u, v), std::max(u, v)});
                embedded_vertices.insert(u);
            }

            // Main Loop
            while (embedded_edges.size() < (size_t)comp_m) {
                // 4. Identify Fragments
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

                        int head = 0;
                        while(head < q_edges.size()) {
                            EdgeRec curr = q_edges[head++];
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

                // 5. Determine Admissible Faces
                int min_admissible = 1e9;
                int best_frag_idx = -1;
                std::vector<std::vector<int>> admissible_faces(fragments.size());

                for (size_t i = 0; i < fragments.size(); ++i) {
                    const auto& frag = fragments[i];
                    for (size_t f_idx = 0; f_idx < faces.size(); ++f_idx) {
                        const auto& face = faces[f_idx];
                        bool all_in = true;
                        for (int c : frag.contacts) {
                            bool found = false;
                            for (int fv : face) {
                                if (fv == c) {
                                    found = true;
                                    break;
                                }
                            }
                            if (!found) {
                                all_in = false;
                                break;
                            }
                        }
                        if (all_in) {
                            admissible_faces[i].push_back((int)f_idx);
                        }
                    }

                    if (admissible_faces[i].empty()) return false;
                    
                    if ((int)admissible_faces[i].size() < min_admissible) {
                        min_admissible = (int)admissible_faces[i].size();
                        best_frag_idx = (int)i;
                    }
                }

                const auto& chosen_frag = fragments[best_frag_idx];
                int chosen_face_idx = admissible_faces[best_frag_idx][0];
                const auto& face = faces[chosen_face_idx];

                // 7. Find Path
                std::vector<int> path;
                if (chosen_frag.contacts.size() >= 2) {
                    int u = *chosen_frag.contacts.begin();
                    find_path_in_fragment(u, chosen_frag.contacts, adj, embedded_edges, path);
                } else {
                        if (chosen_frag.contacts.empty()) {
                            // Should not happen in a connected component with a cycle
                            return false;
                        }
                        int u = *chosen_frag.contacts.begin();
                        for(const auto& e : chosen_frag.edges) {
                            if(e.u == u || e.v == u) {
                                path = {e.u, e.v};
                                break;
                            }
                        }
                }

                // 8. Embed Path
                int u = path.front();
                int v = path.back();

                for (size_t i = 0; i < path.size() - 1; ++i) {
                    int p1 = path[i];
                    int p2 = path[i+1];
                    embedded_edges.insert({std::min(p1, p2), std::max(p1, p2)});
                    embedded_vertices.insert(p1);
                    embedded_vertices.insert(p2);
                }

                int idx_u = -1, idx_v = -1;
                for (size_t i = 0; i < face.size(); ++i) {
                    if (face[i] == u) idx_u = (int)i;
                    if (face[i] == v) idx_v = (int)i;
                }

                std::vector<int> face_path1, face_path2;
                int curr = idx_u;
                while (curr != idx_v) {
                    face_path1.push_back(face[curr]);
                    curr = (curr + 1) % face.size();
                }
                face_path1.push_back(v);

                curr = idx_v;
                while (curr != idx_u) {
                    face_path2.push_back(face[curr]);
                    curr = (curr + 1) % face.size();
                }
                face_path2.push_back(u);

                std::vector<int> p_inner;
                for(size_t i=1; i<path.size()-1; ++i) p_inner.push_back(path[i]);

                std::vector<int> new_face1 = face_path1;
                for(int i=(int)p_inner.size()-1; i>=0; --i) new_face1.push_back(p_inner[i]);

                std::vector<int> new_face2 = face_path2;
                for(size_t i=0; i<p_inner.size(); ++i) new_face2.push_back(p_inner[i]);

                faces[chosen_face_idx] = new_face1;
                faces.push_back(new_face2);
            }
            return true;
        };

        if (!is_planar_component(component_nodes)) return false;
    }

    return true;
}

} // namespace graphlib
