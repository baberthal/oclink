set(SOURCES
    fan.c
    led.c
    link.c
    temp.c
    ${HEADERS})

add_library(clink ${SOURCES})
target_link_libraries(clink ${HIDAPI})
