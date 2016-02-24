if(WIN32)
    set(CLINK_SLEEP_MACRO "Sleep(ms)")
else()
    set(CLINK_SLEEP_MACRO "usleep(ms * 1000)")
endif()

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/src/config.h.in
    ${CMAKE_CURRENT_SOURCE_DIR}/include/config.h
    @ONLY)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

find_library(HIDAPI hidapi PATHS ${HIDAPI_PREFIX})
include_directories(${CMAKE_CURRENT_LIST_DIR}/include)
include_directories(${HIDAPI_PREFIX}/include)
add_library(clink src/clink_fan.c src/clink_led.c src/clink_temp.c src/corsair_link.c)
target_link_libraries(clink ${HIDAPI})

install(TARGETS clink DESTINATION lib)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/include DESTINATION)
