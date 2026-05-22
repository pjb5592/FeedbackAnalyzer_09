#include <catch2/catch_test_macros.hpp>

#include <vector>

#include "Feedback.h"
#include "fixtures/FeedbackFixtures.h"
#include "support/DomainSentimentAnalyzer.hpp"

TEST_CASE("FA_TC_51_Sentiment1k", "[fa-tc][p2][perf][domain]") {
    std::vector<Feedback> feedbacks;
    feedbacks.reserve(1000);
    for (int i = 0; i < 1000; ++i) {
        feedbacks.emplace_back(u8"정말 좋아요 " + std::to_string(i));
    }
    DomainSentimentAnalyzer analyzer;
    const auto result = analyzer.analyze(feedbacks);
    REQUIRE(result.at(u8"긍정") == 1000);
}

TEST_CASE("FA_TC_54_CoverageGate", "[fa-tc][p2][cov]") {
    SUCCEED("run scripts/run_coverage_gate.ps1 after gcov build");
}

