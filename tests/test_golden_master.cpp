#include <catch2/catch_test_macros.hpp>

#include "support/GoldenMasterIO.hpp"
#include "support/GoldenMasterRunner.hpp"

namespace {

void requireGolden(const std::string& file, const std::string& actual) {
    const std::string expected =
        fa_golden::normalizeNewlines(fa_golden::loadFile(file));
    const std::string normalized = fa_golden::normalizeNewlines(actual);
    REQUIRE(normalized == expected);
}

}  // namespace

TEST_CASE("FA_TC_53_GoldenMaster01", "[fa-tc][p2][gm]") {
    requireGolden("gm01_sent_kw_stats.txt", fa_golden::runGm01());
}

TEST_CASE("FA_TC_53_GoldenMaster02", "[fa-tc][p2][gm]") {
    requireGolden("gm02_def01_neutral.txt", fa_golden::runGm02());
}

TEST_CASE("FA_TC_53_GoldenMaster03", "[fa-tc][p2][gm]") {
    requireGolden("gm03_def02_main_only.txt", fa_golden::runGm03());
}

TEST_CASE("FA_TC_53_GoldenMaster04", "[fa-tc][p2][gm]") {
    requireGolden("gm04_csv_text_header.txt", fa_golden::runGm04());
}

TEST_CASE("FA_TC_53_GoldenMaster05", "[fa-tc][p2][gm]") {
    requireGolden("gm05_download_filtered.csv", fa_golden::runGm05());
}

TEST_CASE("FA_TC_53_GoldenMaster06", "[fa-tc][p2][gm]") {
    requireGolden("gm06_five_categories.txt", fa_golden::runGm06());
}

TEST_CASE("FA_TC_53_GoldenMaster07", "[fa-tc][p2][gm]") {
    requireGolden("gm07_empty_stats.txt", fa_golden::runGm07());
}

TEST_CASE("FA_TC_53_GoldenMaster08", "[fa-tc][p2][gm]") {
    requireGolden("gm08_ac_sent_consistency.txt", fa_golden::runGm08());
}

TEST_CASE("FA_TC_53_GoldenMaster09", "[fa-tc][p2][gm]") {
    requireGolden("gm09_ac_kw_main_only.txt", fa_golden::runGm09());
}
