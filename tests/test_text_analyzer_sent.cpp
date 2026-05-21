#include <catch2/catch_test_macros.hpp>

#include "fixtures/Expectations.h"
#include "fixtures/FeedbackFixtures.h"
#include "support/LegacyTextAnalyzerAdapter.hpp"

TEST_CASE("FA_TC_01_SentimentEmpty", "[fa-tc][p0][sent][legacy]") {
    LegacyTextAnalyzerAdapter adapter;
    const auto result = adapter.analyzeSentiment(fa_fixtures::emptyFeedbacks());
    fa_expect::expectSentiment(result, 0, 0, 0);
}

TEST_CASE("FA_TC_02_SentimentPositive", "[fa-tc][p0][sent][legacy]") {
    LegacyTextAnalyzerAdapter adapter;
    const auto result = adapter.analyzeSentiment(
        fa_fixtures::singleText(u8"정말 좋아요 최고입니다"));
    fa_expect::expectSentiment(result, 1, 0, 0);
}

TEST_CASE("FA_TC_03_SentimentNegative", "[fa-tc][p0][sent][legacy]") {
    LegacyTextAnalyzerAdapter adapter;
    const auto result = adapter.analyzeSentiment(
        fa_fixtures::singleText(u8"불만 실망 최악"));
    fa_expect::expectSentiment(result, 0, 0, 1);
}
