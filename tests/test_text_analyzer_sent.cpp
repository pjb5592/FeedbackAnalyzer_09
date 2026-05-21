#include <catch2/catch_test_macros.hpp>

#include "fixtures/Expectations.h"
#include "fixtures/FeedbackFixtures.h"
#include "support/LegacyTextAnalyzerAdapter.hpp"

TEST_CASE("FA_TC_01_SentimentEmpty", "[fa-tc][p0][sent][legacy]") {
    LegacyTextAnalyzerAdapter adapter;
    const auto result = adapter.analyzeSentiment(fa_fixtures::emptyFeedbacks());
    fa_expect::expectSentiment(result, 0, 0, 0);
}
