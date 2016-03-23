set(LIBRARY "libclink")

find_package(HIDAPI REQUIRED)
include_directories(${HIDAPI_INCLUDE_DIRS})

set(HEADERS
    clink.h
    proto.h
    fan.h
    led.h
    link.h
    temp.h)

set(LIBRARY_SOURCES
    link.c
    fan.c
    led.c
    temp.c
    ${HEADERS})

set(PROGRAM "clink")
set(PROGRAM_SOURCES
    main.c
    opt.c
    opt.h
    ${LIBRARY_SOURCES})

if(NOT LIB_INSTALL_DIR)
    set(LIB_INSTALL_DIR "lib" CACHE STRING
        "Set the installation directory for libraries." FORCE)
endif(NOT LIB_INSTALL_DIR)

include_directories(. ${CMAKE_CURRENT_BINARY_DIR})

include(CheckIncludeFile)
check_include_file(unistd.h HAVE_UNISTD_H)
check_include_file(stdlib.h HAVE_STDLIB_H)
check_include_file(stdint.h HAVE_STDINT_H)
check_include_file(stdbool.h HAVE_STDBOOL_H)

if(WIN32)
    set(CLINK_SLEEP_MACRO "Sleep(ms)")
else()
    set(CLINK_SLEEP_MACRO "usleep(ms * 1000)")
endif(WIN32)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/config.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/config.h
    @ONLY)

include(GenerateExportHeader)

add_executable(${PROGRAM} ${PROGRAM_SOURCES})
set_target_properties(${PROGRAM} PROPERTIES
    COMPILE_FLAGS -DCLINK_STATIC_DEFINE)
target_link_libraries(${PROGRAM} ${HIDAPI_LIBRARIES})

add_library(${LIBRARY} ${LIBRARY_SOURCES})
set_target_properties(${LIBRARY} PROPERTIES
    OUTPUT_NAME "clink"
    SOVERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}
    VERSION ${PROJECT_VERSION})
target_link_libraries(${LIBRARY} ${HIDAPI_LIBRARIES})

set_property(TARGET ${LIBRARY} APPEND PROPERTY MACOSX_RPATH true)
set_target_properties(${LIBRARY} PROPERTIES PDB_NAME clink_dll)
generate_export_header(${LIBRARY} BASE_NAME ${PROJECT_NAME})

target_link_libraries(clink ${HIDAPI})
