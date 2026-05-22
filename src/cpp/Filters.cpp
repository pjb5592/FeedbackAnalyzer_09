#include "Filters.h"

#include "Constants.h"
#include "KeywordMatcher.h"
#include "SentimentClassifier.h"

namespace {

std::vector<Feedback> filterBySentiment(const std::vector<Feedback>& dataList,
                                      const std::string& sentimentFilter) {
    if (sentimentFilter == u8"전체") {
        return dataList;
    }

    std::vector<Feedback> result;
    for (const auto& item : dataList) {
        if (fa::classifySentiment(item.getText()) == sentimentFilter) {
            result.push_back(item);
        }
    }
    return result;
}

std::vector<Feedback> filterByKeyword(const std::vector<Feedback>& dataList,
                                      const std::string& keywordFilter) {
    if (keywordFilter == u8"전체") {
        return dataList;
    }

    std::vector<Feedback> result;
    if (!Constants::CATEGORY_KEYWORDS.count(keywordFilter)) {
        return result;
    }

    const auto& categoryMap = Constants::CATEGORY_KEYWORDS.at(keywordFilter);
    for (const auto& item : dataList) {
        const std::string& text = item.getText();
        for (const auto& subEntry : categoryMap) {
            if (fa::containsAny(text, subEntry.second)) {
                result.push_back(item);
                break;
            }
        }
    }
    return result;
}

}  // namespace

std::vector<Feedback> Filters::filterFeedbacks(const std::vector<Feedback>& dataList,
                                               const std::string& sentimentFilter,
                                               const std::string& keywordFilter) {
    const auto afterSentiment = filterBySentiment(dataList, sentimentFilter);
    return filterByKeyword(afterSentiment, keywordFilter);
}
