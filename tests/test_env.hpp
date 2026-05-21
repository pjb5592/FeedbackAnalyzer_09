#pragma once

#include "Constants.h"
#include "Filters.h"

struct FeedbackAnalyzerTestEnv {
    FeedbackAnalyzerTestEnv() {
        Constants::init();
        Filters::initFilterKeywords();
    }
};
