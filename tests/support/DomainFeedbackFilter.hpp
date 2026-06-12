#pragma once

#include <string>
#include <vector>

#include "Feedback.h"

class DomainFeedbackFilter {
public:
    std::vector<Feedback> filter(const std::vector<Feedback>& feedbacks,
                                 const std::string& sentimentFilter,
                                 const std::string& keywordFilter) const;
};
