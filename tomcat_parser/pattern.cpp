#include "pattern.h"

namespace tomcat_parser {
Pattern::Pattern(const std::string& expression,
                 char left_delimiter,
                 char right_delimiter,
                 const CharacterSet& charset,
                 size_t length)
    : m_expr(expression),
      m_left_delimiter(left_delimiter),
      m_right_delimiter(right_delimiter),
      m_charset(charset),
      m_len(length) {
}

const std::string& Pattern::Expression() const {
    return m_expr;
}

char Pattern::LeftDelimiter() const {
    return m_left_delimiter;
}

char Pattern::RightDelimiter() const {
    return m_right_delimiter;
}

const CharacterSet& Pattern::CharSet() const {
    return m_charset;
}

size_t Pattern::Length() const {
    return m_len;
}

bool Pattern::IsFixedLength() const {
    return m_len != 0;
}

LiteralPattern::LiteralPattern(const std::string& expression)
    : Pattern(expression, expression.front(), expression.back(), CharacterSet(expression), expression.size()) {
}

RegexPattern::RegexPattern(const std::string& expression,
                           char left_delimiter,
                           char right_delimiter,
                           const CharacterSet& charset,
                           size_t length,
                           const std::string& description,
                           bool full_capture)
    : Pattern(expression, left_delimiter, right_delimiter, charset, length),
      m_desc(description),
      m_full_capture(full_capture) {
}

const std::string& RegexPattern::Description() const {
    return m_desc;
}

bool RegexPattern::IsFullCapture() const {
    return m_full_capture;
}
} // namespace tomcat_parser