include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

find_library(HIDAPI hidapi PATHS ${HIDAPI_PREFIX})
include_directories(${HIDAPI_PREFIX}/include)

add_library(clink src/clink_fan.c src/clink_led.c src/clink_temp.c src/corsair_link.c)
target_link_libraries(clink ${HIDAPI})

install(TARGETS clink DESTINATION lib)
install(DIRECTORY
    ${CMAKE_CURRENT_SOURCE_DIR}/include ${PROJECT_BINARY_DIR}
    DESTINATION include/clink
    FILES_MATCHING PATTERN "*.h"
    PATTERN "oclink/*" EXCLUDE)
