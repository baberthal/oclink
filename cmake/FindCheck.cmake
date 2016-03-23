# - Try to find the CHECK libraries
#  Once done this will define
#
#  CHECK_FOUND - system has check
#  CHECK_INCLUDE_DIR - the check include directory
#  CHECK_LIBRARIES - check library
#
#  This configuration file for finding libcheck is originally from
#  the opensync project. The originally was downloaded from here:
#  opensync.org/browser/branches/3rd-party-cmake-modules/modules/FindCheck.cmake
#
#  Copyright (c) 2007 Daniel Gollub <dgollub@suse.de>
#  Copyright (c) 2007 Bjoern Ricks  <b.ricks@fh-osnabrueck.de>
#
#  Redistribution and use is allowed according to the terms of the New
#  BSD license.
#  For details see the accompanying COPYING-CMAKE-SCRIPTS file.

find_package(PkgConfig)
pkg_check_modules(PC_CHECK QUIET check)

find_path(CHECK_INCLUDE_DIR check.h
    PATHS ${CHECK_PREFIX}/include
    HINTS ${PC_CHECK_INCLUDE_DIR} ${PC_CHECK_INCLUDE_DIRS})

find_library(CHECK_LIBRARY
    NAMES check
    PATHS ${CHECK_PREFIX}/lib
    HINTS ${PC_CHECK_LIBDIR} ${PC_CHECK_LIBRARY_DIRS})

if("${CHECK_LIBRARY}" STREQUAL "CHECK_LIBRARY-NOTFOUND")

    find_library(COMPAT_LIBRARY
        NAMES compat
        PATHS ${CHECK_PREFIX}/lib /usr/local/lib /usr/lib
        HINTS ${PC_CHECK_LIBDIR} ${PC_CHECK_LIBRARY_DIRS})

endif("${CHECK_LIBRARY}" STREQUAL "CHECK_LIBRARY-NOTFOUND")

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(CHECK
    DEFAULT_MSG
    CHECK_LIBRARY CHECK_INCLUDE_DIR)

if(NOT CHECK_LIBRARY MATCHES "NOTFOUND")
    set(CHECK_LIBRARIES "${CHECK_LIBRARY}")
endif(NOT CHECK_LIBRARY MATCHES "NOTFOUND")

if(NOT "${COMPAT_LIBRARY}" MATCHES "NOTFOUND")
    set(CHECK_LIBRARIES "${CHECK_LIBRARIES}" ${COMPAT_LIBRARY})
endif(NOT "${COMPAT_LIBRARY}" MATCHES "NOTFOUND")

# Hide advanced variables from CMake GUIs
MARK_AS_ADVANCED( CHECK_INCLUDE_DIR CHECK_LIBRARIES )

# INCLUDE( FindPkgConfig )
# # Take care about check.pc settings
# PKG_SEARCH_MODULE( CHECK check )

# # Look for CHECK include dir and libraries
# IF( NOT CHECK_FOUND )
#       IF ( CHECK_INSTALL_DIR )
#               MESSAGE ( STATUS "Using override CHECK_INSTALL_DIR to find check" )
#               SET ( CHECK_INCLUDE_DIR  "${CHECK_INSTALL_DIR}/include" )
#               SET ( CHECK_INCLUDE_DIRS "${CHECK_INCLUDE_DIR}" )
#               FIND_LIBRARY( CHECK_LIBRARY NAMES check PATHS "${CHECK_INSTALL_DIR}/lib" )
#               FIND_LIBRARY( COMPAT_LIBRARY NAMES compat PATHS "${CHECK_INSTALL_DIR}/lib" )
#               SET ( CHECK_LIBRARIES "${CHECK_LIBRARY}" "${COMPAT_LIBRARY}" )
#       ELSE ( CHECK_INSTALL_DIR )
#               FIND_PATH( CHECK_INCLUDE_DIR check.h )
#               FIND_LIBRARY( CHECK_LIBRARIES NAMES check )
#       ENDIF ( CHECK_INSTALL_DIR )

#       IF ( CHECK_INCLUDE_DIR AND CHECK_LIBRARIES )
#               SET( CHECK_FOUND 1 )
#               IF ( NOT Check_FIND_QUIETLY )
#                       MESSAGE ( STATUS "Found CHECK: ${CHECK_LIBRARIES}" )
#               ENDIF ( NOT Check_FIND_QUIETLY )
#       ELSE ( CHECK_INCLUDE_DIR AND CHECK_LIBRARIES )
#               IF ( Check_FIND_REQUIRED )
#                       MESSAGE( FATAL_ERROR "Could NOT find CHECK" )
#               ELSE ( Check_FIND_REQUIRED )
#                       IF ( NOT Check_FIND_QUIETLY )
#                               MESSAGE( STATUS "Could NOT find CHECK" )
#                       ENDIF ( NOT Check_FIND_QUIETLY )
#               ENDIF ( Check_FIND_REQUIRED )
#       ENDIF ( CHECK_INCLUDE_DIR AND CHECK_LIBRARIES )
# ENDIF( NOT CHECK_FOUND )

# # Hide advanced variables from CMake GUIs
# MARK_AS_ADVANCED( CHECK_INCLUDE_DIR CHECK_LIBRARIES )
