cmake_minimum_required(VERSION 3.14)
project(FeedbackAnalyzer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(cmake/FACoverage.cmake)

add_executable(feedback_analyzer
    src/cpp/Constants.cpp
    src/cpp/CsvUploadParser.cpp
    src/cpp/FileHandler.cpp
    src/cpp/SentimentClassifier.cpp
    src/cpp/SentimentKeywordStore.cpp
    src/cpp/TrendAnalyzer.cpp
    src/cpp/TrendCsvParser.cpp
    src/cpp/Filters.cpp
    src/cpp/HtmlRenderer.cpp
    src/cpp/HttpRouter.cpp
    src/cpp/Logger.cpp
    src/cpp/Session.cpp
    src/cpp/TextAnalyzer.cpp
    src/cpp/UIComponents.cpp
    src/cpp/main.cpp
)

target_include_directories(feedback_analyzer PRIVATE src/cpp)

# Windows: httplib mmap uses CreateFile2 / CreateFileMappingFromApp (Win8+).
# MinGW defaults to older WINVER unless raised here.
if(WIN32)
    target_compile_definitions(feedback_analyzer PRIVATE
        _WIN32_WINNT=0x0A00
        WINVER=0x0A00
    )
    target_link_libraries(feedback_analyzer PRIVATE ws2_32)
endif()

# --- Catch2 (red/green test harness; legacy src/cpp headers unchanged) ---
include(FetchContent)
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v3.5.4
)
set(CATCH_BUILD_TESTING OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(Catch2)

set(FA_LEGACY_SOURCES
    src/cpp/Constants.cpp
    src/cpp/CsvUploadParser.cpp
    src/cpp/FileHandler.cpp
    src/cpp/SentimentClassifier.cpp
    src/cpp/SentimentKeywordStore.cpp
    src/cpp/TrendAnalyzer.cpp
    src/cpp/TrendCsvParser.cpp
    src/cpp/Filters.cpp
    src/cpp/TextAnalyzer.cpp
)

set(FA_SUPPORT_SOURCES
    tests/support/DomainSentimentAnalyzer.cpp
    tests/support/DomainKeywordCounter.cpp
    tests/support/DomainFeedbackFilter.cpp
    tests/support/InMemoryDownloadSource.cpp
    tests/support/GoldenMasterRunner.cpp
)

add_executable(feedback_analyzer_tests
    tests/test_main.cpp
    tests/test_text_analyzer_sent.cpp
    tests/test_text_analyzer_kw.cpp
    tests/test_filters.cpp
    tests/test_consistency.cpp
    tests/test_csv_session.cpp
    tests/test_coverage_boundary.cpp
    tests/test_golden_master.cpp
    tests/test_perf_skip.cpp
    tests/test_feature.cpp
    ${FA_LEGACY_SOURCES}
    ${FA_SUPPORT_SOURCES}
)

fa_apply_coverage(feedback_analyzer_tests)

target_include_directories(feedback_analyzer_tests PRIVATE
    src/cpp
    tests
    tests/fixtures
    tests/support
)

target_link_libraries(feedback_analyzer_tests PRIVATE Catch2::Catch2)

if(MSVC)
    target_compile_options(feedback_analyzer_tests PRIVATE /utf-8)
else()
    target_compile_options(feedback_analyzer_tests PRIVATE -finput-charset=UTF-8)
endif()

enable_testing()
include(CTest)
include(Catch)
catch_discover_tests(feedback_analyzer_tests
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

add_executable(golden_master_gen
    tests/tools/golden_master_gen.cpp
    src/cpp/Constants.cpp
    ${FA_SUPPORT_SOURCES}
)
target_include_directories(golden_master_gen PRIVATE
    src/cpp
    tests
    tests/fixtures
    tests/support
)
if(MSVC)
    target_compile_options(golden_master_gen PRIVATE /utf-8)
else()
    target_compile_options(golden_master_gen PRIVATE -finput-charset=UTF-8)
endif()

add_test(NAME GoldenMaster
    COMMAND $<TARGET_FILE:feedback_analyzer_tests> "[gm]"
)
set_tests_properties(GoldenMaster PROPERTIES
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    LABELS "golden;gm;p2"
)

if(FA_ENABLE_COVERAGE)
    add_custom_target(feedback_analyzer_tests_cov
        COMMAND ${CMAKE_COMMAND}
                -DFA_COVERAGE_DIR=${CMAKE_BINARY_DIR}
                -DCMAKE_SOURCE_DIR=${CMAKE_SOURCE_DIR}
                -P ${CMAKE_SOURCE_DIR}/cmake/RunCoverage.cmake
        DEPENDS feedback_analyzer_tests
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Run unit tests and capture lcov baseline for coverage gate"
    )
endif()

add_executable(feedback_analyzer_integration_tests
    tests/integration/test_http_main.cpp
    tests/integration/test_http_integration.cpp
)

target_include_directories(feedback_analyzer_integration_tests PRIVATE
    src/cpp
    tests/integration
)

target_compile_definitions(feedback_analyzer_integration_tests PRIVATE
    FA_SERVER_EXE="$<TARGET_FILE:feedback_analyzer>"
)

target_link_libraries(feedback_analyzer_integration_tests PRIVATE Catch2::Catch2)

if(WIN32)
    target_compile_definitions(feedback_analyzer_integration_tests PRIVATE
        _WIN32_WINNT=0x0A00
        WINVER=0x0A00
    )
    target_link_libraries(feedback_analyzer_integration_tests PRIVATE ws2_32)
endif()

add_dependencies(feedback_analyzer_integration_tests feedback_analyzer)

catch_discover_tests(feedback_analyzer_integration_tests
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)
