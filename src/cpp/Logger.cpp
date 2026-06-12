#include "Logger.h"

#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>

namespace {

std::string getTimestamp() {
    auto now = std::time(nullptr);
    auto tm = *std::localtime(&now);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

}  // namespace

bool Logger::debugMode = true;

void Logger::logInfo(const std::string& message) {
    std::cout << "[" << getTimestamp() << "] INFO: " << message << std::endl;
}

void Logger::logWarning(const std::string& message) {
    std::cout << "[" << getTimestamp() << "] WARNING: " << message << std::endl;
}

void Logger::logError(const std::string& message) {
    std::cerr << "[" << getTimestamp() << "] ERROR: " << message << std::endl;
}

void Logger::logDebug(const std::string& message) {
    if (debugMode) {
        std::cout << "[" << getTimestamp() << "] DEBUG: " << message << std::endl;
    }
}

void Logger::setDebugMode(bool mode) {
    debugMode = mode;
}

bool Logger::isDebugMode() {
    return debugMode;
}
