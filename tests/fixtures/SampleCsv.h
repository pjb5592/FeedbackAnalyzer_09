#pragma once

#include <string>

namespace fa_csv {

inline const std::string kCsvTextHeader = u8"text\n행1\n행2\n";
inline const std::string kCsvIdText = u8"id,text\n1,안녕\n";
inline const std::string kCsvEmpty = "";
inline const std::string kCsvQuoted = "text\n\"a,b\"\n";
inline const std::string kCsvCrlf = "text\r\nline1\r\nline2\r\n";

}  // namespace fa_csv
