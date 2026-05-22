#pragma once

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "Feedback.h"
#include "Filters.h"

class LegacyFiltersAdapter {
    Filters filters_;

public:
    std::vector<Feedback> filter(const std::vector<Feedback>& dataList,
                                 const std::string& sentimentFilter,
                                 const std::string& keywordFilter) {
        std::ostringstream capture;
        auto* previous = std::cout.rdbuf(capture.rdbuf());
        std::vector<Feedback> result =
            filters_.fil(dataList, sentimentFilter, keywordFilter);
        std::cout.rdbuf(previous);
        return result;
    }
};
