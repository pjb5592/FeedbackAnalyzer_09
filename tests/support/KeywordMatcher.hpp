#pragma once

#include <string>
#include <vector>

namespace fa_support {

inline bool containsAny(const std::string& text,
                        const std::vector<std::string>& keywords) {
    for (const auto& kw : keywords) {
        if (text.find(kw) != std::string::npos) {
            return true;
        }
    }
    return false;
}

}  // namespace fa_support
