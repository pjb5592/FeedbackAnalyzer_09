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

TEST_CASE("FA_TC_31_ConsistencyFilteredStats", "[fa-tc][p1][domain]") {
    const auto feedbacks = fa_fixtures::mixedSentimentSet();
    DomainFeedbackFilter filter;
    DomainSentimentAnalyzer analyzer;

    const auto filtered = filter.filter(feedbacks, u8"긍정", u8"전체");
    const auto sent = analyzer.analyze(filtered);
    const int total = sent.at(u8"긍정") + sent.at(u8"중립") + sent.at(u8"부정");
    REQUIRE(total == static_cast<int>(filtered.size()));
}

TEST_CASE("FA_TC_30_ConsistencyKwFilMain", "[fa-tc][p0][def-02][domain]") {
    const auto feedbacks = fa_fixtures::mainOnlyShipping();
    DomainKeywordCounter counter;
    DomainFeedbackFilter filter;

    const auto kw = counter.count(feedbacks);
    const auto filtered = filter.filter(feedbacks, u8"전체", u8"배송");

    REQUIRE(kw.at(u8"배송") >= 1);
    REQUIRE(filtered.size() >= 1);
}

TEST_CASE("FA_TC_32_ConsistencyPostRefactor", "[fa-tc][p0][domain]") {
    DomainSentimentAnalyzer analyzer;
    DomainKeywordCounter counter;
    DomainFeedbackFilter filter;

    const auto def01 = fa_fixtures::def01NeutralAmbiguous();
    const auto sent = analyzer.analyze(def01);
    const auto filNeutral = filter.filter(def01, u8"중립", u8"전체");
    REQUIRE(sent.at(u8"중립") == 1);
    REQUIRE(filNeutral.size() == 1);

    const auto shipping = fa_fixtures::mainOnlyShipping();
    const auto kw = counter.count(shipping);
    const auto filShip = filter.filter(shipping, u8"전체", u8"배송");
    REQUIRE(kw.at(u8"배송") >= 1);
    REQUIRE(filShip.size() >= 1);
}
