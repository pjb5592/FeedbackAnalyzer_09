#pragma once

#include <string>
#include <vector>

struct TrendRow {
    std::string date;
    std::string text;
};

class TrendCsvParser {
public:
    std::vector<TrendRow> parse(const std::string& content) const;
};
