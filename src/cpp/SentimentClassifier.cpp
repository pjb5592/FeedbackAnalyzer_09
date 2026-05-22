#include "SentimentClassifier.h"

#include "Constants.h"

namespace {

struct PolarityScore {
    double positive = 0.0;
    double negative = 0.0;
};

bool hasNegationAfter(const std::string& text, size_t keywordEnd) {
    if (keywordEnd >= text.size()) {
        return false;
    }
    const size_t window = std::min(text.size() - keywordEnd, size_t{8});
    const std::string tail = text.substr(keywordEnd, window);
    if (tail.find(u8"않") != std::string::npos) {
        return true;
    }
    if (tail.find(u8"지 않") != std::string::npos) {
        return true;
    }
    if (tail.find(u8" 않") != std::string::npos) {
        return true;
    }
    return false;
}

double scoreKeywordMatches(const std::string& text,
                           const std::vector<std::string>& keywords) {
    double total = 0.0;
    for (const auto& kw : keywords) {
        if (kw.empty()) {
            continue;
        }
        size_t pos = 0;
        while ((pos = text.find(kw, pos)) != std::string::npos) {
            if (!hasNegationAfter(text, pos + kw.size())) {
                total += 1.0;
            }
            pos += kw.size();
        }
    }
    return total;
}

PolarityScore computeWeightedPolarity(const std::string& text) {
    PolarityScore score;
    score.positive =
        scoreKeywordMatches(text, Constants::SENTIMENT_KEYWORDS[u8"긍정"]);
    score.negative =
        scoreKeywordMatches(text, Constants::SENTIMENT_KEYWORDS[u8"부정"]);
    return score;
}

}  // namespace

namespace fa {

std::string classifySentiment(const std::string& text) {
    const PolarityScore score = computeWeightedPolarity(text);
    if (score.positive > score.negative && score.positive > 0.0) {
        return u8"긍정";
    }
    if (score.negative > score.positive && score.negative > 0.0) {
        return u8"부정";
    }
    return u8"중립";
}

}  // namespace fa
