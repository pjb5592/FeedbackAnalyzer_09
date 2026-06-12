#pragma once

#include <string>
#include <vector>

#include "Feedback.h"

class InMemoryDownloadSource {
public:
    void setSessionFeedbacks(const std::vector<Feedback>& feedbacks);
    void refreshAfterAnalyze(const std::vector<Feedback>& feedbacks);
    void applyFilterResult(const std::vector<Feedback>& filtered, bool success);

    std::string renderCsv() const;

private:
    std::vector<Feedback> sessionFeedbacks_;
    std::vector<Feedback> downloadView_;
    bool hasFilterView_ = false;
};
