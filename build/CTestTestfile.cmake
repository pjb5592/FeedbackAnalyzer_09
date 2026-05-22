# CMake generated Testfile for 
# Source directory: D:/DEV/FeedbackAnalyzer_09
# Build directory: D:/DEV/FeedbackAnalyzer_09/build
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
include("D:/DEV/FeedbackAnalyzer_09/build/feedback_analyzer_tests-b12d07c_include.cmake")
include("D:/DEV/FeedbackAnalyzer_09/build/feedback_analyzer_integration_tests-b12d07c_include.cmake")
add_test(GoldenMaster "D:/DEV/FeedbackAnalyzer_09/build/feedback_analyzer_tests.exe" "[gm]")
set_tests_properties(GoldenMaster PROPERTIES  LABELS "golden;gm;p2" WORKING_DIRECTORY "D:/DEV/FeedbackAnalyzer_09" _BACKTRACE_TRIPLES "D:/DEV/FeedbackAnalyzer_09/CMakeLists.txt;111;add_test;D:/DEV/FeedbackAnalyzer_09/CMakeLists.txt;0;")
subdirs("_deps/catch2-build")
