#include <catch2/catch_test_macros.hpp>

#include "fixtures/Expectations.h"
#include "fixtures/FeedbackFixtures.h"
#include "support/LegacyTextAnalyzerAdapter.hpp"

TEST_CASE("FA_TC_09_KeywordShipping", "[fa-tc][p0][kw][legacy]") {
    LegacyTextAnalyzerAdapter adapter;
    const auto result = adapter.countKeywords(
        fa_fixtures::singleText(u8"??? ???"));
    fa_expect::expectKeywordGe(result, u8"??", 1);
}

TEST_CASE("FA_TC_10_KeywordQuality", "[fa-tc][p0][kw][legacy]") {
    LegacyTextAnalyzerAdapter adapter;
    const auto result = adapter.countKeywords(
        fa_fixtures::singleText(u8"??? ?????"));
    fa_expect::expectKeywordGe(result, u8"??", 1);
}

TEST_CASE("FA_TC_11_KeywordPrice", "[fa-tc][p0][kw][legacy]") {
    LegacyTextAnalyzerAdapter adapter;
    const auto result = adapter.countKeywords(
        fa_fixtures::singleText(u8"??? ???"));
    fa_expect::expectKeywordGe(result, u8"??", 1);
}

TEST_CASE("FA_TC_12_KeywordService", "[fa-tc][p0][kw][legacy]") {
    LegacyTextAnalyzerAdapter adapter;
    const auto result = adapter.countKeywords(
        fa_fixtures::singleText(u8"???? ????"));
    fa_expect::expectKeywordGe(result, u8"???", 1);
}

TEST_CASE("FA_TC_13_KeywordUsability", "[fa-tc][p0][kw][legacy]") {
    LegacyTextAnalyzerAdapter adapter;
    const auto result = adapter.countKeywords(
        fa_fixtures::singleText(u8"??? ????"));
    fa_expect::expectKeywordGe(result, u8"???", 1);
}

TEST_CASE("FA_TC_14_KeywordNone", "[fa-tc][p0][kw][legacy]") {
    LegacyTextAnalyzerAdapter adapter;
    const auto result = adapter.countKeywords(
        fa_fixtures::singleText(u8"?? ??? ????"));
    REQUIRE(result.at(u8"??") == 0);
    REQUIRE(result.at(u8"??") == 0);
    REQUIRE(result.at(u8"??") == 0);
    REQUIRE(result.at(u8"???") == 0);
    REQUIRE(result.at(u8"???") == 0);
}

TEST_CASE("FA_TC_15_KeywordMultiCategory", "[fa-tc][p0][kw][legacy]") {
    LegacyTextAnalyzerAdapter adapter;
    const auto result = adapter.countKeywords(
        fa_fixtures::singleText(u8"??? ??? ??? ????"));
    REQUIRE(result.at(u8"??") >= 1);
    REQUIRE(result.at(u8"??") >= 1);
}
