#include <catch2/catch_test_macros.hpp>

#include "Feedback.h"
#include "CsvUploadParser.h"
#include "Filters.h"
#include "TextAnalyzer.h"
#include "fixtures/FeedbackFixtures.h"
#include "fixtures/SampleCsv.h"
#include "support/CsvUploadParser.hpp"
#include "support/DomainFeedbackFilter.hpp"
#include "support/DomainSentimentAnalyzer.hpp"
#include "support/DomainKeywordCounter.hpp"
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

TEST_CASE("FA_TC_Cov_SentimentAllLabels", "[coverage][boundary][fa-tc]") {
    DomainSentimentAnalyzer analyzer;
    const std::vector<Feedback> feedbacks = {
        Feedback(u8"정말 좋아요"), Feedback(u8"배송 지연"), Feedback(u8"불만 실망")};
    const auto result = analyzer.analyze(feedbacks);
    REQUIRE(result.at(u8"긍정") == 1);
    REQUIRE(result.at(u8"중립") == 1);
    REQUIRE(result.at(u8"부정") == 1);
}

TEST_CASE("FA_TC_Cov_KeywordNoMatch", "[coverage][boundary][fa-tc]") {
    DomainKeywordCounter counter;
    const auto result = counter.count(fa_fixtures::singleText(u8"일반 문장"));
    REQUIRE(result.at(u8"배송") == 0);
}

TEST_CASE("FA_TC_Cov_FilterKeywordShipping", "[coverage][boundary][fa-tc]") {
    DomainFeedbackFilter filter;
    const auto rows =
        filter.filter(fa_fixtures::singleText(u8"배송이 빨라요"), u8"전체", u8"배송");
    REQUIRE(rows.size() == 1);
    REQUIRE(filter
               .filter(fa_fixtures::singleText(u8"일반 문장"), u8"전체", u8"배송")
               .empty());
}

TEST_CASE("FA_TC_Cov_ApplyFilterNotSuccess", "[coverage][boundary][fa-tc]") {
    InMemoryDownloadSource source;
    source.setSessionFeedbacks(fa_fixtures::mixedSentimentSet());
    const auto before = source.renderCsv();
    source.applyFilterResult(fa_fixtures::mixedSentimentSet(), false);
    REQUIRE(source.renderCsv() == before);
}

TEST_CASE("FA_TC_Cov_LegacyAnalyzerSmoke", "[coverage][boundary][fa-tc]") {
    TextAnalyzer analyzer;
    Filters filters;
    const auto feedbacks = fa_fixtures::singleText(u8"배송이 빨라요");
    const auto sent = analyzer.analyzeSentiment(feedbacks);
    REQUIRE(sent.at(u8"중립") == 1);
    REQUIRE(analyzer.countKeywords(feedbacks).at(u8"배송") >= 1);
    const auto filtered = filters.filterFeedbacks(feedbacks, u8"중립", u8"배송");
    REQUIRE(filtered.size() == 1);
    REQUIRE(filters.filterFeedbacks(feedbacks, u8"전체", u8"없는카테고리").empty());
}

TEST_CASE("FA_TC_Cov_ProductionCsvNoHeaderLine", "[coverage][boundary][fa-tc]") {
    CsvUploadParser parser;
    REQUIRE(parser.parse("\n").empty());
}

TEST_CASE("FA_TC_Cov_FilterSentimentMismatch", "[coverage][boundary][fa-tc]") {
    DomainFeedbackFilter filter;
    REQUIRE(filter
               .filter(fa_fixtures::singleText(u8"정말 좋아요"), u8"부정", u8"전체")
               .empty());
}
