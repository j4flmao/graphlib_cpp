#include <iostream>
#include <vector>
#include <graphlib/graphlib.h>
#include <graphlib/two_sat.h>

int main() {
    int n = 3;
    graphlib::TwoSat sat(n);

    sat.add_clause(0, true, 1, true);
    sat.add_clause(0, false, 2, true);
    sat.add_implication(1, false);

    std::vector<bool> assignment;
    bool ok = sat.solve(assignment);

    if (!ok) {
        std::cout << "UNSAT\n";
        return 0;
    }

    std::cout << "SAT\n";
    for (int i = 0; i < n; i++) {
        std::cout << "x" << i << " = " << (assignment[i] ? 1 : 0) << "\n";
    }

    return 0;
}

