#include "DomainSentimentAnalyzer.hpp"

#include "Constants.h"
#include "KeywordMatcher.hpp"

std::map<std::string, int> DomainSentimentAnalyzer::analyze(
    const std::vector<Feedback>& feedbacks) const {
    std::map<std::string, int> result;
    result[u8"긍정"] = 0;
    result[u8"중립"] = 0;
    result[u8"부정"] = 0;

    for (const auto& feedback : feedbacks) {
        const std::string& text = feedback.getText();
        std::string label = u8"중립";
        if (fa_support::containsAny(text,
                                    Constants::SENTIMENT_KEYWORDS[u8"긍정"])) {
            label = u8"긍정";
        } else if (fa_support::containsAny(
                       text, Constants::SENTIMENT_KEYWORDS[u8"부정"])) {
            label = u8"부정";
        }
        result[label]++;
    }

    return result;
}
