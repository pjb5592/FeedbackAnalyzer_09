#pragma once

#include <string>
#include <vector>

#include "Feedback.h"

class FileHandler {
public:
    static std::string renderCsv(const std::vector<Feedback>& data);
    static bool saveResult(const std::vector<Feedback>& data, const std::string& path);
};
