#include "graphlib/general_matching.h"
#include <iostream>
#include <vector>

int main() {
    int n = 40;
    graphlib::GeneralMatching gm(n);
    for(int i=0; i<n; ++i) {
        int next = (i + 1) % n;
        int w = (i % 2 == 0) ? 10 : 1;
        gm.add_edge(i, next, w);
        // std::cout << "Edge " << i << "-" << next << " w=" << w << std::endl;
    }
    gm.add_edge(0, 10, 1);
    gm.add_edge(5, 15, 1);

    std::cout << "Starting matching..." << std::endl;
    long long res = gm.maximum_weight_matching();
    std::cout << "Result: " << res << std::endl;
    
    if (res == 200) {
        std::cout << "SUCCESS" << std::endl;
    } else {
        std::cout << "FAILURE: Expected 200, got " << res << std::endl;
    }
    return 0;
}