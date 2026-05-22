#pragma once

#include <map>
#include <string>
#include <vector>

#include "Feedback.h"

class TextAnalyzer {
public:
    std::map<std::string, int> analyzeSentiment(const std::vector<Feedback>& feedbacks);
    std::map<std::string, int> countKeywords(const std::vector<Feedback>& feedbacks);
};
