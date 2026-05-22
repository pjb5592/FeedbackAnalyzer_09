#include <catch2/catch_test_macros.hpp>

#include "fixtures/FeedbackFixtures.h"
#include "fixtures/SampleCsv.h"
#include "support/CsvUploadParser.hpp"
#include "support/DomainFeedbackFilter.hpp"
#include "support/InMemoryDownloadSource.hpp"

TEST_CASE("FA_TC_Cov_CsvHeaderOnly", "[coverage][boundary][fa-tc]") {
    CsvUploadParser parser;
    REQUIRE(parser.parse(fa_csv::kCsvHeaderOnly).empty());
}

TEST_CASE("FA_TC_Cov_CsvNoNewline", "[coverage][boundary][fa-tc]") {
    CsvUploadParser parser;
    REQUIRE(parser.parse("text").empty());
}

TEST_CASE("FA_TC_Cov_CsvBlankLineSkip", "[coverage][boundary][fa-tc]") {
    CsvUploadParser parser;
    const auto feedbacks = parser.parse(fa_csv::kCsvBlankLine);
    REQUIRE(feedbacks.size() == 2);
    REQUIRE(feedbacks[0].getText() == "row1");
    REQUIRE(feedbacks[1].getText() == "row2");
}

TEST_CASE("FA_TC_Cov_ApplyFilterEmptySuccess", "[coverage][boundary][fa-tc]") {
    InMemoryDownloadSource source;
    source.setSessionFeedbacks(fa_fixtures::mixedSentimentSet());
    const auto before = source.renderCsv();
    source.applyFilterResult({}, true);
    REQUIRE(source.renderCsv() == before);
}

TEST_CASE("FA_TC_Cov_SessionKeepsFilterView", "[coverage][boundary][fa-tc]") {
    auto feedbacks = fa_fixtures::mixedSentimentSet();
    DomainFeedbackFilter filter;
    const auto positive = filter.filter(feedbacks, u8"긍정", u8"전체");

    InMemoryDownloadSource source;
    source.setSessionFeedbacks(feedbacks);
    source.applyFilterResult(positive, true);
    const auto csvFiltered = source.renderCsv();

    feedbacks.emplace_back(u8"추가 피드백");
    source.setSessionFeedbacks(feedbacks);
    REQUIRE(source.renderCsv() == csvFiltered);
}

TEST_CASE("FA_TC_Cov_FilterSentimentBranch", "[coverage][boundary][fa-tc]") {
    DomainFeedbackFilter filter;
    const auto subset = filter.filter(
        fa_fixtures::singleText(u8"불만 실망"), u8"부정", u8"전체");
    REQUIRE(subset.size() == 1);
}

TEST_CASE("FA_TC_Cov_FilterAllAll", "[coverage][boundary][fa-tc]") {
    DomainFeedbackFilter filter;
    const auto all = filter.filter(fa_fixtures::mixedSentimentSet(), u8"전체", u8"전체");
    REQUIRE(all.size() == fa_fixtures::mixedSentimentSet().size());
}

TEST_CASE("FA_TC_Cov_FilterUnknownCategory", "[coverage][boundary][fa-tc]") {
    DomainFeedbackFilter filter;
    REQUIRE(
        filter.filter(fa_fixtures::singleText(u8"일반 문장"), u8"전체", u8"없는카테고리")
            .empty());
}
