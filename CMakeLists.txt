cmake_minimum_required(VERSION 2.8)
project(OpenCorsairLink)

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

set(OCL_UTILS_DIR ${CMAKE_CURRENT_LIST_DIR}/debug)

option(BUILD_SHARED_LIBS "build shared libraries" OFF)
set(HIDAPI_PREFIX "/usr/local" CACHE PATH "prefix where hidapi is installed")

include_directories("${CMAKE_CURRENT_LIST_DIR}/debug")
add_subdirectory(debug)

include_directories("${CMAKE_CURRENT_LIST_DIR}/clink" "${CMAKE_CURRENT_LIST_DIR}/oclink")
add_subdirectory(clink)
add_subdirectory(oclink)

set(VERSION_MAJOR 0 CACHE INTERNAL "Project major version.")
set(VERSION_MINOR 2 CACHE INTERNAL "Project minor version.")
set(VERSION_TEENY 0 CACHE INTERNAL "Project teeny version.")

add_subdirectory(doc)
