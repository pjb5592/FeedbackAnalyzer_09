#include "FileHandler.h"

#include <fstream>
#include <sstream>

std::string FileHandler::renderCsv(const std::vector<Feedback>& data) {
    std::ostringstream csv;
    csv << "\xEF\xBB\xBF";
    csv << "text\n";
    for (const auto& item : data) {
        csv << item.getText() << '\n';
    }
    return csv.str();
}

bool FileHandler::saveResult(const std::vector<Feedback>& data,
                             const std::string& path) {
    std::ofstream out(path, std::ios::binary);
    if (!out) {
        return false;
    }
    const std::string csv = renderCsv(data);
    out.write(csv.data(), static_cast<std::streamsize>(csv.size()));
    return out.good();
}
