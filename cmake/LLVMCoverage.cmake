find_program(LLVM_COV_PATH llvm-cov)
find_program(LLVM_PROFDATA_PATH llvm-profdata)

if(NOT LLVM_COV_PATH)
  message(FATAL_ERROR "llvm-cov not found! Aborting...")
endif(NOT LLVM_COV_PATH)

if(NOT LLVM_PROFDATA_PATH)
  message(FATAL_ERROR "llvm-profdata not found. Aborting...")
endif(NOT LLVM_PROFDATA_PATH)

if(NOT "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  message(FATAL_ERROR "Compiler is not Clang! Aborting...")
endif(NOT "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")

set(CMAKE_C_FLAGS_COVERAGE "-g -O0 -fprofile-instr-generate -fcoverage-mapping")
set(CMAKE_CXX_FLAGS_COVERAGE "-g -O0 -fprofile-instr-generate -fcoverage-mapping")
set(CMAKE_EXE_LINKER_FLAGS_COVERAGE "-fprofile-instr-generate")
set(CMAKE_SHARED_LINKER_FLAGS_COVERAGE "-fprofile-instr-generate")

set(COVERAGE_DIR "${PROJECT_BINARY_DIR}/coverage" CACHE PATH "Project coverage directory")

set(GENERATED_PROFDATA "" CACHE INTERNAL "The generated profdata")

mark_as_advanced(CMAKE_C_FLAGS_COVERAGE
  CMAKE_CXX_FLAGS_COVERAGE
  CMAKE_EXE_LINKER_FLAGS_COVERAGE
  CMAKE_SHARED_LINKER_FLAGS_COVERAGE)

file(MAKE_DIRECTORY ${COVERAGE_DIR})

add_custom_target(coverage-all)

function(ADD_LLVM_COVERAGE_TARGET _targetname _working_directory _source_files)
  if(NOT LLVM_PROFDATA_PATH)
    message(FATAL_ERROR "llvm-profdata not found. Aborting...")
  endif(NOT LLVM_PROFDATA_PATH)

  set(MY_SOURCE_FILES "")
  foreach(SRC_FILE ${_source_files})
    get_filename_component(FULL_SRC_PATH "${SRC_FILE}" ABSOLUTE)
    list(APPEND MY_SOURCE_FILES ${FULL_SRC_PATH})
  endforeach()

  add_custom_command(OUTPUT "${_targetname}.profraw"
    COMMAND "${_targetname}" ${ARGV3} # run the tests to generate profdata
    # rename the profraw files
    COMMAND ${CMAKE_COMMAND} -E rename "default.profraw" "${_targetname}.profraw"
    VERBATIM
    USES_TERMINAL
    DEPENDS ${_targetname}
    WORKING_DIRECTORY ${_working_directory}
    COMMENT "Running executable to generate raw profile data")

  add_custom_command(OUTPUT "${COVERAGE_DIR}/${_targetname}.profdata"
    COMMAND ${LLVM_PROFDATA_PATH} merge -o "${COVERAGE_DIR}/${_targetname}.profdata"
    "${_targetname}.profraw"
    WORKING_DIRECTORY ${_working_directory}
    DEPENDS "${_targetname}.profraw")

  add_custom_command(OUTPUT "${COVERAGE_DIR}/${_targetname}_full_report.txt"
    COMMAND ${LLVM_COV_PATH} show -instr-profile=${_targetname}.profdata
    $<TARGET_FILE:${_targetname}> ${MY_SOURCE_FILES}
    > "${COVERAGE_DIR}/${_targetname}_full_report.txt"

    WORKING_DIRECTORY ${COVERAGE_DIR}
    DEPENDS "${COVERAGE_DIR}/${_targetname}.profdata"
    VERBATIM)

  add_custom_command(OUTPUT "${COVERAGE_DIR}/${_targetname}_report.txt"
    COMMAND ${LLVM_COV_PATH} report -instr-profile=${_targetname}.profdata
    $<TARGET_FILE:${_targetname}> ${MY_SOURCE_FILES}
    > ${_targetname}_report.txt
    DEPENDS "${COVERAGE_DIR}/${_targetname}.profdata"

    WORKING_DIRECTORY ${COVERAGE_DIR})

  add_custom_target(coverage-${_targetname}
    DEPENDS "${COVERAGE_DIR}/${_targetname}_full_report.txt"
    DEPENDS "${COVERAGE_DIR}/${_targetname}_report.txt"
    COMMAND ${LLVM_COV_PATH} report -instr-profile=${_targetname}.profdata
    $<TARGET_FILE:${_targetname}> ${MY_SOURCE_FILES}

    WORKING_DIRECTORY ${COVERAGE_DIR})

  add_dependencies(coverage-all coverage-${_targetname})

endfunction(ADD_LLVM_COVERAGE_TARGET)
