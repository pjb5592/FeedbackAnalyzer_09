#include "TrendCsvParser.h"

#include <sstream>

#include "CsvUploadParser.h"

namespace {

std::string stripCr(std::string line) {
    if (!line.empty() && line.back() == '\r') {
        line.pop_back();
    }
    return line;
}

}  // namespace

std::vector<TrendRow> TrendCsvParser::parse(const std::string& content) const {
    std::vector<TrendRow> result;
    if (content.empty()) {
        return result;
    }

    std::istringstream stream(content);
    std::string line;
    if (!std::getline(stream, line)) {
        return result;
    }
    line = stripCr(line);

    CsvUploadParser lineParser;
    const auto headerFields = lineParser.parseLine(line);
    int dateColumn = -1;
    int textColumn = 0;
    for (size_t i = 0; i < headerFields.size(); ++i) {
        if (headerFields[i] == "date") {
            dateColumn = static_cast<int>(i);
        }
        if (headerFields[i] == "text") {
            textColumn = static_cast<int>(i);
        }
    }

    while (std::getline(stream, line)) {
        line = stripCr(line);
        if (line.empty()) {
            continue;
        }
        const auto fields = lineParser.parseLine(line);
        if (static_cast<int>(fields.size()) <= textColumn) {
            continue;
        }
        TrendRow row;
        if (dateColumn >= 0 && static_cast<int>(fields.size()) > dateColumn) {
            row.date = fields[static_cast<size_t>(dateColumn)];
        }
        row.text = fields[static_cast<size_t>(textColumn)];
        if (!row.text.empty()) {
            result.push_back(std::move(row));
        }
    }

    return result;
}
