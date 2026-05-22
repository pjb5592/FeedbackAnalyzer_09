#include "httplib.h"

#include "Constants.h"
#include "FileHandler.h"
#include "Filters.h"
#include "HttpRouter.h"
#include "Logger.h"
#include "TextAnalyzer.h"

int main() {
    Constants::init();

    httplib::Server svr;
    TextAnalyzer textAnalyzer;
    Filters filters;
    FileHandler fileHandler;
    AppContext ctx{textAnalyzer, filters};

    HttpRouter::registerRoutes(svr, ctx);

    Logger::logInfo(u8"서버가 http://localhost:8080 에서 시작됩니다.");
    svr.listen("0.0.0.0", 8080);

    return 0;
}
