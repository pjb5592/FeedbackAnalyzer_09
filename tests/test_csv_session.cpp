#include <catch2/catch_test_macros.hpp>

#include "fixtures/FeedbackFixtures.h"
#include "fixtures/SampleCsv.h"
#include "support/CsvUploadParser.hpp"
#include "support/DomainFeedbackFilter.hpp"
#include "support/InMemoryDownloadSource.hpp"

TEST_CASE("FA_TC_33_CsvTextHeader", "[fa-tc][p0][csv][domain]") {
    CsvUploadParser parser;
    const auto feedbacks = parser.parse(fa_csv::kCsvTextHeader);
    REQUIRE(feedbacks.size() == 2);
    REQUIRE(feedbacks[0].getText() == u8"행1");
    REQUIRE(feedbacks[1].getText() == u8"행2");
}

TEST_CASE("FA_TC_34_CsvIdTextColumn", "[fa-tc][p0][csv][def-04][domain]") {
    CsvUploadParser parser;
    const auto feedbacks = parser.parse(fa_csv::kCsvIdText);
    REQUIRE(feedbacks.size() == 1);
    REQUIRE(feedbacks[0].getText() == u8"안녕");
}

TEST_CASE("FA_TC_35_CsvEmpty", "[fa-tc][p0][csv][domain]") {
    CsvUploadParser parser;
    REQUIRE(parser.parse(fa_csv::kCsvEmpty).empty());
}

TEST_CASE("FA_TC_36_CsvQuoted", "[fa-tc][p1][csv][domain]") {
    CsvUploadParser parser;
    const auto feedbacks = parser.parse(fa_csv::kCsvQuoted);
    REQUIRE(feedbacks.size() == 1);
    REQUIRE(feedbacks[0].getText() == "a,b");
}

TEST_CASE("FA_TC_37_CsvCrlf", "[fa-tc][p1][csv][domain]") {
    CsvUploadParser parser;
    const auto feedbacks = parser.parse(fa_csv::kCsvCrlf);
    REQUIRE(feedbacks.size() == 2);
    REQUIRE(feedbacks[0].getText() == "line1");
    REQUIRE(feedbacks[1].getText() == "line2");
}

TEST_CASE("FA_TC_39_DownloadWithoutFilter", "[fa-tc][p0][dl][domain]") {
    InMemoryDownloadSource source;
    source.setSessionFeedbacks(fa_fixtures::mixedSentimentSet());
    const auto csv = source.renderCsv();
    REQUIRE(csv.find("\xEF\xBB\xBF") == 0);
    REQUIRE(csv.find("text\n") != std::string::npos);
    REQUIRE(csv.find(u8"정말 좋아요") != std::string::npos);
}

TEST_CASE("FA_TC_40_DownloadAfterFilter", "[fa-tc][p0][dl][def-03][domain]") {
    const auto feedbacks = fa_fixtures::mixedSentimentSet();
    DomainFeedbackFilter filter;
    const auto filtered = filter.filter(feedbacks, u8"긍정", u8"전체");

    InMemoryDownloadSource source;
    source.setSessionFeedbacks(feedbacks);
    source.applyFilterResult(filtered, true);

    const auto csv = source.renderCsv();
    REQUIRE(csv.find(u8"정말 좋아요") != std::string::npos);
    REQUIRE(csv.find(u8"불만") == std::string::npos);
}

TEST_CASE("FA_TC_41_DownloadFilterEmptyKeep", "[fa-tc][p0][dl][def-03][domain]") {
    const auto feedbacks = fa_fixtures::mixedSentimentSet();
    DomainFeedbackFilter filter;
    const auto positive = filter.filter(feedbacks, u8"긍정", u8"전체");
    const auto empty =
        filter.filter(feedbacks, u8"긍정", u8"품질");

    InMemoryDownloadSource source;
    source.setSessionFeedbacks(feedbacks);
    source.applyFilterResult(positive, true);
    const auto csvAfterFilter = source.renderCsv();

    source.applyFilterResult(empty, false);
    REQUIRE(source.renderCsv() == csvAfterFilter);
}

TEST_CASE("FA_TC_42_DownloadStale", "[fa-tc][p1][dl][def-03][domain]") {
    auto feedbacks = fa_fixtures::mixedSentimentSet();
    DomainFeedbackFilter filter;
    const auto positive = filter.filter(feedbacks, u8"긍정", u8"전체");

    InMemoryDownloadSource source;
    source.setSessionFeedbacks(feedbacks);
    source.applyFilterResult(positive, true);
    REQUIRE(source.renderCsv().find(u8"불만") == std::string::npos);

    feedbacks.push_back(Feedback(u8"새 피드백 추가"));
    source.refreshAfterAnalyze(feedbacks);
    const auto csv = source.renderCsv();
    REQUIRE(csv.find(u8"새 피드백 추가") != std::string::npos);
    REQUIRE(csv.find(u8"불만") != std::string::npos);
}

TEST_CASE("FA_TC_43_DownloadUtf8Bom", "[fa-tc][p1][dl][domain]") {
    InMemoryDownloadSource source;
    source.setSessionFeedbacks(fa_fixtures::singleText(u8"test"));
    const auto csv = source.renderCsv();
    REQUIRE(csv.size() >= 3);
    REQUIRE(static_cast<unsigned char>(csv[0]) == 0xEF);
    REQUIRE(static_cast<unsigned char>(csv[1]) == 0xBB);
    REQUIRE(static_cast<unsigned char>(csv[2]) == 0xBF);
}
