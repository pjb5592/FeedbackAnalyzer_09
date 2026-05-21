#include <catch2/catch_session.hpp>

#include "test_env.hpp"

int main(int argc, char* argv[]) {
    static FeedbackAnalyzerTestEnv env;
    return Catch::Session().run(argc, argv);
}
