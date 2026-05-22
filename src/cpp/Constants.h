#pragma once

#include <map>
#include <string>
#include <vector>

class Constants {
public:
    static std::map<std::string, std::vector<std::string>> SENTIMENT_KEYWORDS;
    static std::map<std::string, std::map<std::string, std::vector<std::string>>> CATEGORY_KEYWORDS;

    static void init();
    static const std::vector<std::string>& getCategoryNames();
};
