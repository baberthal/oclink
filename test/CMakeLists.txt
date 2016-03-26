include_directories(${CMAKE_CURRENT_LIST_DIR}/../src ${PROJECT_BINARY_DIR}/src)

find_package(Check REQUIRED)
include_directories(${CHECK_INCLUDE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(TEST_FLAGS -g -O0 -fprofile-instr-generate -fcoverage-mapping -fprofile-arcs -ftest-coverage)

include(CheckCCompilerFlag)
check_c_compiler_flag(-glldb HAVE_G_LLDB)
check_c_compiler_flag(-ggdb HAVE_G_GDB)
if(HAVE_G_LLDB)
    list(APPEND TEST_FLAGS -glldb)
elseif(HAVE_G_GDB)
    list(APPEND TEST_FLAGS -ggdb)
endif(HAVE_G_LLDB)

set(TEST_SOURCES
    ../src/clink.h
    ../src/proto.h
    ../src/fan.h
    ../src/led.h
    ../src/link.h
    ../src/temp.h
    ../src/util.h
    ../src/fan.c
    ../src/led.c
    ../src/link.c
    ../src/temp.c
    ../src/util.c
    ocl_tests.c
    ocl_tests.h
    fan_tests.c
    led_tests.c
    util_tests.c
    temp_tests.c
    main.c)

add_executable(ocl_tests ${TEST_SOURCES})
target_compile_definitions(ocl_tests PRIVATE -DDEBUG -DDEBUG_HID -DCLINK_STATIC_DEFINE)
target_compile_options(ocl_tests PRIVATE ${TEST_FLAGS})
target_link_libraries(ocl_tests ${CHECK_LIBRARIES} ${HIDAPI_LIBRARIES})
set_target_properties(ocl_tests PROPERTIES LINK_FLAGS "--coverage")

SETUP_TARGET_FOR_COVERAGE(coverage-ocl_gcov ocl_tests gcov)
add_llvm_coverage_target(ocl_tests
    ${CMAKE_CURRENT_BINARY_DIR} "${TEST_SOURCES}")

add_custom_target(run_tests
    "${PROJECT_SOURCE_DIR}/script/test-runner" ${CMAKE_CURRENT_BINARY_DIR}
    --log-file ${CMAKE_CURRENT_BINARY_DIR}/test.log
    DEPENDS ocl_tests
    COMMENT "Running all tests..."
    VERBATIM
    USES_TERMINAL)
