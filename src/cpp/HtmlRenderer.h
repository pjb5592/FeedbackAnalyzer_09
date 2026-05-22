#pragma once

#include <map>
#include <string>
#include <vector>

class HtmlRenderer {
public:
    struct PageModel {
        std::string success;
        std::string warning;
        std::string error;
        std::map<std::string, int> sentimentResults;
        std::map<std::string, int> keywordResults;
    };

    static std::string escapeHtml(const std::string& s);
    static std::string renderPage(const PageModel& model);
};
