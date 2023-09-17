#ifndef TOMCAT_PARSER_PARSER_H
#define TOMCAT_PARSER_PARSER_H

#include <memory>
#include <regex>
#include <string>
#include <vector>

#include "log.h"
#include "pattern.h"
#include "pattern_factory.h"

namespace tomcat_parser {
class Parser {
public:
    Parser() = default;

    Parser(const Parser&) = delete;
    const Parser& operator=(const Parser&) = delete;

    void Process(const std::string& format, const std::vector<std::string>& input, std::vector<Log>& output);

private:
    bool GenPatterns(const std::string& format);

    Pattern* GetPattern(size_t index) const;

    void SplitPatterns();
    void SplitPatternsReverse();

    void GenPatternGroups();

    bool SplitSingleLine(const std::string& line);

    Log ProcessSingleLine(const std::string& line) const;

#ifdef DEBUG
    void LogPatterns(const std::string& message) const;
    void LogSplits(const std::string& message) const;
    void LogPatternGroups(const std::string& message) const;
    void LogSplitLineIndices(const std::string& message) const;
#endif

    PatternFactory m_pattern_factory;
    std::vector<std::unique_ptr<Pattern>> m_patterns;

    enum class SplitType : uint8_t {
        LENGTH,
        DELIMITER,
        CHARSET,
    } type;
    struct SplitSpec {
        SplitSpec(size_t len, size_t begin_index, size_t end_index)
            : type(SplitType::LENGTH), len(len), begin_index(begin_index), end_index(end_index) {}
        SplitSpec(char delimiter, size_t begin_index, size_t end_index)
            : type(SplitType::DELIMITER), delimiter(delimiter), begin_index(begin_index), end_index(end_index) {}
        SplitSpec(const CharacterSet& charset, size_t begin_index, size_t end_index)
            : type(SplitType::CHARSET), charset(charset), begin_index(begin_index), end_index(end_index) {}

        SplitType type;
        union {
            size_t len;
            char delimiter;
            CharacterSet charset;
        };
        size_t begin_index;
        size_t end_index;
    };
    std::vector<uint8_t> m_can_split;
    std::vector<SplitSpec> m_split_specs, m_split_specs_rev;

    struct PatternGroup {
        PatternGroup(const std::regex& regex,
                     const std::vector<RegexPattern*>& regex_patterns,
                     size_t begin_index,
                     size_t end_index)
            : regex(regex), regex_patterns(regex_patterns), begin_index(begin_index), end_index(end_index) {}

        std::regex regex;
        std::vector<RegexPattern*> regex_patterns;
        size_t begin_index;
        size_t end_index;
    };
    std::vector<PatternGroup> m_pattern_groups;

    std::vector<size_t> m_split_line_indices;
};
} // namespace tomcat_parser
#endif // TOMCAT_PARSER_PARSER_H