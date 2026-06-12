#pragma once

#include <string>
#include <vector>

#include "Feedback.h"

class Session {
private:
    static std::vector<Feedback> currentFeedbacks;
    static std::vector<Feedback> downloadView_;
    static bool hasFilterView_;

public:
    static const std::vector<Feedback>& getFeedbacks();
    static void appendFeedback(const Feedback& feedback);
    static void appendFeedbacks(const std::vector<Feedback>& feedbacks);

    static void setSessionFeedbacks(const std::vector<Feedback>& feedbacks);
    static void refreshAfterAnalyze(const std::vector<Feedback>& feedbacks);
    static void applyFilterResult(const std::vector<Feedback>& filtered, bool success);

    static const std::vector<Feedback>& getDownloadView();
    static std::string renderDownloadCsv();
};
