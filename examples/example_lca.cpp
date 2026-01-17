#include <graphlib/graphlib.h>
#include <iostream>
#include <vector>

int main() {
    using graphlib::TreeLCA;

    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int n;
    int q;
    if (!(std::cin >> n >> q)) {
        return 0;
    }

    TreeLCA tree(n);

    for (int i = 0; i < n - 1; i++) {
        int u;
        int v;
        std::cin >> u >> v;
        u--;
        v--;
        tree.add_edge(u, v);
    }

    tree.build(0);

    while (q--) {
        int u;
        int v;
        std::cin >> u >> v;
        u--;
        v--;
        int w = tree.lca(u, v);
        int d = tree.distance(u, v);
        if (w < 0) {
            std::cout << "-1 -1\n";
        } else {
            std::cout << (w + 1) << " " << d << "\n";
        }
    }

    return 0;
}

