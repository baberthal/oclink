set(CURSES_NEED_NCURSES TRUE)
include(FindCurses)

if(APPLE)
    option(USE_HOMEBREW_NCURSES "Use homebrew curses, if installed." ON)
endif(APPLE)

if(USE_HOMEBREW_NCURSES)
    message("Using homebrew ncurses")
    find_library(Curses PATHS "/usr/local/opt/ncurses" NO_DEFAULT_PATH)
else()
    find_package(Curses)
endif(USE_HOMEBREW_NCURSES)

include(CMakePrintHelpers)

cmake_print_variables(CURSES_FOUND CURSES_INCLUDE_DIR CURSES_LIBRARIES CURSES_HAVE_CURSES_H CURSES_HAVE_NCURSES_H CURSES_HAVE_NCURSES_CURSES_H CURSES_HAVE_NCURSES_NCURSES_H )

if(NOT CURSES_FOUND)
    return()
endif(NOT CURSES_FOUND)


if(CURSES_HAVE_NCURSES_H)
    set(CURSES_INCLUDE_DIRECTIVE "ncurses.h"
        CACHE INTERNAL "include directive")

elseif(CURSES_HAVE_NCURSES_NCURSES_H)
    set(CURSES_INCLUDE_DIRECTIVE "ncurses/ncurses.h"
        CACHE INTERNAL "include directive")

elseif(CURSES_HAVE_CURSES_H)
    set(CURSES_INCLUDE_DIRECTIVE "curses.h"
        CACHE INTERNAL "include directive")

elseif(CURSES_HAVE_NCURSES_CURSES_H)
    set(CURSES_INCLUDE_DIRECTIVE "ncurses/curses.h"
        CACHE INTERNAL "include directive")

endif(CURSES_HAVE_NCURSES_H)

configure_file("${CMAKE_CURRENT_LIST_DIR}/config.h.in"
    ${CMAKE_CURRENT_BINARY_DIR}/tui_config.h
    @ONLY)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${PROJECT_SOURCE_DIR}/clink/include)
include_directories(${CURSES_INCLUDE_DIR})

add_executable(tui main.c $<TARGET_OBJECTS:cl_debug>)
add_dependencies(tui clink)
target_link_libraries(tui clink ${CURSES_LIBRARIES})
