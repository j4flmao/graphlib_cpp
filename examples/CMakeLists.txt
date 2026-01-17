add_executable(example_max_flow example_max_flow.cpp)
target_link_libraries(example_max_flow PRIVATE graphlib)

add_executable(example_splay_tree example_splay_tree.cpp)
target_link_libraries(example_splay_tree PRIVATE graphlib)

add_executable(example_graph_core example_graph_core.cpp)
target_link_libraries(example_graph_core PRIVATE graphlib)

add_executable(example_bipartite example_bipartite.cpp)
target_link_libraries(example_bipartite PRIVATE graphlib)

add_executable(example_dag example_dag.cpp)
target_link_libraries(example_dag PRIVATE graphlib)

add_executable(example_min_cost_flow example_min_cost_flow.cpp)
target_link_libraries(example_min_cost_flow PRIVATE graphlib)

add_executable(example_general_matching example_general_matching.cpp)
target_link_libraries(example_general_matching PRIVATE graphlib)

add_executable(example_johnson example_johnson.cpp)
target_link_libraries(example_johnson PRIVATE graphlib)

add_executable(example_lca example_lca.cpp)
target_link_libraries(example_lca PRIVATE graphlib)

add_executable(example_twosat example_twosat.cpp)
target_link_libraries(example_twosat PRIVATE graphlib)

add_custom_command(TARGET example_max_flow POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:graphlib>
        $<TARGET_FILE_DIR:example_max_flow>
)

add_custom_command(TARGET example_splay_tree POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:graphlib>
        $<TARGET_FILE_DIR:example_splay_tree>
)

foreach(exe IN ITEMS
    example_min_cost_flow
    example_general_matching
    example_johnson
    example_graph_core
    example_bipartite
    example_dag
    example_lca
    example_twosat
)
    add_custom_command(TARGET ${exe} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:graphlib>
            $<TARGET_FILE_DIR:${exe}>
    )
endforeach()
