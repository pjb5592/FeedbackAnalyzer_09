#pragma once

#include <map>
#include <string>
#include <vector>

#include "TrendCsvParser.h"

class TrendAnalyzer {
public:
    std::map<std::string, std::map<std::string, int>> aggregateByDate(
        const std::vector<TrendRow>& rows) const;
};
