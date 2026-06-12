#pragma once

#include <map>
#include <string>
#include <vector>

#include "Feedback.h"

class DomainKeywordCounter {
public:
    std::map<std::string, int> count(
        const std::vector<Feedback>& feedbacks) const;
};
