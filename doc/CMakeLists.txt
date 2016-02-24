find_package(Doxygen)
option(BUILD_DOCUMENTATION
    "Create and install the doxygen documentation (requires doxygen)."
    ${DOXYGEN_FOUND})

include(CMakeDependentOption)
option(doxy_gen_html "Generate HTML Documentation" ON)
option(doxy_gen_latex "Generate LaTeX Documentation" ON)
option(doxy_gen_man "Generate Man Pages" ON)
option(doxy_gen_docset "Generate Xcode Docset" OFF)

mark_as_advanced(doxy_gen_html)
mark_as_advanced(doxy_gen_latex)
mark_as_advanced(doxy_gen_man)
mark_as_advanced(doxy_gen_docset)

if(doxy_gen_html)
    set(doxy_gen_html_flag "YES" CACHE INTERNAL "internal use only")
endif(doxy_gen_html)

if(doxy_gen_latex)
    set(doxy_gen_latex_flag "YES" CACHE INTERNAL "internal use only")
endif(doxy_gen_latex)

if(doxy_gen_man)
    set(doxy_gen_man_flag "YES" CACHE INTERNAL "internal use only")
endif(doxy_gen_man)

if(doxy_gen_docset)
    set(doxy_gen_html_flag "YES" CACHE INTERNAL "internal use only")
endif(doxy_gen_docset)

set(doxy_main_page ${PROJECT_SOURCE_DIR}/README.md CACHE INTERNAL "Doxygen main page")
mark_as_advanced(doxy_main_page)

if(BUILD_DOCUMENTATION)
    if(NOT DOXYGEN_FOUND)
        message(FATAL_ERROR "Doxygen is needed to build the documentation.")
    endif(NOT DOXYGEN_FOUND)

    set(doxyfile_in ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
    set(doxyfile ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

    configure_file(${doxyfile_in} ${doxyfile} @ONLY)

    add_custom_target(doc
        COMMAND ${DOXYGEN_EXECUTABLE} ${doxyfile}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM)

    install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html DESTINATION share/doc)
endif(BUILD_DOCUMENTATION)
