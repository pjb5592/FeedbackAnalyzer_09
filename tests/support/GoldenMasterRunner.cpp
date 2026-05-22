#include "GoldenMasterRunner.hpp"

#include <sstream>

#include "CsvUploadParser.hpp"
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

std::string runGm01() {
    const auto feedbacks = fa_fixtures::mixedSentimentSet();
    DomainSentimentAnalyzer analyzer;
    DomainKeywordCounter counter;
    std::ostringstream out;
    out << formatSentiment(analyzer.analyze(feedbacks));
    out << formatKeywords(counter.count(feedbacks));
    return out.str();
}

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

std::string runGm03() {
    const auto feedbacks = fa_fixtures::mainOnlyShipping();
    DomainKeywordCounter counter;
    DomainFeedbackFilter filter;
    std::ostringstream out;
    out << "kw." << u8"배송" << '=' << counter.count(feedbacks).at(u8"배송") << '\n';
    out << "filter." << u8"배송" << ".count="
        << filter.filter(feedbacks, u8"전체", u8"배송").size() << '\n';
    return out.str();
}

std::string runGm04() {
    CsvUploadParser parser;
    const auto rows = parser.parse(fa_csv::kCsvTextHeader);
    std::ostringstream out;
    out << "csv.count=" << rows.size() << '\n';
    if (rows.size() >= 2) {
        out << "csv.row0=" << rows[0].getText() << '\n';
        out << "csv.row1=" << rows[1].getText() << '\n';
    }
    return out.str();
}

std::string runGm05() {
    const auto feedbacks = fa_fixtures::mixedSentimentSet();
    DomainFeedbackFilter filter;
    const auto filtered = filter.filter(feedbacks, u8"긍정", u8"전체");
    InMemoryDownloadSource source;
    source.setSessionFeedbacks(feedbacks);
    source.applyFilterResult(filtered, true);
    return source.renderCsv();
}

std::string runGm06() {
    const auto feedbacks = fa_fixtures::fiveCategoriesOneEach();
    DomainKeywordCounter counter;
    return formatKeywords(counter.count(feedbacks));
}

std::string runGm07() {
    DomainSentimentAnalyzer analyzer;
    DomainKeywordCounter counter;
    const auto empty = fa_fixtures::emptyFeedbacks();
    std::ostringstream out;
    out << formatSentiment(analyzer.analyze(empty));
    out << formatKeywords(counter.count(empty));
    return out.str();
}

std::string runGm08() {
    std::ostringstream out;
    out << runGm02();
    out << "---\n";
    out << runGm03();
    return out.str();
}

std::string runGm09() { return runGm03(); }

}  // namespace fa_golden
