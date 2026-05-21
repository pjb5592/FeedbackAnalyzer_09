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

}  // namespace fa_fixtures
