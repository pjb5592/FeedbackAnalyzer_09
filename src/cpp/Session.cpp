#include "Session.h"

#include "FileHandler.h"

std::vector<Feedback> Session::currentFeedbacks;
std::vector<Feedback> Session::downloadView_;
bool Session::hasFilterView_ = false;

const std::vector<Feedback>& Session::getFeedbacks() {
    return currentFeedbacks;
}

void Session::appendFeedback(const Feedback& feedback) {
    currentFeedbacks.push_back(feedback);
}

void Session::appendFeedbacks(const std::vector<Feedback>& feedbacks) {
    currentFeedbacks.insert(currentFeedbacks.end(), feedbacks.begin(), feedbacks.end());
}

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
    return FileHandler::renderCsv(downloadView_);
}
