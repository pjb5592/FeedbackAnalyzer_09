#include <catch2/catch_test_macros.hpp>

#include <cstdio>
#include <fstream>
#include <string>

#include "Constants.h"
#include "FileHandler.h"
#include "SentimentClassifier.h"
#include "SentimentKeywordStore.h"
#include "TrendAnalyzer.h"
#include "TrendCsvParser.h"
#include "fixtures/FeedbackFixtures.h"
#include "fixtures/SampleCsv.h"
#include "support/DomainSentimentAnalyzer.hpp"

TEST_CASE("FA_TC_56_WeightedSentimentNegation", "[fa-tc][p2][feature][sent]") {
    REQUIRE(fa::classifySentiment(u8"나쁘지 않아요") == u8"중립");
}

TEST_CASE("FA_TC_56b_WeightedSentimentMixedPolarity",
          "[fa-tc][p2][feature][sent]") {
    REQUIRE(fa::classifySentiment(u8"최고입니다 정말 불만 실망 최악") ==
            u8"부정");
}

TEST_CASE("FA_TC_57_FileHandlerSaveResult", "[fa-tc][p2][feature][file]") {
    const auto feedbacks = fa_fixtures::mixedSentimentSet();
    const std::string path = "fa_tc57_save_result.csv";
    REQUIRE(FileHandler::saveResult(feedbacks, path));

    std::ifstream in(path, std::ios::binary);
    REQUIRE(in.good());
    std::string content((std::istreambuf_iterator<char>(in)),
                        std::istreambuf_iterator<char>());
    REQUIRE(content.substr(0, 3) == "\xEF\xBB\xBF");
    REQUIRE(content.find("text\n") != std::string::npos);
    REQUIRE(content.find(u8"정말 좋아요") != std::string::npos);
    REQUIRE(content.find(u8"불만 실망") != std::string::npos);
    std::remove(path.c_str());
}

TEST_CASE("FA_TC_55_TrendCsvAggregate", "[fa-tc][p2][feature][trend]") {
    TrendCsvParser parser;
    const auto rows = parser.parse(fa_csv::kCsvTrendTwoDays);
    REQUIRE(rows.size() == 2);
    REQUIRE(rows[0].date == "2024-01-01");
    REQUIRE(rows[1].date == "2024-01-02");

    TrendAnalyzer analyzer;
    const auto byDate = analyzer.aggregateByDate(rows);
    REQUIRE(byDate.at("2024-01-01").at(u8"긍정") == 1);
    REQUIRE(byDate.at("2024-01-01").at(u8"부정") == 0);
    REQUIRE(byDate.at("2024-01-02").at(u8"부정") == 1);
    REQUIRE(byDate.at("2024-01-02").at(u8"긍정") == 0);
}

TEST_CASE("FA_TC_58_SentimentKeywordDb", "[fa-tc][p2][feature][db]") {
    const std::string path = "fa_tc58_sentiment_db.txt";
    {
        std::ofstream out(path);
        out << u8"긍정:테스트좋음\n";
    }

    SentimentKeywordStore store;
    REQUIRE(store.loadFromFile(path));
    store.applyToConstants();
    REQUIRE(fa::classifySentiment(u8"테스트좋음 문장") == u8"긍정");

    Constants::init();
    std::remove(path.c_str());
}
