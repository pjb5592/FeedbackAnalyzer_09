cmake_minimum_required(VERSION 3.14)
project(FeedbackAnalyzer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(feedback_analyzer
    src/cpp/main.cpp
    src/cpp/Constants.cpp
    src/cpp/Filters.cpp
    src/cpp/Logger.cpp
    src/cpp/Session.cpp
    src/cpp/TextAnalyzer.cpp
    src/cpp/UIComponents.cpp
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
    src/cpp/Filters.cpp
    src/cpp/TextAnalyzer.cpp
)

set(FA_SUPPORT_SOURCES
    tests/support/DomainSentimentAnalyzer.cpp
    tests/support/DomainKeywordCounter.cpp
    tests/support/DomainFeedbackFilter.cpp
)

add_executable(feedback_analyzer_tests
    tests/test_main.cpp
    tests/test_text_analyzer_sent.cpp
    tests/test_text_analyzer_kw.cpp
    tests/test_consistency.cpp
    ${FA_LEGACY_SOURCES}
    ${FA_SUPPORT_SOURCES}
)

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
catch_discover_tests(feedback_analyzer_tests)
