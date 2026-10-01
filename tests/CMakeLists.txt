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

file(GLOB GRAPHLIB_TEST_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/test_*.cpp")
add_executable(graphlib_tests ${GRAPHLIB_TEST_SOURCES})
target_include_directories(graphlib_tests PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/support"
)

if(GRAPHLIB_BUILD_TOOLS)
    add_executable(debug_matching debug_main.cpp)
    target_link_libraries(debug_matching PRIVATE graphlib)
endif()

target_link_libraries(graphlib_tests
        PRIVATE
        graphlib
        GTest::gtest
        GTest::gtest_main
    )

if(GRAPHLIB_WARNINGS_AS_ERRORS)
    if(MSVC)
        target_compile_options(graphlib_tests PRIVATE /W4 /WX)
    else()
        target_compile_options(graphlib_tests PRIVATE -Wall -Wextra -Wpedantic -Werror)
    endif()
endif()

include(GoogleTest)
# DISCOVERY_MODE PRE_TEST ensures tests are discovered at runtime, avoiding build-time execution issues on Windows CI
gtest_discover_tests(graphlib_tests
    DISCOVERY_MODE PRE_TEST
    PROPERTIES TIMEOUT 30 LABELS unit)


