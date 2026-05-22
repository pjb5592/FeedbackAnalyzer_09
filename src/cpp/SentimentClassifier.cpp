#include "SentimentClassifier.h"

#include "Constants.h"
#include "KeywordMatcher.h"

namespace fa {

std::string classifySentiment(const std::string& text) {
    if (containsAny(text, Constants::SENTIMENT_KEYWORDS[u8"긍정"])) {
        return u8"긍정";
    }
    if (containsAny(text, Constants::SENTIMENT_KEYWORDS[u8"부정"])) {
        return u8"부정";
    }
    return u8"중립";
}

}  // namespace fa
