#include "Session.h"

#include <sstream>

std::vector<Feedback> Session::currentFeedbacks;
std::vector<Feedback> Session::downloadView_;
bool Session::hasFilterView_ = false;

void Session::setSessionFeedbacks(const std::vector<Feedback>& feedbacks) {
    currentFeedbacks = feedbacks;
    if (!hasFilterView_) {
        downloadView_ = currentFeedbacks;
    }
}

void Session::refreshAfterAnalyze(const std::vector<Feedback>& feedbacks) {
    currentFeedbacks = feedbacks;
    downloadView_ = currentFeedbacks;
    hasFilterView_ = false;
}

void Session::applyFilterResult(const std::vector<Feedback>& filtered, bool success) {
    if (success && !filtered.empty()) {
        downloadView_ = filtered;
        hasFilterView_ = true;
    }
}

const std::vector<Feedback>& Session::getDownloadView() {
    return downloadView_;
}

std::string Session::renderDownloadCsv() {
    std::ostringstream csv;
    csv << "\xEF\xBB\xBF";
    csv << "text\n";
    for (const auto& item : downloadView_) {
        csv << item.getText() << '\n';
    }
    return csv.str();
}
