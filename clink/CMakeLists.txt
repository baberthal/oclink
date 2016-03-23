include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

find_library(HIDAPI hidapi PATHS ${HIDAPI_PREFIX})
include_directories(${HIDAPI_PREFIX}/include)

set(HEADERS
    clink.h
    proto.h
    fan.h
    led.h
    link.h
    temp.h)

add_subdirectory(src)

if(OCL_TESTS)
    add_subdirectory(test)
    enable_testing()
endif(OCL_TESTS)
