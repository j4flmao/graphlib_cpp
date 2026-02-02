#ifndef GRAPHLIB_TWO_SAT_H
#define GRAPHLIB_TWO_SAT_H

#include "export.h"
#include <vector>

namespace graphlib {

/**
 * @brief 2-Satisfiability Solver.
 * 
 * Solves a system of boolean clauses of the form (a OR b).
 * Variables are 0-indexed: 0, 1, ..., n-1.
 */
class GRAPHLIB_API TwoSat {
public:
    explicit TwoSat(int n);
    
    // add_clause(i, val_i, j, val_j) corresponds to: (x_i == val_i OR x_j == val_j)
    // val is true/false.
    // Equivalent to: (!val_i -> val_j) AND (!val_j -> val_i)
    void add_clause(int i, bool val_i, int j, bool val_j);
    
    // Adds (x_i == val_i)
    void add_implication(int i, bool val_i);

    // Solves the 2-SAT problem.
    // Returns true if satisfiable, false otherwise.
    // If satisfiable, 'assignment' is filled with the satisfying assignment (true/false for each variable).
    bool solve(std::vector<bool>& assignment);

private:
    int n_;
    // Internal graph has 2*n nodes.
    // 2*i represents x_i (False) ? No.
    // Let's use:
    // 2*i     : x_i is False
    // 2*i + 1 : x_i is True
    struct SimpleEdge {
        int u, v;
    };
    std::vector<SimpleEdge> edges_;
};

}

#endif
