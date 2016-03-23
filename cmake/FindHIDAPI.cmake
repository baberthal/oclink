# - Try to find the HIDAPI libraries
#  Once done this will define
#
#  HIDAPI_FOUND - system has check
#  HIDAPI_INCLUDE_DIRS - the check include directory
#  HIDAPI_LIBRARIES - check library
#
#  Copyright (c) 2016 Morgan Lieberthal <morgan@jmorgan.org>
#
#  Redistribution and use is allowed according to the terms of the New
#  BSD license.
#  For details see the accompanying COPYING-CMAKE-SCRIPTS file.

find_package(PkgConfig)
pkg_check_modules(PC_HIDAPI QUIET hidapi)

find_path(HIDAPI_INCLUDE_DIR hidapi/hidapi.h
    PATHS ${HIDAPI_PREFIX}/include
    HINTS ${PC_HIDAPI_INCLUDE_DIR} ${PC_HIDAPI_INCLUDE_DIRS})

find_library(HIDAPI_LIBRARY
    names hidapi
    PATHS ${HIDAPI_PREFIX}/lib
    HINTS ${PC_HIDAPI_LIBDIR} ${PC_HIDAPI_LIBRARY_DIRS})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Hidapi
    DEFAULT_MSG
    HIDAPI_LIBRARY HIDAPI_INCLUDE_DIR)

mark_as_advanced(HIDAPI_LIBRARY HIDAPI_INCLUDE_DIR)

set(HIDAPI_LIBRARIES ${HIDAPI_LIBRARY})
set(HIDAPI_INCLUDE_DIRS ${HIDAPI_INCLUDE_DIR})

