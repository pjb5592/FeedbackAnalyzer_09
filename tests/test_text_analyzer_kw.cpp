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
        fa_fixtures::singleText(u8"°¡°İÀÌ ºñ½Î¿ä"));
    fa_expect::expectKeywordGe(result, u8"°¡°İ", 1);
}

TEST_CASE("FA_TC_12_KeywordService", "[fa-tc][p0][kw][legacy]") {
    LegacyTextAnalyzerAdapter adapter;
    const auto result = adapter.countKeywords(
        fa_fixtures::singleText(u8"¼­ºñ½º°¡ Ä£ÀıÇØ¿ä"));
    fa_expect::expectKeywordGe(result, u8"¼­ºñ½º", 1);
}
