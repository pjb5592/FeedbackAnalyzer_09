#pragma once

#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>

namespace fa_golden {

inline std::filesystem::path goldenRoot() {
    const std::filesystem::path direct = std::filesystem::path("tests") / "golden";
    if (std::filesystem::exists(direct)) {
        return direct;
    }
    return std::filesystem::path("..") / "tests" / "golden";
}

inline std::string loadFile(const std::string& name) {
    const auto path = goldenRoot() / name;
    std::ifstream in(path, std::ios::binary);
    std::ostringstream buffer;
    buffer << in.rdbuf();
    std::string content = buffer.str();
    while (!content.empty() && (content.back() == '\n' || content.back() == '\r')) {
        content.pop_back();
    }
    return content + '\n';
}

inline std::string normalizeNewlines(std::string text) {
    std::string out;
    out.reserve(text.size());
    for (size_t i = 0; i < text.size(); ++i) {
        if (text[i] == '\r' && i + 1 < text.size() && text[i + 1] == '\n') {
            out += '\n';
            ++i;
        } else if (text[i] == '\r') {
            out += '\n';
        } else {
            out += text[i];
        }
    }
    return out;
}

}  // namespace fa_golden
