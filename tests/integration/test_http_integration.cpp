#include <catch2/catch_test_macros.hpp>

#include <string>

#include "HttpServerFixture.hpp"

namespace {

bool bodyContains(const std::string& body, const std::string& needle) {
    return body.find(needle) != std::string::npos;
}

}  // namespace

TEST_CASE("FA_TC_44_HttpGetRoot", "[fa-tc][p1][http][integration]") {
    HttpServerFixture server;
    if (!server.ready()) {
        SKIP("feedback_analyzer server not available");
    }
    auto cli = server.client();
    const auto res = cli.Get("/");
    REQUIRE(res);
    REQUIRE(res->status == 200);
    REQUIRE(res->get_header_value("Content-Type").find("text/html") !=
            std::string::npos);
    REQUIRE(bodyContains(res->body, "Feedback Analyzer"));
}

TEST_CASE("FA_TC_45_HttpAnalyze", "[fa-tc][p1][http][integration]") {
    HttpServerFixture server;
    if (!server.ready()) {
        SKIP("feedback_analyzer server not available");
    }
    auto cli = server.client();
    const auto res = cli.Post("/analyze", "text=test", "application/x-www-form-urlencoded");
    REQUIRE(res);
    REQUIRE(res->status == 200);
    REQUIRE(bodyContains(res->body, u8"분석 결과"));
    REQUIRE(bodyContains(res->body, u8"감정 분포"));
}

TEST_CASE("FA_TC_46_HttpAnalyzeEmpty", "[fa-tc][p1][http][integration]") {
    HttpServerFixture server;
    if (!server.ready()) {
        SKIP("feedback_analyzer server not available");
    }
    auto cli = server.client();
    cli.Post("/analyze", "text=keep", "application/x-www-form-urlencoded");
    const auto res = cli.Post("/analyze", "text=", "application/x-www-form-urlencoded");
    REQUIRE(res);
    REQUIRE(res->status == 200);
    REQUIRE(bodyContains(res->body, u8"1개의 피드백"));
}

TEST_CASE("FA_TC_47_HttpUpload", "[fa-tc][p1][http][integration]") {
    HttpServerFixture server;
    if (!server.ready()) {
        SKIP("feedback_analyzer server not available");
    }
    auto cli = server.client();
    const std::string csv = "text\nlineA\nlineB\n";
    httplib::UploadFormDataItems items = {
        {"file", csv, "sample.csv", "text/csv"}};
    const auto res = cli.Post("/upload", items);
    REQUIRE(res);
    REQUIRE(res->status == 200);
    REQUIRE((bodyContains(res->body, u8"2개의 피드백") || bodyContains(res->body, "2")));
}

TEST_CASE("FA_TC_48_HttpFilterEmpty", "[fa-tc][p1][http][integration]") {
    HttpServerFixture server;
    if (!server.ready()) {
        SKIP("feedback_analyzer server not available");
    }
    auto cli = server.client();
    const std::string body = std::string(u8"sentiment=") + u8"전체" + "&keyword=" + u8"전체";
    const auto res = cli.Post("/filter", body, "application/x-www-form-urlencoded");
    REQUIRE(res);
    REQUIRE(res->status == 200);
    REQUIRE(bodyContains(res->body, u8"분석할 피드백이 없습니다"));
}

TEST_CASE("FA_TC_49_HttpFilterZero", "[fa-tc][p1][http][integration]") {
    HttpServerFixture server;
    if (!server.ready()) {
        SKIP("feedback_analyzer server not available");
    }
    auto cli = server.client();
    cli.Post("/analyze", "text=good", "application/x-www-form-urlencoded");
    const std::string body = std::string(u8"sentiment=") + u8"부정" + "&keyword=" + u8"전체";
    const auto res = cli.Post("/filter", body, "application/x-www-form-urlencoded");
    REQUIRE(res);
    REQUIRE(res->status == 200);
    REQUIRE(bodyContains(res->body, u8"필터링 결과가 없습니다"));
}

TEST_CASE("FA_TC_50_HttpEscapeHtml", "[fa-tc][p2][http][integration]") {
    HttpServerFixture server;
    if (!server.ready()) {
        SKIP("feedback_analyzer server not available");
    }
    auto cli = server.client();
    const auto res = cli.Post("/analyze", "text=%3Cscript%3E",
                              "application/x-www-form-urlencoded");
    REQUIRE(res);
    REQUIRE(res->status == 200);
    REQUIRE_FALSE(bodyContains(res->body, "<script>"));
}

TEST_CASE("FA_TC_38_UploadNoStats", "[fa-tc][p0][http][integration]") {
    HttpServerFixture server;
    if (!server.ready()) {
        SKIP("feedback_analyzer server not available");
    }
    auto cli = server.client();
    const std::string csv = "text\nonlyUpload\n";
    httplib::UploadFormDataItems items = {
        {"file", csv, "sample.csv", "text/csv"}};
    const auto res = cli.Post("/upload", items);
    REQUIRE(res);
    REQUIRE(res->status == 200);
    REQUIRE_FALSE(bodyContains(res->body, u8"감정 분포"));
    REQUIRE_FALSE(bodyContains(res->body, u8"키워드 분포"));
}

TEST_CASE("FA_TC_52_HttpMultiline", "[fa-tc][p2][http][integration]") {
    HttpServerFixture server;
    if (!server.ready()) {
        SKIP("feedback_analyzer server not available");
    }
    auto cli = server.client();
    const auto res = cli.Post("/analyze", "text=line1%0Aline2",
                              "application/x-www-form-urlencoded");
    REQUIRE(res);
    REQUIRE(res->status == 200);
    REQUIRE(bodyContains(res->body, u8"1개의 피드백"));
}
