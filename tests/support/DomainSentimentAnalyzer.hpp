#pragma once

#include <map>
#include <string>
#include <vector>

#include "Feedback.h"

class DomainSentimentAnalyzer {
public:
    std::map<std::string, int> analyze(
        const std::vector<Feedback>& feedbacks) const;
};
