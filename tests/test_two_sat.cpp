#include <gtest/gtest.h>
#include "graphlib/two_sat.h"

using namespace graphlib;

TEST(TwoSatTest, SimpleSatisfiable) {
    // (x0 OR x1) AND (!x0 OR x2) AND (!x1 OR !x2)
    // Solution: x0=T, x1=F, x2=T ?
    // (T or F) = T.
    // (F or T) = T.
    // (T or F) = T.
    // Yes.
    
    TwoSat sat(3);
    sat.add_clause(0, true, 1, true); // x0 or x1
    sat.add_clause(0, false, 2, true); // !x0 or x2
    sat.add_clause(1, false, 2, false); // !x1 or !x2
    
    std::vector<bool> assign;
    bool result = sat.solve(assign);
    
    EXPECT_TRUE(result);
    // Verify assignment
    bool x0 = assign[0];
    bool x1 = assign[1];
    bool x2 = assign[2];
    
    EXPECT_TRUE((x0 || x1));
    EXPECT_TRUE((!x0 || x2));
    EXPECT_TRUE((!x1 || !x2));
}

TEST(TwoSatTest, Unsatisfiable) {
    // (x0) AND (!x0)
    TwoSat sat(1);
    sat.add_clause(0, true, 0, true);
    sat.add_clause(0, false, 0, false);
    
    std::vector<bool> assign;
    bool result = sat.solve(assign);
    
    EXPECT_FALSE(result);
}

TEST(TwoSatTest, ForceVal) {
    TwoSat sat(2);
    // Force x0 = true
    sat.add_implication(0, true);
    // x0 -> x1
    sat.add_clause(0, false, 1, true);
    
    std::vector<bool> assign;
    EXPECT_TRUE(sat.solve(assign));
    EXPECT_TRUE(assign[0]);
    EXPECT_TRUE(assign[1]);
}
