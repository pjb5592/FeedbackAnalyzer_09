#pragma once
#include <string>
#include <vector>
#include <map>
#include "Feedback.h"
#include "Constants.h"
#include "KeywordMatcher.h"

class TextAnalyzer {
public:
    std::map<std::string, int> analyzeSentiment(const std::vector<Feedback>& feedbacks) {
        std::map<std::string, int> res;
        res[u8"긍정"] = 0;
        res[u8"중립"] = 0;
        res[u8"부정"] = 0;

        for (const auto& f : feedbacks) {
            std::string txt = f.getText();
            std::string s = u8"중립";
            if (fa::containsAny(txt, Constants::SENTIMENT_KEYWORDS[u8"긍정"])) {
                s = u8"긍정";
            } else if (fa::containsAny(txt, Constants::SENTIMENT_KEYWORDS[u8"부정"])) {
                s = u8"부정";
            }
            res[s]++;
        }

        return res;
    }

    std::map<std::string, int> countKeywords(const std::vector<Feedback>& feedbacks) {
        std::map<std::string, int> res2;
        for (const auto& entry : Constants::CATEGORY_KEYWORDS) {
            res2[entry.first] = 0;
        }

        for (const auto& f : feedbacks) {
            std::string txt = f.getText();
            for (const auto& entry : Constants::CATEGORY_KEYWORDS) {
                const std::string& cat = entry.first;
                if (entry.second.count("main")) {
                    const auto& kws = entry.second.at("main");
                    if (fa::containsAny(txt, kws)) {
                        res2[cat]++;
                    }
                }
            }
        }

        return res2;
    }
};
