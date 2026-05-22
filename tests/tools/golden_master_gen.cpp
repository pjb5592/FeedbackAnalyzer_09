#include <iostream>

#include "Constants.h"
#include "support/GoldenMasterIO.hpp"
#include "support/GoldenMasterRunner.hpp"

int main() {
    Constants::init();
    struct Scenario {
        const char* file;
        std::string (*run)();
    };
    const Scenario scenarios[] = {
        {"gm01_analyze_aggregate.approved.txt", fa_golden::runGm01},
        {"gm02_filter_neutral.approved.txt", fa_golden::runGm02},
        {"gm03_download_csv.approved.txt", fa_golden::runGm03},
        {"gm04_upload_csv.approved.txt", fa_golden::runGm04},
    };

    for (const auto& scenario : scenarios) {
        fa_golden::writeFile(scenario.file, scenario.run());
        std::cout << "wrote tests/golden/" << scenario.file << '\n';
    }
    return 0;
}
