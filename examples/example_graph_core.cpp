#include <graphlib/graphlib.h>
#include <iostream>
#include <queue>
#include <vector>

int main() {
    int n = 6;
    graphlib::Graph g(n, false);

    g.add_edge(0, 1);
    g.add_edge(1, 2);
    g.add_edge(2, 3);
    g.add_edge(3, 4);
    g.add_edge(4, 5);

    bool connected = graphlib::is_connected(g);
    bool tree = graphlib::is_tree(g);

    std::cout << "Connected: " << (connected ? "true" : "false") << "\n";
    std::cout << "Is tree: " << (tree ? "true" : "false") << "\n";

    std::vector<int> dist(n, -1);
    std::queue<int> q;
    int start = 0;
    dist[start] = 0;
    q.push(start);

    while (!q.empty()) {
        int u = q.front();
        q.pop();
        for (graphlib::Edge* e = g.get_edges(u); e != nullptr; e = e->next) {
            int v = e->to;
            if (dist[v] == -1) {
                dist[v] = dist[u] + 1;
                q.push(v);
            }
        }
    }

    std::cout << "BFS distances from 0:\n";
    for (int i = 0; i < n; ++i) {
        std::cout << "dist[" << i << "] = " << dist[i] << "\n";
    }

    return 0;
}

