cmake_minimum_required(VERSION 2.8)
project(OpenCorsairLink)

set(VERSION_MAJOR 0 CACHE INTERNAL "Project major version.")
set(VERSION_MINOR 2 CACHE INTERNAL "Project minor version.")
set(VERSION_TEENY 0 CACHE INTERNAL "Project teeny version.")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)

include(MacroEnsureOutOfSourceBuild)
macro_ensure_out_of_source_build("building in the source tree is not allowed.")

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE DEBUG)
    set(BUILD_DOCUMENTATION ON)
endif()

if(APPLE)
    set(CMAKE_MACOSX_RPATH ON)
endif(APPLE)

if(WIN32)
    set(CLINK_SLEEP_MACRO "Sleep(ms)")
else()
    set(CLINK_SLEEP_MACRO "usleep(ms * 1000)")
endif(WIN32)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/clink/src/config.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/config.h
    @ONLY)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(OCL_UTILS_DIR ${CMAKE_CURRENT_LIST_DIR}/debug)

option(BUILD_SHARED_LIBS "build shared libraries" OFF)
set(HIDAPI_PREFIX "/usr/local" CACHE PATH "prefix where hidapi is installed")

include_directories("${CMAKE_CURRENT_LIST_DIR}/debug")
add_subdirectory(debug)

include_directories("${CMAKE_CURRENT_LIST_DIR}/clink" "${CMAKE_CURRENT_LIST_DIR}/oclink")

add_subdirectory(clink)
add_subdirectory(oclink)
add_subdirectory(tui)
add_subdirectory(doc)