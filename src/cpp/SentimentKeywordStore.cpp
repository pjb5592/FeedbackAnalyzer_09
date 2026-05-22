#include "SentimentKeywordStore.h"

#include <fstream>
#include <sstream>

#include "Constants.h"

namespace {

std::vector<std::string> splitCsvTokens(const std::string& line) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream stream(line);
    while (std::getline(stream, token, ',')) {
        if (!token.empty()) {
            tokens.push_back(token);
        }
    }
    return tokens;
}

}  // namespace

bool SentimentKeywordStore::loadFromFile(const std::string& path) {
    std::ifstream in(path);
    if (!in) {
        return false;
    }
    keywords_.clear();
    std::string line;
    while (std::getline(in, line)) {
        if (line.empty() || line[0] == '#') {
            continue;
        }
        const size_t colon = line.find(':');
        if (colon == std::string::npos) {
            continue;
        }
        const std::string label = line.substr(0, colon);
        keywords_[label] = splitCsvTokens(line.substr(colon + 1));
    }
    return !keywords_.empty();
}

bool SentimentKeywordStore::saveToFile(const std::string& path) const {
    std::ofstream out(path);
    if (!out) {
        return false;
    }
    for (const auto& entry : keywords_) {
        out << entry.first << ':';
        for (size_t i = 0; i < entry.second.size(); ++i) {
            if (i > 0) {
                out << ',';
            }
            out << entry.second[i];
        }
        out << '\n';
    }
    return out.good();
}

void SentimentKeywordStore::applyToConstants() const {
    for (const auto& entry : keywords_) {
        Constants::SENTIMENT_KEYWORDS[entry.first] = entry.second;
    }
}
