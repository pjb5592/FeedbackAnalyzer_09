#include <catch2/catch_test_macros.hpp>

#include "support/GoldenMasterIO.hpp"
#include "support/GoldenMasterRunner.hpp"

namespace {

void requireGolden(const std::string& approvedFile, const std::string& actual) {
    const std::string expected =
        fa_golden::normalizeNewlines(fa_golden::loadFile(approvedFile));
    const std::string normalized = fa_golden::normalizeNewlines(actual);
    REQUIRE(normalized == expected);
}

}  // namespace

TEST_CASE("GM-01 analyze aggregate", "[fa-tc][p2][gm][gm-01]") {
    requireGolden("gm01_analyze_aggregate.approved.txt", fa_golden::runGm01());
}

TEST_CASE("GM-02 filter neutral", "[fa-tc][p2][gm][gm-02]") {
    requireGolden("gm02_filter_neutral.approved.txt", fa_golden::runGm02());
}

TEST_CASE("GM-03 download CSV", "[fa-tc][p2][gm][gm-03]") {
    requireGolden("gm03_download_csv.approved.txt", fa_golden::runGm03());
}

TEST_CASE("GM-04 upload CSV", "[fa-tc][p2][gm][gm-04]") {
    requireGolden("gm04_upload_csv.approved.txt", fa_golden::runGm04());
}
