#pragma once

#include <map>
#include <string>
#include <vector>

#include "Feedback.h"

class HtmlRenderer {
public:
    static std::string escapeHtml(const std::string& s);

    static std::string renderPage(const std::string& success,
                                  const std::string& warning,
                                  const std::string& error,
                                  const std::map<std::string, int>& sentimentResults,
                                  const std::map<std::string, int>& keywordResults,
                                  const std::vector<Feedback>& feedbacks);
};
