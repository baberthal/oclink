include_directories(${CMAKE_CURRENT_LIST_DIR}/../src)

find_package(Check REQUIRED)
include_directories(${CHECK_INCLUDE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(TEST_SOURCES ocl_tests.c)
add_executable(ocl_tests ${TEST_SOURCES})
target_link_libraries(ocl_tests clink ${CHECK_LIBRARIES})
target_compile_definitions(ocl_tests PUBLIC -DDEBUG)
target_compile_options(ocl_tests PUBLIC -g -ggdb -O1)

add_custom_target(run_tests
    ALL
    "${PROJECT_SOURCE_DIR}/script/test-runner" ${CMAKE_CURRENT_BINARY_DIR}
    DEPENDS ocl_tests
    COMMENT "Running all tests..."
    VERBATIM
    USES_TERMINAL)
