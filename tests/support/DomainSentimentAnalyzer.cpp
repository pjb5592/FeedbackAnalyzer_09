#include "DomainSentimentAnalyzer.hpp"

#include "SentimentClassifier.h"

std::map<std::string, int> DomainSentimentAnalyzer::analyze(
    const std::vector<Feedback>& feedbacks) const {
    std::map<std::string, int> result;
    result[u8"긍정"] = 0;
    result[u8"중립"] = 0;
    result[u8"부정"] = 0;

    for (const auto& feedback : feedbacks) {
        result[fa::classifySentiment(feedback.getText())]++;
    }

    return result;
}
