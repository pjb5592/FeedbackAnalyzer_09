#pragma once

#include <map>
#include <string>
#include <vector>

class SentimentKeywordStore {
public:
    bool loadFromFile(const std::string& path);
    bool saveToFile(const std::string& path) const;
    void applyToConstants() const;
    const std::map<std::string, std::vector<std::string>>& keywords() const {
        return keywords_;
    }

private:
    std::map<std::string, std::vector<std::string>> keywords_;
};
