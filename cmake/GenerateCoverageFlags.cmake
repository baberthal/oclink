# - Try to figure out which compiler flags to use for coverage
#  Once done this will define
#
#  <PREFIX>_COVERAGE_SUPPORTED - system supports code coverage info generation
#  <PREFIX>_COVERAGE_FLAGS - the compiler and linker flags to use
#
#  Copyright (c) 2016 Morgan Lieberthal <morgan@jmorgan.org>
#
#  Redistribution and use is allowed according to the terms of the New
#  BSD license.
#  For details see the accompanying COPYING-CMAKE-SCRIPTS file.
include(CheckCSourceCompiles)

macro(generate_coverage_flags _prefix)

    set(DUMMY_MAIN "int main(void) { return 0; }")
    set(CMAKE_REQUIRED_FLAGS "-fprofile-arcs -ftest-coverage")
    check_c_source_compiles("${DUMMY_MAIN}" GCC_COV_WORKS)

    set(CMAKE_REQUIRED_FLAGS "-fprofile-instr-generate -fcoverage-mapping")
    check_c_source_compiles("${DUMMY_MAIN}" LLVM_COV_WORKS)

    if(GCC_COV_WORKS)
        set(${_prefix}_GCC_COVERAGE_SUPPORTED 1)
        set(${_prefix}_GCC_COVERAGE_FLAGS "-fprofile-arcs -ftest-coverage")
    else()
        set(${_prefix}_GCC_COVERAGE_SUPPORTED "${_prefix}-GCC-UNSUPPORTED")
    endif(GCC_COV_WORKS)

    if(LLVM_COV_WORKS)
        set(${_prefix}_LLVM_COV_SUPPORTED 1)
        set(${_prefix}_LLVM_COV_FLAGS "-fprofile-instr-generate -fcoverage-mapping")
    else()
        set(${_prefix}_LLVM_COV_COVERAGE_SUPPORTED "${_prefix}-LLVM-UNSUPPORTED")
    endif(LLVM_COV_WORKS)

endmacro(generate_coverage_flags)

