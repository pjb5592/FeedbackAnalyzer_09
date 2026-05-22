#include "GoldenMasterRunner.hpp"

#include <sstream>

#include "CsvUploadParser.hpp"
#include "TrendAnalyzer.h"
#include "TrendCsvParser.h"
#include "DomainFeedbackFilter.hpp"
#include "DomainKeywordCounter.hpp"
#include "DomainSentimentAnalyzer.hpp"
#include "InMemoryDownloadSource.hpp"
#include "fixtures/FeedbackFixtures.h"
#include "fixtures/SampleCsv.h"

namespace {

std::string formatSentiment(const std::map<std::string, int>& sent) {
    std::ostringstream out;
    out << "sent.positive=" << sent.at(u8"긍정") << '\n';
    out << "sent.neutral=" << sent.at(u8"중립") << '\n';
    out << "sent.negative=" << sent.at(u8"부정") << '\n';
    return out.str();
}

std::string formatKeywords(const std::map<std::string, int>& kw) {
    std::ostringstream out;
    const std::string categories[] = {u8"배송", u8"품질", u8"가격", u8"서비스",
                                      u8"사용성"};
    for (const auto& cat : categories) {
        out << "kw." << cat << '=' << kw.at(cat) << '\n';
    }
    return out.str();
}

}  // namespace

namespace fa_golden {

// GM-01: analyze — mixed feedbacks → sentiment + keyword aggregates (Domain).
std::string runGm01() {
    const auto feedbacks = fa_fixtures::mixedSentimentSet();
    DomainSentimentAnalyzer analyzer;
    DomainKeywordCounter counter;
    std::ostringstream out;
    out << formatSentiment(analyzer.analyze(feedbacks));
    out << formatKeywords(counter.count(feedbacks));
    return out.str();
}

// GM-02: filter neutral — DEF-01 representative sentence (Domain).
std::string runGm02() {
    const auto feedbacks = fa_fixtures::def01NeutralAmbiguous();
    DomainSentimentAnalyzer analyzer;
    DomainFeedbackFilter filter;
    std::ostringstream out;
    out << formatSentiment(analyzer.analyze(feedbacks));
    out << "filter.neutral.count="
        << filter.filter(feedbacks, u8"중립", u8"전체").size() << '\n';
    return out.str();
}

// GM-03: download CSV — positive filter then InMemoryDownloadSource bytes (Domain).
std::string runGm03() {
    const auto feedbacks = fa_fixtures::mixedSentimentSet();
    DomainFeedbackFilter filter;
    const auto filtered = filter.filter(feedbacks, u8"긍정", u8"전체");
    InMemoryDownloadSource source;
    source.setSessionFeedbacks(feedbacks);
    source.applyFilterResult(filtered, true);
    return source.renderCsv();
}

// GM-04: upload CSV — kCsvTextHeader parse snapshot (Domain).
std::string runGm04() {
    CsvUploadParser parser;
    const auto rows = parser.parse(fa_csv::kCsvTextHeader);
    std::ostringstream out;
    out << "upload.count=" << rows.size() << '\n';
    if (rows.size() >= 2) {
        out << "upload.row0=" << rows[0].getText() << '\n';
        out << "upload.row1=" << rows[1].getText() << '\n';
    }
    return out.str();
}

// GM-10: trend CSV — date buckets with weighted sentiment (feature).
std::string runGm10() {
    TrendCsvParser parser;
    const auto rows = parser.parse(fa_csv::kCsvTrendTwoDays);
    TrendAnalyzer analyzer;
    const auto byDate = analyzer.aggregateByDate(rows);
    std::ostringstream out;
    for (const auto& dayEntry : byDate) {
        const auto& bucket = dayEntry.second;
        out << "trend." << dayEntry.first << ".positive="
            << bucket.at(u8"긍정") << '\n';
        out << "trend." << dayEntry.first << ".neutral="
            << bucket.at(u8"중립") << '\n';
        out << "trend." << dayEntry.first << ".negative="
            << bucket.at(u8"부정") << '\n';
    }
    return out.str();
}

}  // namespace fa_golden
