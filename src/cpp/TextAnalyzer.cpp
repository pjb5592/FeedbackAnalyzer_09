#include "TextAnalyzer.h"

#include "Constants.h"
#include "KeywordMatcher.h"
#include "SentimentClassifier.h"

std::map<std::string, int> TextAnalyzer::analyzeSentiment(
    const std::vector<Feedback>& feedbacks) {
    std::map<std::string, int> result;
    result[u8"긍정"] = 0;
    result[u8"중립"] = 0;
    result[u8"부정"] = 0;

    for (const auto& feedback : feedbacks) {
        result[fa::classifySentiment(feedback.getText())]++;
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
