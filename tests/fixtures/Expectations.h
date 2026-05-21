#pragma once

#include <map>
#include <string>

#include <catch2/catch_test_macros.hpp>

namespace fa_expect {

inline void expectSentiment(const std::map<std::string, int>& m, int pos, int neu,
                            int neg) {
    REQUIRE(m.at(u8"긍정") == pos);
    REQUIRE(m.at(u8"중립") == neu);
    REQUIRE(m.at(u8"부정") == neg);
}

inline void expectKeywordGe(const std::map<std::string, int>& m,
                            const std::string& cat, int min) {
    REQUIRE(m.at(cat) >= min);
}

}  // namespace fa_expect
