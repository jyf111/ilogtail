#ifndef TOMCAT_PARSER_TEST_REGEX_PARSER_H
#define TOMCAT_PARSER_TEST_REGEX_PARSER_H

#include <memory>
#include <regex>
#include <string>
#include <vector>

#include "log.h"
#include "pattern.h"
#include "pattern_factory.h"

namespace tomcat_parser {
class RegexParser {
public:
    RegexParser() = default;

    RegexParser(const RegexParser&) = delete;
    const RegexParser& operator=(const RegexParser&) = delete;

    void Process(const std::string& format, const std::vector<std::string>& input, std::vector<Log>& output);

private:
    bool GenPattern(const std::string& format);

    Log ProcessSingleLine(const std::string& line) const;

    PatternFactory m_pattern_factory;
    std::vector<std::unique_ptr<RegexPattern>> m_regex_patterns;
    std::regex m_regex;
};
} // namespace tomcat_parser
#endif // TOMCAT_PARSER_TEST_REGEX_PARSER_H