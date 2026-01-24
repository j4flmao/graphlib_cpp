include(FetchContent)

find_package(GTest CONFIG)

if(NOT GTest_FOUND)
    message(STATUS "GTest not found, fetching from git...")
    FetchContent_Declare(
      googletest
      GIT_REPOSITORY https://github.com/google/googletest.git
      GIT_TAG v1.14.0
    )
    # For Windows: Prevent overriding the parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
    
    if(NOT TARGET GTest::gtest)
        add_library(GTest::gtest ALIAS gtest)
    endif()
    if(NOT TARGET GTest::gtest_main)
        add_library(GTest::gtest_main ALIAS gtest_main)
    endif()
endif()

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
    test_dominator_tree.cpp
    test_centroid_decomposition.cpp
    test_tree_max_path.cpp
    test_extreme_algo.cpp
    test_directed_mst.cpp
    test_dynamic_connectivity.cpp
    test_extreme_additions_v2.cpp
    test_graph_measures.cpp
    test_planarity.cpp
    test_tree_algo_v2.cpp
    test_np_hard_v2.cpp
    test_graph_measures_v2.cpp
    test_graph_generator.cpp
    test_extreme_upgrade.cpp
    test_extreme_upgrade_v2.cpp
    test_bipartite_matching.cpp
    test_extreme_matching.cpp
    test_chordal.cpp
    test_tree_decomposition.cpp
    test_block_cut_tree.cpp
    test_maximum_weight_closure.cpp
    test_cycle_algo.cpp
    test_maximum_density_subgraph.cpp
    test_min_cost_max_flow.cpp
    test_isomorphism.cpp
    test_k_shortest_paths.cpp
    test_tree_path_queries.cpp
)

    target_link_libraries(graphlib_tests
        PRIVATE
        graphlib
        GTest::gtest
        GTest::gtest_main
    )

include(GoogleTest)
# DISCOVERY_MODE PRE_TEST ensures tests are discovered at runtime, avoiding build-time execution issues on Windows CI
gtest_discover_tests(graphlib_tests DISCOVERY_MODE PRE_TEST)


