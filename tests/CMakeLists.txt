find_package(GTest QUIET)

if(GTest_FOUND)
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
    )

    target_link_libraries(graphlib_tests
        PRIVATE
        graphlib
        GTest::gtest
        GTest::gtest_main
    )

    include(GoogleTest)
    gtest_discover_tests(graphlib_tests)
else()
    message(WARNING "GTest not found, graphlib tests will not be built")
endif()


