#include "TextAnalyzer.h"

#include "Constants.h"
#include "KeywordMatcher.h"

namespace {

std::string classifySentiment(const std::string& text) {
    if (fa::containsAny(text, Constants::SENTIMENT_KEYWORDS[u8"긍정"])) {
        return u8"긍정";
    }
    if (fa::containsAny(text, Constants::SENTIMENT_KEYWORDS[u8"부정"])) {
        return u8"부정";
    }
    return u8"중립";
}

}  // namespace

std::map<std::string, int> TextAnalyzer::analyzeSentiment(
    const std::vector<Feedback>& feedbacks) {
    std::map<std::string, int> result;
    result[u8"긍정"] = 0;
    result[u8"중립"] = 0;
    result[u8"부정"] = 0;

    for (const auto& feedback : feedbacks) {
        result[classifySentiment(feedback.getText())]++;
    }

    return result;
}

std::map<std::string, int> TextAnalyzer::countKeywords(
    const std::vector<Feedback>& feedbacks) {
    std::map<std::string, int> result;
    for (const auto& entry : Constants::CATEGORY_KEYWORDS) {
        result[entry.first] = 0;
    }

    for (const auto& feedback : feedbacks) {
        const std::string& text = feedback.getText();
        for (const auto& entry : Constants::CATEGORY_KEYWORDS) {
            const std::string& category = entry.first;
            if (entry.second.count("main")) {
                const auto& keywords = entry.second.at("main");
                if (fa::containsAny(text, keywords)) {
                    result[category]++;
                }
            }
        }
    }

    return result;
}
