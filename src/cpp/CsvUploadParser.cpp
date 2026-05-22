#include "CsvUploadParser.h"

#include <sstream>

namespace {

std::string stripCr(std::string line) {
    if (!line.empty() && line.back() == '\r') {
        line.pop_back();
    }
    return line;
}

}  // namespace

std::vector<std::string> CsvUploadParser::parseLine(const std::string& line) {
    std::vector<std::string> fields;
    std::string field;
    bool inQuotes = false;
    for (char c : line) {
        if (c == '"') {
            inQuotes = !inQuotes;
        } else if (c == ',' && !inQuotes) {
            fields.push_back(field);
            field.clear();
        } else {
            field += c;
        }
    }
    fields.push_back(field);
    return fields;
}

std::vector<Feedback> CsvUploadParser::parse(const std::string& content) const {
    std::vector<Feedback> result;
    if (content.empty()) {
        return result;
    }

    std::istringstream stream(content);
    std::string line;
    if (!std::getline(stream, line)) {
        return result;
    }
    line = stripCr(line);

    const auto headerFields = parseLine(line);
    int textColumn = 0;
    for (size_t i = 0; i < headerFields.size(); ++i) {
        if (headerFields[i] == "text") {
            textColumn = static_cast<int>(i);
            break;
        }
    }

    while (std::getline(stream, line)) {
        line = stripCr(line);
        if (line.empty()) {
            continue;
        }
        const auto fields = parseLine(line);
        if (static_cast<int>(fields.size()) > textColumn &&
            !fields[static_cast<size_t>(textColumn)].empty()) {
            result.emplace_back(fields[static_cast<size_t>(textColumn)]);
        }
    }

    return result;
}
