#include <catch2/catch_test_macros.hpp>

#include "fixtures/FeedbackFixtures.h"
#include "support/DomainFeedbackFilter.hpp"
#include "support/DomainKeywordCounter.hpp"
#include "support/DomainSentimentAnalyzer.hpp"

TEST_CASE("FA_TC_16_KeywordMainOnlyMismatch", "[fa-tc][p0][kw][def-02][domain]") {
    const auto feedbacks = fa_fixtures::mainOnlyQuality();
    DomainKeywordCounter counter;
    DomainFeedbackFilter filter;

    const auto kw = counter.count(feedbacks);
    const auto filtered = filter.filter(feedbacks, u8"전체", u8"품질");

    REQUIRE(kw.at(u8"품질") >= 1);
    REQUIRE(filtered.size() >= 1);
}

TEST_CASE("FA_TC_17_FilterNeutralZero", "[fa-tc][p0][fil][def-01][domain]") {
    const auto feedbacks = fa_fixtures::def01NeutralAmbiguous();
    DomainFeedbackFilter filter;
    const auto filtered = filter.filter(feedbacks, u8"중립", u8"전체");
    REQUIRE(filtered.size() == 1);
}

TEST_CASE("FA_TC_29_ConsistencySentFilNeutral", "[fa-tc][p0][def-01][domain]") {
    const auto feedbacks = fa_fixtures::def01NeutralAmbiguous();
    DomainSentimentAnalyzer analyzer;
    DomainFeedbackFilter filter;

    const auto sent = analyzer.analyze(feedbacks);
    const auto filtered = filter.filter(feedbacks, u8"중립", u8"전체");

    REQUIRE(sent.at(u8"중립") == 1);
    REQUIRE(filtered.size() == 1);
}
