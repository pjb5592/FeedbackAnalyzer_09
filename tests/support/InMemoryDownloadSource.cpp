#include "InMemoryDownloadSource.hpp"

#include <sstream>

void InMemoryDownloadSource::setSessionFeedbacks(
    const std::vector<Feedback>& feedbacks) {
    sessionFeedbacks_ = feedbacks;
    if (!hasFilterView_) {
        downloadView_ = sessionFeedbacks_;
    }
}

void InMemoryDownloadSource::refreshAfterAnalyze(
    const std::vector<Feedback>& feedbacks) {
    sessionFeedbacks_ = feedbacks;
    downloadView_ = sessionFeedbacks_;
    hasFilterView_ = false;
}

void InMemoryDownloadSource::applyFilterResult(
    const std::vector<Feedback>& filtered, bool success) {
    if (success && !filtered.empty()) {
        downloadView_ = filtered;
        hasFilterView_ = true;
    }
}

std::string InMemoryDownloadSource::renderCsv() const {
    std::ostringstream csv;
    csv << "\xEF\xBB\xBF";
    csv << "text\n";
    for (const auto& item : downloadView_) {
        csv << item.getText() << '\n';
    }
    return csv.str();
}
