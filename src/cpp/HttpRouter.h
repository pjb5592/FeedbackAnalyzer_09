#pragma once

#include "httplib.h"
#include "Filters.h"
#include "TextAnalyzer.h"

struct AppContext {
    TextAnalyzer& textAnalyzer;
    Filters& filters;
};

class HttpRouter {
public:
    static void registerRoutes(httplib::Server& server, AppContext& ctx);
};
