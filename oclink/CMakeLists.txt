include_directories(${PROJECT_SOURCE_DIR}/clink/include)

add_executable(oclink main.c $<TARGET_OBJECTS:cl_debug>)
add_dependencies(oclink clink)
target_link_libraries(oclink clink)
