set(DARWIN_LIB "libclink_darwin")
set(DARWIN_STATICLIB "libclink_darwin_static")

set(DARWIN_HEADERS
  clink_darwin.h
  util.h
  hid_utils.h
  pthread_barrier.h
)

set(DARWIN_SOURCES
  clink_darwin.c
  util.c
  hid_utils.c
  pthread_barrier.c
  ${DARWIN_HEADERS}
)

find_library(CARBON_LIBRARY Carbon)
find_library(CORE_FOUNDATION_LIBRARY CoreFoundation)
find_library(IO_KIT_LIBRARY IOKit)

include(CheckIncludeFile)
check_include_file("CoreFoundation/CoreFoundation.h" HAVE_CORE_FOUNDATION_H)
check_include_file("IOKit/hid/IOHIDLib.h" HAVE_IOHID_LIB_H)
check_include_file(unistd.h HAVE_UNISTD_H)
check_include_file(stdlib.h HAVE_STDLIB_H)
check_include_file(stdint.h HAVE_STDINT_H)
check_include_file(stdbool.h HAVE_STDBOOL_H)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/clink_darwin_cfg.h.in
  ${CMAKE_CURRENT_BINARY_DIR}/clink_darwin_cfg.h
)

include_directories(. ${CMAKE_CURRENT_BINARY_DIR})

include(GenerateExportHeader)

add_library(${DARWIN_LIB} SHARED ${DARWIN_SOURCES})
set_target_properties(${DARWIN_LIB} PROPERTIES
  OUTPUT_NAME "clink_darwin"
  SOVERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}
  VERSION ${PROJECT_VERSION}
)

add_library(${DARWIN_STATICLIB} STATIC ${DARWIN_SOURCES})
set_target_properties(${DARWIN_STATICLIB} PROPERTIES
  COMPILE_FLAGS -DCLINK_DARWIN_STATIC_DEFINE
  POSITION_INDEPENDENT_CODE ON
  OUTPUT_NAME "clink_darwin_static"
  VERSION ${PROJECT_VERSION}
)

target_link_libraries(${DARWIN_LIB}
  ${CARBON_LIBRARY}
  ${CORE_FOUNDATION_LIBRARY}
  ${IO_KIT_LIBRARY}
)

target_link_libraries(${DARWIN_STATICLIB}
  ${CARBON_LIBRARY}
  ${CORE_FOUNDATION_LIBRARY}
  ${IO_KIT_LIBRARY}
)

set_property(TARGET ${DARWIN_LIB} APPEND PROPERTY MACOSX_RPATH true)
set_target_properties(${DARWIN_LIB} PROPERTIES PDB_NAME clink_darwin_dll)
generate_export_header(${DARWIN_LIB} BASE_NAME "clink_darwin")

if(OCL_TESTS)
  include_directories(test)
  add_subdirectory(test)
  enable_testing()

  add_test(NAME darwin_test_suite COMMAND darwin_tests)
endif(OCL_TESTS)
