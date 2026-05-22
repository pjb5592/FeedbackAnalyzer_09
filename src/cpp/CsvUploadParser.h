#pragma once

#include <string>
#include <vector>

#include "Feedback.h"

class CsvUploadParser {
public:
    std::vector<Feedback> parse(const std::string& content) const;
    static std::vector<std::string> parseLine(const std::string& line);
};
