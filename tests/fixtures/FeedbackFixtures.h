#pragma once

#include <string>
#include <vector>

#include "Feedback.h"

namespace fa_fixtures {

inline std::vector<Feedback> emptyFeedbacks() { return {}; }

inline std::vector<Feedback> singleText(const std::string& text) {
    return {Feedback(text)};
}

inline std::vector<Feedback> def01NeutralAmbiguous() {
    return singleText(u8"보통 배송은 괜찮아요");
}

inline std::vector<Feedback> mainOnlyShipping() { return singleText(u8"배송"); }

inline std::vector<Feedback> mainOnlyQuality() { return singleText(u8"품질"); }

inline std::vector<Feedback> positiveNegativeOverlap() {
    return singleText(u8"최고입니다 정말 불만 실망 최악");
}

inline std::vector<Feedback> mixedSentimentSet() {
    return {Feedback(u8"정말 좋아요 최고입니다"),
            Feedback(u8"불만 실망 최악"),
            Feedback(u8"배송이 늦었습니다")};
}

inline std::vector<Feedback> fiveCategoriesOneEach() {
    return {Feedback(u8"배송이 빨라요"), Feedback(u8"품질이 우수합니다"),
            Feedback(u8"가격이 비싸요"), Feedback(u8"서비스가 친절해요"),
            Feedback(u8"사용이 편리해요")};
}

}  // namespace fa_fixtures
