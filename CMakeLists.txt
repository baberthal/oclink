# CMake build script for OpenCorsairLink
#
# Building (out of source tree):
# > mkdir build && cd build
# > cmake .. [-DSETTINGS=VALUE]
# > cmake --build .
#
# Testing:
# > ctest -V
#
# Install:
# > cmake --build . --target install

cmake_minimum_required(VERSION 3.0)
project(clink VERSION 0.2.0)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)

include(MacroEnsureOutOfSourceBuild)
macro_ensure_out_of_source_build("building in the source tree is not allowed.")

# Build options
#
option(BUILD_SHARED_LIBS "Build Shared Library (OFF for static)" ON)
option(TAGS "Generate tags" OFF)
option(PROFILE "Generate profiling information" OFF)
option(VALGRIND "Configure build for valgrind" OFF)
option(BUILD_DOCUMENTATION "Build the doxygen documentation" ON)
option(OCL_TESTS "Build all test targets and enable testing" ON)

if(OCL_TESTS)
    option(OCL_COVERAGE "Build targets with code coverage enabled" ON)
endif(OCL_TESTS)

if(OCL_COVERAGE)
    include(LLVMCoverage)
endif(OCL_COVERAGE)

set(PROJECT_NAME "clink")

set(PROJECT_VERSION_MAJOR 0 CACHE INTERNAL "Project major version.")
set(PROJECT_VERSION_MINOR 2 CACHE INTERNAL "Project minor version.")
set(PROJECT_VERSION_PATCH 0 CACHE INTERNAL "Project teeny version.")
set(PROJECT_VERSION
    ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH})

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

set(OCL_WARNING_FLAGS "-Wall -Wextra -Wno-gnu-zero-variadic-macro-arguments")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pedantic -std=c99 ${OCL_WARNING_FLAGS}")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g -ggdb -O0")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -pg -O2")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-DDEBUG)
else(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-DNDEBUG)
endif(CMAKE_BUILD_TYPE STREQUAL "Debug")

if(APPLE)
    set(CMAKE_MACOSX_RPATH ON)
endif(APPLE)

set(OCL_UTILS_DIR ${CMAKE_CURRENT_LIST_DIR}/debug)
set(OCL_SCRIPT_DIR ${CMAKE_CURRENT_LIST_DIR}/script)
set(HIDAPI_PREFIX "/usr/local" CACHE PATH "prefix where hidapi is installed")

find_package(HIDAPI REQUIRED)
include_directories(${HIDAPI_INCLUDE_DIRS})

include_directories(${CMAKE_CURRENT_BINARY_DIR} src)

add_subdirectory(src)

if(OCL_TESTS)
    include_directories(test)
    add_subdirectory(test)
    enable_testing()

    add_test(NAME util_test_suite COMMAND util_tests)
    add_test(NAME ocl_test_suite COMMAND ocl_tests)
endif(OCL_TESTS)

if(TAGS)
    find_program(CTAGS ctags)
    if(NOT CTAGS)
        message(FATAL_ERROR "Could not find ctags command")
    endif(NOT CTAGS)

    file(GLOB_RECURSE SRC_ALL *.[ch])

    add_custom_command(
        OUTPUT tags
        COMMAND ${CTAGS} -a ${SRC_ALL}
        DEPENDS ${SRC_ALL}
        )

    add_custom_target(
        do_tags ALL
        DEPENDS tags
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_BINARY_DIR}/tags
        ${PROJECT_SOURCE_DIR}/.git/tags
        )
endif(TAGS)

if(BUILD_DOCUMENTATION)
    add_subdirectory(doc)
endif(BUILD_DOCUMENTATION)

include(JMLMessage)
jml_separator()
jml_message(BoldYellow "Configuration Summary:")
message(STATUS "${BoldBlue}Project${ColorReset} ${Cyan}${PROJECT_NAME}${ColorReset} ${BoldBlue}Version${ColorReset} ${Cyan}${PROJECT_VERSION}${ColorReset}")
jml_config_summary_message(CMAKE_BUILD_TYPE)
jml_config_summary_message(CMAKE_INSTALL_PREFIX)
jml_config_summary_message(BUILD_SHARED_LIBS)
jml_config_summary_message(TAGS)
jml_config_summary_message(PROFILE)
jml_config_summary_message(VALGRIND)
jml_config_summary_message(BUILD_DOCUMENTATION)
jml_config_summary_message(OCL_TESTS)
jml_config_summary_message(OCL_COVERAGE)
jml_config_summary_message(LIBRARY_SOURCES)
jml_config_summary_message(TEST_SOURCES)
jml_config_summary_message(LIB_INSTALL_DIR)
jml_config_summary_message(COVERAGE_DIR)
jml_separator()
