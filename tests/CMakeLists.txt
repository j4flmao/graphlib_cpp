find_package(GTest CONFIG REQUIRED)

add_executable(graphlib_tests
    test_max_flow.cpp
        test_bipartite.cpp
        test_dag.cpp
        test_splay_tree.cpp
        test_scc_and_connectivity.cpp
        test_shortest_path_and_utils.cpp
        test_general_matching.cpp
        test_circulation.cpp
        test_tree.cpp
        test_mst.cpp
        test_shortest_path_extra.cpp
    test_advanced_cases.cpp
    test_comprehensive.cpp
    test_full_coverage.cpp
    test_randomized_differential.cpp
    test_problems.cpp
    test_extreme_cases.cpp
    test_integration.cpp
    test_algo_specifics.cpp
    test_robustness.cpp
    test_invariants.cpp
    test_scale.cpp
    test_extended_coverage.cpp
    test_concurrency.cpp
    test_connectivity_extended.cpp
    test_structure_robustness.cpp
    test_advanced_algorithms_2.cpp
    test_negative_cost_flow.cpp
    test_complex_scenarios.cpp
    test_all_cases_coverage.cpp
    test_advanced_shortest_path.cpp
    test_eulerian.cpp
    test_weighted_matching.cpp
    test_invariants_extended.cpp
    test_advanced_structures.cpp
    test_problems_hard.cpp
    test_advanced_tree.cpp
    test_np_hard_extra.cpp
    test_advanced_dag_tree.cpp
    test_graph_theory_full.cpp
    test_problems_hard_extra.cpp
    test_bipartite_matching_extra.cpp
    test_stress_combined.cpp
    test_comprehensive_final.cpp
)

    target_link_libraries(graphlib_tests
        PRIVATE
        graphlib
        GTest::gtest
        GTest::gtest_main
    )

include(GoogleTest)
gtest_discover_tests(graphlib_tests)


