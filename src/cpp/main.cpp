#include "httplib.h"

#include "Constants.h"
#include "Filters.h"
#include "HttpRouter.h"
#include "Logger.h"
#include "TextAnalyzer.h"

int main() {
    Constants::init();

    httplib::Server server;
    TextAnalyzer textAnalyzer;
    Filters filters;
    AppContext context{textAnalyzer, filters};

    HttpRouter::registerRoutes(server, context);

    Logger::logInfo(u8"서버가 http://localhost:8080 에서 시작됩니다.");
    server.listen("0.0.0.0", 8080);

    return 0;
}
