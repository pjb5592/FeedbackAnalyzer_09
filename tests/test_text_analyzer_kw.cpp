#include <catch2/catch_test_macros.hpp>

#include "fixtures/Expectations.h"
#include "fixtures/FeedbackFixtures.h"
#include "support/LegacyTextAnalyzerAdapter.hpp"

TEST_CASE("FA_TC_09_KeywordShipping", "[fa-tc][p0][kw][legacy]") {
    LegacyTextAnalyzerAdapter adapter;
    const auto result = adapter.countKeywords(
        fa_fixtures::singleText(u8"ë°°ì†¡ì´ ë¹¨ë¼ìš”"));
    fa_expect::expectKeywordGe(result, u8"ë°°ì†¡", 1);
}

TEST_CASE("FA_TC_10_KeywordQuality", "[fa-tc][p0][kw][legacy]") {
    LegacyTextAnalyzerAdapter adapter;
    const auto result = adapter.countKeywords(
        fa_fixtures::singleText(u8"í’ˆì§ˆì´ ìš°ìˆ˜í•©ë‹ˆë‹¤"));
    fa_expect::expectKeywordGe(result, u8"í’ˆì§ˆ", 1);
}

TEST_CASE("FA_TC_11_KeywordPrice", "[fa-tc][p0][kw][legacy]") {
    LegacyTextAnalyzerAdapter adapter;
    const auto result = adapter.countKeywords(
        fa_fixtures::singleText(u8"°¡°ÝÀÌ ºñ½Î¿ä"));
    fa_expect::expectKeywordGe(result, u8"°¡°Ý", 1);
}

TEST_CASE("FA_TC_12_KeywordService", "[fa-tc][p0][kw][legacy]") {
    LegacyTextAnalyzerAdapter adapter;
    const auto result = adapter.countKeywords(
        fa_fixtures::singleText(u8"¼­ºñ½º°¡ Ä£ÀýÇØ¿ä"));
    fa_expect::expectKeywordGe(result, u8"¼­ºñ½º", 1);
}

TEST_CASE("FA_TC_13_KeywordUsability", "[fa-tc][p0][kw][legacy]") {
    LegacyTextAnalyzerAdapter adapter;
    const auto result = adapter.countKeywords(
        fa_fixtures::singleText(u8"»ç¿ëÀÌ Æí¸®ÇØ¿ä"));
    fa_expect::expectKeywordGe(result, u8"»ç¿ë¼º", 1);
}

TEST_CASE("FA_TC_14_KeywordNone", "[fa-tc][p0][kw][legacy]") {
    LegacyTextAnalyzerAdapter adapter;
    const auto result = adapter.countKeywords(
        fa_fixtures::singleText(u8"¿À´Ã ³¯¾¾°¡ ¸¼½À´Ï´Ù"));
    REQUIRE(result.at(u8"¹è¼Û") == 0);
    REQUIRE(result.at(u8"Ç°Áú") == 0);
    REQUIRE(result.at(u8"°¡°Ý") == 0);
    REQUIRE(result.at(u8"¼­ºñ½º") == 0);
    REQUIRE(result.at(u8"»ç¿ë¼º") == 0);
}

TEST_CASE("FA_TC_15_KeywordMultiCategory", "[fa-tc][p0][kw][legacy]") {
    LegacyTextAnalyzerAdapter adapter;
    const auto result = adapter.countKeywords(
        fa_fixtures::singleText(u8"¹è¼Ûµµ ºü¸£°í Ç°Áúµµ ÁÁ½À´Ï´Ù"));
    REQUIRE(result.at(u8"¹è¼Û") >= 1);
    REQUIRE(result.at(u8"Ç°Áú") >= 1);
}
