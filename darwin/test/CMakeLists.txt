include_directories(${CMAKE_CURRENT_LIST_DIR}/.. ${CMAKE_CURRENT_BINARY_DIR})
find_package(Check REQUIRED)
include_directories(${CHECK_INCLUDE_DIR} ${CMAKE_CURRENT_SOURCE_DIR})

set(TEST_FLAGS -g -O0 -fprofile-instr-generate -fcoverage-mapping -fprofile-arcs -ftest-coverage)

set(TEST_SOURCES
  darwin_tests.c
  darwin_tests.h
  hid_utils_tests.c
  util_tests.c
  main.c
)

add_executable(darwin_tests ${TEST_SOURCES})
target_compile_definitions(darwin_tests PRIVATE -DDEBUG -DCLINK_STATIC_DEFINE)
target_compile_options(darwin_tests PRIVATE ${TEST_FLAGS})
target_link_libraries(darwin_tests ${CHECK_LIBRARIES} libclink_darwin)
set_target_properties(darwin_tests PROPERTIES LINK_FLAGS "--coverage")

SETUP_TARGET_FOR_COVERAGE(coverage-darwin_gcov darwin_tests gcov-darwin)
