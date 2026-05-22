#pragma once
#include <vector>
#include <map>
#include <string>
#include "Feedback.h"

class Session {
private:
    static std::vector<Feedback> currentFeedbacks;
    static std::vector<Feedback> downloadView_;
    static bool hasFilterView_;
    static std::map<std::string, std::string> internalData;
    static std::map<std::string, std::string> filterOptions;

public:
    static void initSessionStateUgly() {
        // already initialized as static
    }

    static std::vector<Feedback>& getOldDataFromSession(const std::string& key) {
        return currentFeedbacks;
    }

    static void updateCurrentFeedbacks(const std::vector<Feedback>& feedbacks) {
        currentFeedbacks = feedbacks;
    }

    static std::vector<Feedback>& getCurrentFeedbacks() {
        return currentFeedbacks;
    }

    static void setSessionFeedbacks(const std::vector<Feedback>& feedbacks);
    static void refreshAfterAnalyze(const std::vector<Feedback>& feedbacks);
    static void applyFilterResult(const std::vector<Feedback>& filtered, bool success);

    static const std::vector<Feedback>& getDownloadView();
    static std::string renderDownloadCsv();
};
