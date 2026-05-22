#pragma once
#include <vector>
#include <string>
#include "Feedback.h"

class Session {
private:
    static std::vector<Feedback> currentFeedbacks;
    static std::vector<Feedback> downloadView_;
    static bool hasFilterView_;

public:
    static std::vector<Feedback>& getCurrentFeedbacks() {
        return currentFeedbacks;
    }

    static void setSessionFeedbacks(const std::vector<Feedback>& feedbacks);
    static void refreshAfterAnalyze(const std::vector<Feedback>& feedbacks);
    static void applyFilterResult(const std::vector<Feedback>& filtered, bool success);

    static const std::vector<Feedback>& getDownloadView();
    static std::string renderDownloadCsv();
};
