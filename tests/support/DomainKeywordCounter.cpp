#include "DomainKeywordCounter.hpp"

#include "Constants.h"
#include "KeywordMatcher.hpp"

std::map<std::string, int> DomainKeywordCounter::count(
    const std::vector<Feedback>& feedbacks) const {
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
                if (fa_support::containsAny(text, keywords)) {
                    result[category]++;
                }
            }
        }
    }

    return result;
}
