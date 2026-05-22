#include <catch2/catch_test_macros.hpp>

#include "fixtures/FeedbackFixtures.h"
#include "support/DomainFeedbackFilter.hpp"
#include "support/DomainSentimentAnalyzer.hpp"

TEST_CASE("FA_TC_18_FilterNeutralAll", "[fa-tc][p0][fil][domain]") {
    DomainFeedbackFilter filter;
    const auto filtered =
        filter.filter(fa_fixtures::def01NeutralAmbiguous(), u8"전체", u8"전체");
    REQUIRE(filtered.size() == 1);
}

TEST_CASE("FA_TC_19_FilterPositive", "[fa-tc][p0][fil][domain]") {
    DomainFeedbackFilter filter;
    const auto filtered = filter.filter(
        fa_fixtures::singleText(u8"정말 좋아요"), u8"긍정", u8"전체");
    REQUIRE(filtered.size() == 1);
}

TEST_CASE("FA_TC_20_FilterNegative", "[fa-tc][p0][fil][domain]") {
    DomainFeedbackFilter filter;
    const auto filtered = filter.filter(
        fa_fixtures::singleText(u8"불만입니다"), u8"부정", u8"전체");
    REQUIRE(filtered.size() == 1);
}

TEST_CASE("FA_TC_21_FilterSentimentAll", "[fa-tc][p0][fil][domain]") {
    const auto feedbacks = fa_fixtures::mixedSentimentSet();
    DomainFeedbackFilter filter;
    const auto filtered = filter.filter(feedbacks, u8"전체", u8"전체");
    REQUIRE(filtered.size() == feedbacks.size());
}

TEST_CASE("FA_TC_22_FilterOverlapOk", "[fa-tc][p0][fil][def-01][domain]") {
    const auto feedbacks = fa_fixtures::singleText(u8"괜찮아요");
    DomainSentimentAnalyzer analyzer;
    DomainFeedbackFilter filter;

    const auto sent = analyzer.analyze(feedbacks);
    const auto positive =
        filter.filter(feedbacks, u8"긍정", u8"전체");
    const auto neutral =
        filter.filter(feedbacks, u8"중립", u8"전체");

    REQUIRE(sent.at(u8"중립") == 1);
    REQUIRE(positive.size() == 0);
    REQUIRE(neutral.size() == 1);
}

TEST_CASE("FA_TC_23_FilterCategoryShipping", "[fa-tc][p0][fil][domain]") {
    DomainFeedbackFilter filter;
    const auto filtered = filter.filter(
        fa_fixtures::singleText(u8"배송이 빨라요"), u8"전체", u8"배송");
    REQUIRE(filtered.size() >= 1);
}

TEST_CASE("FA_TC_24_FilterMainOnlyZero", "[fa-tc][p0][fil][def-02][domain]") {
    DomainFeedbackFilter filter;
    const auto filtered = filter.filter(fa_fixtures::mainOnlyShipping(), u8"전체",
                                        u8"배송");
    REQUIRE(filtered.size() >= 1);
}

TEST_CASE("FA_TC_25_FilterKeywordAll", "[fa-tc][p0][fil][domain]") {
    const auto feedbacks = fa_fixtures::mixedSentimentSet();
    DomainFeedbackFilter filter;
    const auto filtered = filter.filter(feedbacks, u8"긍정", u8"전체");
    REQUIRE(filtered.size() == 1);
}

TEST_CASE("FA_TC_26_FilterIntersection", "[fa-tc][p0][fil][domain]") {
    DomainFeedbackFilter filter;
    const auto filtered = filter.filter(
        fa_fixtures::singleText(u8"정말 좋아요 최고입니다"), u8"긍정", u8"품질");
    REQUIRE(filtered.size() == 0);
}

TEST_CASE("FA_TC_27_FilterUnknownKeyword", "[fa-tc][p1][fil][domain]") {
    DomainFeedbackFilter filter;
    const auto filtered = filter.filter(
        fa_fixtures::singleText(u8"배송이 빨라요"), u8"전체", u8"unknown_cat");
    REQUIRE(filtered.empty());
}

TEST_CASE("FA_TC_28_FilterFiveCategories", "[fa-tc][p1][fil][domain]") {
    const auto feedbacks = fa_fixtures::fiveCategoriesOneEach();
    DomainFeedbackFilter filter;
    const std::string categories[] = {u8"배송", u8"품질", u8"가격", u8"서비스",
                                      u8"사용성"};
    for (const auto& category : categories) {
        const auto filtered = filter.filter(feedbacks, u8"전체", category);
        REQUIRE(filtered.size() >= 1);
    }
}
