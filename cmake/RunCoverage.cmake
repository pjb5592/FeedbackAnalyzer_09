# Run feedback_analyzer_tests and capture lcov (invoked by feedback_analyzer_tests_cov).
if(NOT DEFINED FA_COVERAGE_DIR OR NOT FA_COVERAGE_DIR)
    message(FATAL_ERROR "FA_COVERAGE_DIR not set (pass -DFA_COVERAGE_DIR=<build-dir>)")
endif()
if(NOT DEFINED CMAKE_SOURCE_DIR OR NOT CMAKE_SOURCE_DIR)
    set(CMAKE_SOURCE_DIR "${FA_COVERAGE_DIR}/..")
endif()

set(TEST_EXE "${FA_COVERAGE_DIR}/feedback_analyzer_tests.exe")
if(NOT EXISTS "${TEST_EXE}")
    set(TEST_EXE "${FA_COVERAGE_DIR}/feedback_analyzer_tests")
endif()
if(NOT EXISTS "${TEST_EXE}")
    message(FATAL_ERROR "Test executable not found under ${FA_COVERAGE_DIR}")
endif()

find_program(LCOV_BIN lcov)
if(NOT LCOV_BIN)
    message(FATAL_ERROR "lcov not found; install lcov for FA_ENABLE_COVERAGE")
endif()

execute_process(
    COMMAND "${LCOV_BIN}" --directory . --zerocounters
    WORKING_DIRECTORY "${FA_COVERAGE_DIR}"
    RESULT_VARIABLE _zero_result
)

execute_process(
    COMMAND "${TEST_EXE}"
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    RESULT_VARIABLE _run_result
)
if(_run_result)
    message(FATAL_ERROR "feedback_analyzer_tests failed with code ${_run_result}")
endif()

set(INFO_FILE "${FA_COVERAGE_DIR}/coverage.info")
execute_process(
    COMMAND "${LCOV_BIN}" --directory . --capture
            --output-file "${INFO_FILE}" --rc lcov_branch_coverage=1
    WORKING_DIRECTORY "${FA_COVERAGE_DIR}"
    RESULT_VARIABLE _lcov_result
    ERROR_VARIABLE _lcov_err
)
if(_lcov_result)
    message(FATAL_ERROR "lcov capture failed (${_lcov_result}): ${_lcov_err}")
endif()

message(STATUS "Wrote ${INFO_FILE}")
