#include "DomainFeedbackFilter.hpp"

#include "Constants.h"
#include "KeywordMatcher.hpp"
#include "SentimentClassifier.h"

std::vector<Feedback> DomainFeedbackFilter::filter(
    const std::vector<Feedback>& feedbacks, const std::string& sentimentFilter,
    const std::string& keywordFilter) const {
    std::vector<Feedback> afterSentiment;

    if (sentimentFilter != u8"전체") {
        for (const auto& feedback : feedbacks) {
            if (fa::classifySentiment(feedback.getText()) == sentimentFilter) {
                afterSentiment.push_back(feedback);
            }
        }
    } else {
        afterSentiment = feedbacks;
    }

    if (keywordFilter == u8"전체") {
        return afterSentiment;
    }

    std::vector<Feedback> result;
    if (!Constants::CATEGORY_KEYWORDS.count(keywordFilter)) {
        return result;
    }

    const auto& categoryMap = Constants::CATEGORY_KEYWORDS.at(keywordFilter);
    for (const auto& feedback : afterSentiment) {
        const std::string& text = feedback.getText();
        for (const auto& subEntry : categoryMap) {
            if (fa_support::containsAny(text, subEntry.second)) {
                result.push_back(feedback);
                break;
            }
        }
    }

    return result;
}
