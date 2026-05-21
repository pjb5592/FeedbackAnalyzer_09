#include <catch2/catch_test_macros.hpp>

#include "fixtures/Expectations.h"
#include "fixtures/FeedbackFixtures.h"
#include "support/LegacyTextAnalyzerAdapter.hpp"

TEST_CASE("FA_TC_09_KeywordShipping", "[fa-tc][p0][kw][legacy]") {
    LegacyTextAnalyzerAdapter adapter;
    const auto result = adapter.countKeywords(
        fa_fixtures::singleText(u8"배송이 빨라요"));
    fa_expect::expectKeywordGe(result, u8"배송", 1);
}
