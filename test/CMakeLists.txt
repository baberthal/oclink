include_directories(${CMAKE_CURRENT_LIST_DIR}/../src ${PROJECT_BINARY_DIR}/src)

find_package(Check REQUIRED)
include_directories(${CHECK_INCLUDE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(TEST_FLAGS -g -O0)

include(CheckCCompilerFlag)
check_c_compiler_flag(-glldb HAVE_G_LLDB)
check_c_compiler_flag(-ggdb HAVE_G_GDB)
if(HAVE_G_LLDB)
    list(APPEND TEST_FLAGS -glldb)
elseif(HAVE_G_GDB)
    list(APPEND TEST_FLAGS -ggdb)
endif(HAVE_G_LLDB)

set(TEST_SOURCES ocl_tests.c)
add_executable(ocl_tests ${TEST_SOURCES})
target_link_libraries(ocl_tests libclink ${CHECK_LIBRARIES})
target_compile_definitions(ocl_tests PUBLIC -DDEBUG)
target_compile_options(ocl_tests PUBLIC ${TEST_FLAGS})

add_executable(util_tests util_tests.c)
target_link_libraries(util_tests libclink ${CHECK_LIBRARIES})
target_compile_definitions(util_tests PUBLIC -DDEBUG)
target_compile_options(util_tests PUBLIC ${TEST_FLAGS})

add_custom_target(run_tests
    ALL
    "${PROJECT_SOURCE_DIR}/script/test-runner" ${CMAKE_CURRENT_BINARY_DIR}
    --log-file ${CMAKE_CURRENT_BINARY_DIR}/test.log
    DEPENDS ocl_tests
    COMMENT "Running all tests..."
    VERBATIM
    USES_TERMINAL)
