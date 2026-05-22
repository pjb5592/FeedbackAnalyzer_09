#pragma once

#include <map>
#include <string>
#include <vector>

#include "Feedback.h"
#include "TextAnalyzer.h"

class LegacyTextAnalyzerAdapter {
    TextAnalyzer analyzer_;

public:
    std::map<std::string, int> analyzeSentiment(
        const std::vector<Feedback>& feedbacks) {
        return analyzer_.sent(feedbacks);
    }

    std::map<std::string, int> countKeywords(
        const std::vector<Feedback>& feedbacks) {
        return analyzer_.kw(feedbacks);
    }
};
