#include <catch2/catch_test_macros.hpp>

#include "fixtures/FeedbackFixtures.h"
#include "support/LegacyFiltersAdapter.hpp"
#include "support/LegacyTextAnalyzerAdapter.hpp"

TEST_CASE("FA_TC_16_KeywordMainOnlyMismatch", "[fa-tc][p0][kw][def-02][legacy]") {
    const auto feedbacks = fa_fixtures::mainOnlyQuality();
    LegacyTextAnalyzerAdapter analyzer;
    LegacyFiltersAdapter filter;

    const auto kw = analyzer.countKeywords(feedbacks);
    const auto filtered =
        filter.filter(feedbacks, u8"전체", u8"품질");

    REQUIRE(kw.at(u8"품질") >= 1);
    REQUIRE(filtered.size() >= 1);
}

TEST_CASE("FA_TC_17_FilterNeutralZero", "[fa-tc][p0][fil][def-01][legacy]") {
    const auto feedbacks = fa_fixtures::def01NeutralAmbiguous();
    LegacyFiltersAdapter filter;
    const auto filtered = filter.filter(feedbacks, u8"중립", u8"전체");
    REQUIRE(filtered.size() == 1);
}

TEST_CASE("FA_TC_29_ConsistencySentFilNeutral", "[fa-tc][p0][def-01][legacy]") {
    const auto feedbacks = fa_fixtures::def01NeutralAmbiguous();
    LegacyTextAnalyzerAdapter analyzer;
    LegacyFiltersAdapter filter;

    const auto sent = analyzer.analyzeSentiment(feedbacks);
    const auto filtered = filter.filter(feedbacks, u8"중립", u8"전체");

    REQUIRE(sent.at(u8"중립") == 1);
    REQUIRE(filtered.size() == 1);
}
