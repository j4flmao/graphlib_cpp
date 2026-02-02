
#include "graphlib/general_matching.h"
#include <iostream>
#include <cassert>

using namespace graphlib;

void test_weighted_large_cycle() {
    std::cout << "Testing Weighted Large Cycle..." << std::endl;
    int n = 40;
    GeneralMatching gm(n);
    for(int i=0; i<n; ++i) {
        int next = (i + 1) % n;
        int w = (i % 2 == 0) ? 10 : 1;
        gm.add_edge(i, next, w);
    }
    gm.add_edge(0, 10, 1);
    gm.add_edge(5, 15, 1);
    
    long long res = gm.maximum_weight_matching();
    std::cout << "Result: " << res << " Expected: 200" << std::endl;
    if (res == 200) {
        std::cout << "PASS" << std::endl;
    } else {
        std::cout << "FAIL" << std::endl;
    }
}

int main() {
    test_weighted_large_cycle();
    return 0;
}
