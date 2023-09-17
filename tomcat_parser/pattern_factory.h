#ifndef TOMCAT_PARSER_FORMAT_TABLE_H
#define TOMCAT_PARSER_FORMAT_TABLE_H

#include <memory>
#include <unordered_map>

#include "pattern.h"

namespace tomcat_parser {
class PatternFactory {
public:
    PatternFactory();

    PatternFactory(const PatternFactory&) = delete;
    const PatternFactory& operator=(const PatternFactory&) = delete;

    std::unique_ptr<LiteralPattern> GenLiteralPattern(const std::string& expression) const;
    std::unique_ptr<RegexPattern> GenRegexPattern(const std::string& expression) const;

private:
    struct FormatSpec {
        std::string expr;
        std::string charset;
        std::string desc;

        char left_delimiter;
        char right_delimiter;
        size_t len;
        bool full_capture;
    };

    std::unordered_map<char, FormatSpec> m_format_table;
    std::unordered_map<char, FormatSpec> m_keyformat_table;
};
} // namespace tomcat_parser
#endif // TOMCAT_PARSER_FORMAT_TABLE_H