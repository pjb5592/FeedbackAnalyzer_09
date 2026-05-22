#include "TrendAnalyzer.h"

#include "SentimentClassifier.h"

std::map<std::string, std::map<std::string, int>> TrendAnalyzer::aggregateByDate(
    const std::vector<TrendRow>& rows) const {
    std::map<std::string, std::map<std::string, int>> result;
    for (const auto& row : rows) {
        auto& bucket = result[row.date];
        if (!bucket.count(u8"긍정")) {
            bucket[u8"긍정"] = 0;
            bucket[u8"중립"] = 0;
            bucket[u8"부정"] = 0;
        }
        bucket[fa::classifySentiment(row.text)]++;
    }
    return result;
}
