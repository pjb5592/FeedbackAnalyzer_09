#include <catch2/catch_test_macros.hpp>

#include "fixtures/Expectations.h"
#include "fixtures/FeedbackFixtures.h"
#include "support/DomainKeywordCounter.hpp"

TEST_CASE("FA_TC_09_KeywordShipping", "[fa-tc][p0][kw][domain]") {
    DomainKeywordCounter counter;
    const auto result = counter.count(
        fa_fixtures::singleText(u8"\uBC30\uC1A1\uC774 \uBE60\uB77C\uC694"));
    fa_expect::expectKeywordGe(result, u8"\uBC30\uC1A1", 1);
}

TEST_CASE("FA_TC_10_KeywordQuality", "[fa-tc][p0][kw][domain]") {
    DomainKeywordCounter counter;
    const auto result = counter.count(
        fa_fixtures::singleText(u8"\uD488\uC9C8\uC774 \uC6B0\uC218\uD569\uB2C8\uB2E4"));
    fa_expect::expectKeywordGe(result, u8"\uD488\uC9C8", 1);
}

TEST_CASE("FA_TC_11_KeywordPrice", "[fa-tc][p0][kw][domain]") {
    DomainKeywordCounter counter;
    const auto result = counter.count(
        fa_fixtures::singleText(u8"\uAC00\uACA9\uC774 \uBE44\uC2F8\uC694"));
    fa_expect::expectKeywordGe(result, u8"\uAC00\uACA9", 1);
}

TEST_CASE("FA_TC_12_KeywordService", "[fa-tc][p0][kw][domain]") {
    DomainKeywordCounter counter;
    const auto result = counter.count(
        fa_fixtures::singleText(u8"\uC11C\uBE44\uC2A4\uAC00 \uCE5C\uC808\uD574\uC694"));
    fa_expect::expectKeywordGe(result, u8"\uC11C\uBE44\uC2A4", 1);
}

TEST_CASE("FA_TC_13_KeywordUsability", "[fa-tc][p0][kw][domain]") {
    DomainKeywordCounter counter;
    const auto result = counter.count(
        fa_fixtures::singleText(u8"\uC0AC\uC6A9\uC774 \uD3B8\uB9AC\uD574\uC694"));
    fa_expect::expectKeywordGe(result, u8"\uC0AC\uC6A9\uC131", 1);
}

TEST_CASE("FA_TC_14_KeywordNone", "[fa-tc][p0][kw][domain]") {
    DomainKeywordCounter counter;
    const auto result = counter.count(
        fa_fixtures::singleText(u8"\uC624\uB298 \uB0A0\uC528\uAC00 \uB9D1\uC2B5\uB2C8\uB2E4"));
    REQUIRE(result.at(u8"\uBC30\uC1A1") == 0);
    REQUIRE(result.at(u8"\uD488\uC9C8") == 0);
    REQUIRE(result.at(u8"\uAC00\uACA9") == 0);
    REQUIRE(result.at(u8"\uC11C\uBE44\uC2A4") == 0);
    REQUIRE(result.at(u8"\uC0AC\uC6A9\uC131") == 0);
}

TEST_CASE("FA_TC_15_KeywordMultiCategory", "[fa-tc][p0][kw][domain]") {
    DomainKeywordCounter counter;
    const auto result = counter.count(
        fa_fixtures::singleText(u8"\uBC30\uC1A1\uB3C4 \uBE60\uB974\uACE0 \uD488\uC9C8\uB3C4 \uC88B\uC2B5\uB2C8\uB2E4"));
    REQUIRE(result.at(u8"\uBC30\uC1A1") >= 1);
    REQUIRE(result.at(u8"\uD488\uC9C8") >= 1);
}
