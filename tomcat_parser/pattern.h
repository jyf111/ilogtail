#ifndef TOMCAT_PARSER_PATTERN_H
#define TOMCAT_PARSER_PATTERN_H

#include <string>

#include "character_set.h"

namespace tomcat_parser {
class Pattern {
public:
    Pattern(const std::string& expression,
            char left_delimiter,
            char right_delimiter,
            const CharacterSet& charset,
            size_t length);
    virtual ~Pattern() = default;

    Pattern(const Pattern&) = delete;
    const Pattern& operator=(const Pattern&) = delete;

    const std::string& Expression() const;
    char LeftDelimiter() const;
    bool HasLeftDelimiter() const;
    char RightDelimiter() const;
    bool HasRightDelimiter() const;
    const CharacterSet& CharSet() const;

    size_t Length() const;
    bool IsFixedLength() const;

protected:
    std::string m_expr;
    char m_left_delimiter;
    char m_right_delimiter;
    CharacterSet m_charset;

    size_t m_len;
};

// Represents a plain literal text
class LiteralPattern : public Pattern {
public:
    LiteralPattern(const std::string& expression);
};

// Represents a format control text, identified by a leading '%'
class RegexPattern : public Pattern {
public:
    RegexPattern(const std::string& expression,
                 char left_delimiter,
                 char right_delimiter,
                 const CharacterSet& charset,
                 size_t length,
                 const std::string& description,
                 bool full_capture);

    const std::string& Description() const;

    bool IsFullCapture() const;

private:
    std::string m_desc;

    bool m_full_capture;
};
} // namespace tomcat_parser
#endif // TOMCAT_PARSER_PATTERN_H