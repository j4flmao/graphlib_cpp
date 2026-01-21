cmake_minimum_required(VERSION 3.15)

set(GRAPHLIB_VERSION "1.0.3" CACHE STRING "GraphLib version")
project(GraphLib VERSION ${GRAPHLIB_VERSION} LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(BUILD_SHARED_LIBS "Build shared library (DLL/SO)" ON)
option(BUILD_TESTS "Build test suite" ON)
option(BUILD_EXAMPLES "Build examples" ON)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

set(GRAPHLIB_SOURCES
    src/graph_core.cpp
    src/bipartite.cpp
    src/dag.cpp
    src/max_flow.cpp
    src/mst.cpp
    src/connectivity.cpp
    src/scc.cpp
    src/splay_tree.cpp
    src/version.cpp
    src/shortest_path.cpp
    src/general_matching.cpp
    src/tree.cpp
)

set(GRAPHLIB_HEADERS
    include/graphlib/export.h
    include/graphlib/graph_core.h
    include/graphlib/bipartite.h
    include/graphlib/max_flow.h
    include/graphlib/dag.h
    include/graphlib/shortest_path.h
    include/graphlib/mst.h
    include/graphlib/connectivity.h
    include/graphlib/scc.h
    include/graphlib/splay_tree.h
    include/graphlib/general_matching.h
    include/graphlib/tree.h
    include/graphlib/graphlib.h
)

add_library(graphlib ${GRAPHLIB_SOURCES})

set_target_properties(graphlib PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION 1
    PUBLIC_HEADER "${GRAPHLIB_HEADERS}"
)

target_compile_definitions(graphlib PRIVATE GRAPHLIB_VERSION_STRING="${PROJECT_VERSION}")

if(WIN32)
    target_compile_definitions(graphlib PRIVATE GRAPHLIB_EXPORTS)
    target_compile_definitions(graphlib INTERFACE GRAPHLIB_IMPORTS)
endif()

if(MSVC)
    add_compile_options(/utf-8)
    target_compile_options(graphlib PRIVATE /W4)
else()
    target_compile_options(graphlib PRIVATE -Wall -Wextra -Wpedantic)
endif()

include(GNUInstallDirs)

install(TARGETS graphlib
    EXPORT GraphLibTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/graphlib
)

install(EXPORT GraphLibTargets
    FILE GraphLibTargets.cmake
    NAMESPACE GraphLib::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/GraphLib
)

include(CMakePackageConfigHelpers)
set(GRAPHLIB_INSTALL_CMAKEDIR "${CMAKE_INSTALL_LIBDIR}/cmake/GraphLib")

configure_package_config_file(
    cmake/GraphLibConfig.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/GraphLibConfig.cmake"
    INSTALL_DESTINATION "${GRAPHLIB_INSTALL_CMAKEDIR}"
)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/GraphLibConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/GraphLibConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/GraphLibConfigVersion.cmake"
    DESTINATION "${GRAPHLIB_INSTALL_CMAKEDIR}"
)

if(BUILD_TESTS)
    enable_testing()
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/CMakeLists.txt")
        add_subdirectory(tests)
    endif()
endif()

if(BUILD_EXAMPLES)
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/examples/CMakeLists.txt")
        add_subdirectory(examples)
    endif()
endif()

set(CPACK_PACKAGE_NAME "GraphLib")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "C++ Graph Algorithms Library")
set(CPACK_PACKAGE_VENDOR "GraphLib")
set(CPACK_PACKAGE_CONTACT "Your Name <your.email@example.com>")
set(CPACK_GENERATOR "ZIP;TGZ")
set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${PROJECT_VERSION}-${CMAKE_SYSTEM_NAME}")

include(CPack)
