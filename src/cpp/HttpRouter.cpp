#include "HttpRouter.h"

#include <map>
#include <sstream>
#include <string>

#include "CsvUploadParser.h"
#include "Feedback.h"
#include "HtmlRenderer.h"
#include "Logger.h"
#include "Session.h"

namespace {

std::string urlDecode(const std::string& str) {
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

std::map<std::string, std::string> parseForm(const std::string& body) {
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

void setHtmlResponse(httplib::Response& res, const std::string& html) {
    res.set_content(html, "text/html; charset=UTF-8");
}

}  // namespace

void HttpRouter::registerRoutes(httplib::Server& server, AppContext& ctx) {
    server.Get("/", [](const httplib::Request&, httplib::Response& res) {
        const auto& feedbacks = Session::getFeedbacks();
        setHtmlResponse(res, HtmlRenderer::renderPage(u8"피드백 분석기 시작", "", "",
                                                      {}, {}, feedbacks));
    });

    server.Post("/analyze", [&ctx](const httplib::Request& req, httplib::Response& res) {
        try {
            auto params = parseForm(req.body);
            std::string text = params["text"];

            if (!text.empty()) {
                auto start = text.find_first_not_of(" \t\r\n");
                auto end = text.find_last_not_of(" \t\r\n");
                if (start != std::string::npos) {
                    text = text.substr(start, end - start + 1);
                    Session::appendFeedback(Feedback(text));
                }
            }

            const auto& feedbacks = Session::getFeedbacks();
            for (const auto& fb : feedbacks) {
                Logger::logInfo(fb.getText());
            }

            Logger::logInfo(u8"현재 " + std::to_string(feedbacks.size()) +
                            u8"개의 피드백이 입력되었습니다.");

            std::string success =
                std::to_string(feedbacks.size()) + u8"개의 피드백이 입력되었습니다.";
            std::map<std::string, int> sentimentResults, keywordResults;

            if (!feedbacks.empty()) {
                sentimentResults = ctx.textAnalyzer.analyzeSentiment(feedbacks);
                keywordResults = ctx.textAnalyzer.countKeywords(feedbacks);
                Logger::logInfo(u8"감성 분석 완료");
                Logger::logInfo(u8"키워드 분석 완료");
            }

            Session::refreshAfterAnalyze(feedbacks);

            setHtmlResponse(res, HtmlRenderer::renderPage(success, "", "",
                                                          sentimentResults, keywordResults,
                                                          feedbacks));
        } catch (const std::exception& e) {
            Logger::logError(std::string(u8"오류 발생: ") + e.what());
            setHtmlResponse(res, HtmlRenderer::renderPage("", "",
                                                          u8"처리 중 오류가 발생했습니다.", {},
                                                          {}, {}));
        }
    });

    server.Post("/upload", [](const httplib::Request& req, httplib::Response& res) {
        try {
            if (req.form.has_file("file")) {
                const auto file = req.form.get_file("file");
                if (!file.content.empty()) {
                    CsvUploadParser parser;
                    Session::appendFeedbacks(parser.parse(file.content));
                    Logger::logInfo(u8"파일이 성공적으로 업로드되었습니다.");
                }
            }
            const auto& feedbacks = Session::getFeedbacks();
            Session::setSessionFeedbacks(feedbacks);
            std::string success =
                std::to_string(feedbacks.size()) + u8"개의 피드백이 입력되었습니다.";
            setHtmlResponse(res,
                            HtmlRenderer::renderPage(success, "", "", {}, {}, feedbacks));
        } catch (const std::exception& e) {
            Logger::logError(std::string(u8"파일 업로드 오류: ") + e.what());
            setHtmlResponse(res, HtmlRenderer::renderPage("", "",
                                                          u8"파일 업로드 중 오류가 발생했습니다.",
                                                          {}, {}, {}));
        }
    });

    server.Post("/filter", [&ctx](const httplib::Request& req, httplib::Response& res) {
        try {
            const auto& feedbacks = Session::getFeedbacks();
            auto params = parseForm(req.body);
            std::string sentiment = params["sentiment"];
            std::string keyword = params["keyword"];

            if (!feedbacks.empty()) {
                auto filtered =
                    ctx.filters.filterFeedbacks(feedbacks, sentiment, keyword);
                Session::applyFilterResult(filtered, !filtered.empty());
                if (!filtered.empty()) {
                    auto sentimentResults = ctx.textAnalyzer.analyzeSentiment(filtered);
                    auto keywordResults = ctx.textAnalyzer.countKeywords(filtered);
                    Logger::logInfo(u8"필터링 결과: " +
                                    std::to_string(filtered.size()) + u8"개의 피드백");
                    setHtmlResponse(res, HtmlRenderer::renderPage("", "", "",
                                                                  sentimentResults,
                                                                  keywordResults, filtered));
                } else {
                    Logger::logWarning(u8"필터링 결과가 없습니다.");
                    setHtmlResponse(res, HtmlRenderer::renderPage(
                                            "", u8"필터링 결과가 없습니다.", "", {}, {}, {}));
                }
            } else {
                Logger::logWarning(u8"분석할 피드백이 없습니다.");
                setHtmlResponse(res, HtmlRenderer::renderPage(
                                        "", u8"분석할 피드백이 없습니다.", "", {}, {}, {}));
            }
        } catch (const std::exception& e) {
            Logger::logError(std::string(u8"오류 발생: ") + e.what());
            setHtmlResponse(res, HtmlRenderer::renderPage("", "",
                                                          u8"처리 중 오류가 발생했습니다.", {},
                                                          {}, {}));
        }
    });

    server.Get("/download", [](const httplib::Request&, httplib::Response& res) {
        const std::string csv = Session::renderDownloadCsv();
        res.set_header("Content-Disposition",
                       "attachment; filename=\"filtered_feedback.csv\"");
        res.set_content(csv, "text/csv; charset=UTF-8");
    });
}
