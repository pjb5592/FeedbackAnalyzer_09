#include "httplib.h"
#include "Feedback.h"
#include "Constants.h"
#include "Session.h"
#include "TextAnalyzer.h"
#include "Filters.h"
#include "FileHandler.h"
#include "Logger.h"
#include "CsvUploadParser.h"
#include "HtmlRenderer.h"
#include <sstream>
#include <fstream>
#include <algorithm>

static TextAnalyzer textAnalyzer;
static Filters filters;
static FileHandler fileHandler;

// URL decode utility
static std::string urlDecode(const std::string& str) {
    std::string result;
    for (size_t i = 0; i < str.size(); i++) {
        if (str[i] == '%' && i + 2 < str.size()) {
            int val;
            std::istringstream iss(str.substr(i + 1, 2));
            if (iss >> std::hex >> val) {
                result += static_cast<char>(val);
                i += 2;
            } else {
                result += str[i];
            }
        } else if (str[i] == '+') {
            result += ' ';
        } else {
            result += str[i];
        }
    }
    return result;
}

// Parse form body
static std::map<std::string, std::string> parseForm(const std::string& body) {
    std::map<std::string, std::string> params;
    std::istringstream stream(body);
    std::string pair;
    while (std::getline(stream, pair, '&')) {
        auto eq = pair.find('=');
        if (eq != std::string::npos) {
            params[urlDecode(pair.substr(0, eq))] = urlDecode(pair.substr(eq + 1));
        }
    }
    return params;
}

int main() {
    Constants::init();

    httplib::Server svr;

    // GET /
    svr.Get("/", [](const httplib::Request&, httplib::Response& res) {
        auto& feedbacks = Session::getCurrentFeedbacks();
        std::string html = HtmlRenderer::renderPage(u8"피드백 분석기 시작", "", "", {}, {}, feedbacks);
        res.set_content(html, "text/html; charset=UTF-8");
    });

    // POST /analyze
    svr.Post("/analyze", [](const httplib::Request& req, httplib::Response& res) {
        try {
            auto& feedbacks = Session::getCurrentFeedbacks();
            auto params = parseForm(req.body);
            std::string text = params["text"];

            if (!text.empty()) {
                // trim
                auto start = text.find_first_not_of(" \t\r\n");
                auto end = text.find_last_not_of(" \t\r\n");
                if (start != std::string::npos) {
                    text = text.substr(start, end - start + 1);
                    feedbacks.push_back(Feedback(text));
                }
            }

            for (const auto& fb : feedbacks) {
                Logger::logInfo(fb.getText());
            }

            Logger::logInfo(u8"현재 " + std::to_string(feedbacks.size()) + u8"개의 피드백이 입력되었습니다.");

            std::string success = std::to_string(feedbacks.size()) + u8"개의 피드백이 입력되었습니다.";
            std::map<std::string, int> sentimentResults, keywordResults;

            if (!feedbacks.empty()) {
                sentimentResults = textAnalyzer.analyzeSentiment(feedbacks);
                keywordResults = textAnalyzer.countKeywords(feedbacks);
                Logger::logInfo(u8"감성 분석 완료");
                Logger::logInfo(u8"키워드 분석 완료");
            }

            Session::refreshAfterAnalyze(feedbacks);

            std::string html = HtmlRenderer::renderPage(success, "", "", sentimentResults, keywordResults, feedbacks);
            res.set_content(html, "text/html; charset=UTF-8");
        } catch (const std::exception& e) {
            Logger::logError(std::string(u8"오류 발생: ") + e.what());
            std::string html = HtmlRenderer::renderPage("", "", u8"처리 중 오류가 발생했습니다.", {}, {}, {});
            res.set_content(html, "text/html; charset=UTF-8");
        }
    });

    // POST /upload
    svr.Post("/upload", [](const httplib::Request& req, httplib::Response& res) {
        try {
            auto& feedbacks = Session::getCurrentFeedbacks();
            if (req.form.has_file("file")) {
                const auto file = req.form.get_file("file");
                if (!file.content.empty()) {
                    CsvUploadParser parser;
                    const auto parsed = parser.parse(file.content);
                    for (const auto& item : parsed) {
                        feedbacks.push_back(item);
                    }
                    Logger::logInfo(u8"파일이 성공적으로 업로드되었습니다.");
                }
            }
            Session::setSessionFeedbacks(feedbacks);
            std::string success = std::to_string(feedbacks.size()) + u8"개의 피드백이 입력되었습니다.";
            std::string html = HtmlRenderer::renderPage(success, "", "", {}, {}, feedbacks);
            res.set_content(html, "text/html; charset=UTF-8");
        } catch (const std::exception& e) {
            Logger::logError(std::string(u8"파일 업로드 오류: ") + e.what());
            std::string html = HtmlRenderer::renderPage("", "", u8"파일 업로드 중 오류가 발생했습니다.", {}, {}, {});
            res.set_content(html, "text/html; charset=UTF-8");
        }
    });

    // POST /filter
    svr.Post("/filter", [](const httplib::Request& req, httplib::Response& res) {
        try {
            auto& feedbacks = Session::getCurrentFeedbacks();
            auto params = parseForm(req.body);
            std::string sentiment = params["sentiment"];
            std::string keyword = params["keyword"];

            if (!feedbacks.empty()) {
                auto filtered = filters.filterFeedbacks(feedbacks, sentiment, keyword);
                Session::applyFilterResult(filtered, !filtered.empty());
                if (!filtered.empty()) {
                    auto sentimentResults = textAnalyzer.analyzeSentiment(filtered);
                    auto keywordResults = textAnalyzer.countKeywords(filtered);
                    Logger::logInfo(u8"필터링 결과: " + std::to_string(filtered.size()) + u8"개의 피드백");
                    std::string html = HtmlRenderer::renderPage("", "", "", sentimentResults, keywordResults, filtered);
                    res.set_content(html, "text/html; charset=UTF-8");
                } else {
                    Logger::logWarning(u8"필터링 결과가 없습니다.");
                    std::string html = HtmlRenderer::renderPage("", u8"필터링 결과가 없습니다.", "", {}, {}, {});
                    res.set_content(html, "text/html; charset=UTF-8");
                }
            } else {
                Logger::logWarning(u8"분석할 피드백이 없습니다.");
                std::string html = HtmlRenderer::renderPage("", u8"분석할 피드백이 없습니다.", "", {}, {}, {});
                res.set_content(html, "text/html; charset=UTF-8");
            }
        } catch (const std::exception& e) {
            Logger::logError(std::string(u8"오류 발생: ") + e.what());
            std::string html = HtmlRenderer::renderPage("", "", u8"처리 중 오류가 발생했습니다.", {}, {}, {});
            res.set_content(html, "text/html; charset=UTF-8");
        }
    });

    // GET /download
    svr.Get("/download", [](const httplib::Request&, httplib::Response& res) {
        const std::string csv = Session::renderDownloadCsv();
        res.set_header("Content-Disposition", "attachment; filename=\"filtered_feedback.csv\"");
        res.set_content(csv, "text/csv; charset=UTF-8");
    });

    Logger::logInfo(u8"서버가 http://localhost:8080 에서 시작됩니다.");
    svr.listen("0.0.0.0", 8080);

    return 0;
}
