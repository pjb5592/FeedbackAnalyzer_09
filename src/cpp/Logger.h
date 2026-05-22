#pragma once

#include <string>

class Logger {
private:
    static bool debugMode;

public:
    static void logInfo(const std::string& message);
    static void logWarning(const std::string& message);
    static void logError(const std::string& message);
    static void logDebug(const std::string& message);

    static void setDebugMode(bool mode);
    static bool isDebugMode();
};
