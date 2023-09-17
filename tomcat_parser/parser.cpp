#include "parser.h"

#ifdef DEBUG
#include <iostream>
#endif

namespace tomcat_parser {
void Parser::Process(const std::string& format, const std::vector<std::string>& input, std::vector<Log>& output) {
    if (format.empty() || !GenPatterns(format))
        return; // ill-formed format
#ifdef DEBUG
    LogPatterns("Generate patterns:");
#endif

    m_can_split.clear();
    m_split_specs.clear();
    m_split_specs_rev.clear();
    m_can_split.resize(m_patterns.size() - 1);
    const int ROUND = 2;
    for (int i = 0; i < ROUND; i++) {
        !(i & 1) ? SplitPatterns() : SplitPatternsReverse();
#ifdef DEBUG
        LogSplits("Split patterns round " + std::to_string(i) + ":");
#endif
    }

    GenPatternGroups();
#ifdef DEBUG
    LogPatternGroups("Generate pattern groups:");
#endif

    for (const auto& line : input) {
        if (!SplitSingleLine(line)) {
            output.emplace_back();
            continue;
        }
#ifdef DEBUG
        LogSplitLineIndices("Split single line:");
#endif

        output.push_back(ProcessSingleLine(line));
    }
}

bool Parser::GenPatterns(const std::string& format) {
    m_patterns.clear();

    const char CONTROL = '%';
    std::string literal_str;
    for (size_t len = format.size(), i = 0; i < len;) {
        if (format[i] == CONTROL) {
            if (i + 1 == len)
                return false;
            if (format[i + 1] == CONTROL) { // %%
                literal_str += CONTROL;
                i += 2;
            } else {
                if (!literal_str.empty()) {
                    m_patterns.push_back(m_pattern_factory.GenLiteralPattern(literal_str));
                    literal_str.clear();
                }
                if (format[i + 1] == '{') { // %{xxx}
                    size_t j = i + 1;
                    while (j < len && format[j] != '}')
                        ++j;
                    if (j + 1 >= len)
                        return false;
                    std::unique_ptr<RegexPattern> regex_pattern
                        = m_pattern_factory.GenRegexPattern(format.substr(i + 1, j - i + 1)); // [i + 1, j + 1]
                    if (!regex_pattern)
                        return false;
                    m_patterns.push_back(std::move(regex_pattern));
                    i = j + 2;
                } else {
                    std::unique_ptr<RegexPattern> regex_pattern
                        = m_pattern_factory.GenRegexPattern(format.substr(i + 1, 1)); // [i + 1, i + 1]
                    if (!regex_pattern)
                        return false;
                    m_patterns.push_back(std::move(regex_pattern));
                    i += 2;
                }
            }
        } else {
            size_t j = i + 1;
            while (j < len && format[j] != CONTROL)
                ++j;
            literal_str += format.substr(i, j - i); // [i, j - 1]
            i = j;
        }
    }
    if (!literal_str.empty())
        m_patterns.push_back(m_pattern_factory.GenLiteralPattern(literal_str));
    return true;
}

Pattern* Parser::GetPattern(size_t index) const {
    if (index < m_patterns.size()) {
        auto literal_pattern = dynamic_cast<LiteralPattern*>(m_patterns[index].get());
        if (literal_pattern) {
            return literal_pattern;
        } else {
            return dynamic_cast<RegexPattern*>(m_patterns[index].get());
        }
    }
    return nullptr;
}

void Parser::SplitPatterns() {
    CharacterSet charset;
    Pattern* cur_pattern = nullptr;
    Pattern* nxt_pattern = GetPattern(0);
    for (size_t len = m_patterns.size(), begin_index = 0, i = 0; i + 1 < len; i++) {
        cur_pattern = nxt_pattern;
        nxt_pattern = GetPattern(i + 1);

        if (m_can_split[i]) {
            charset.Clear();
            begin_index = i + 1;
            continue;
        }

        charset.SetUnion(cur_pattern->CharSet());
        if ((!i || m_can_split[i - 1]) && cur_pattern->IsFixedLength()) {
            m_can_split[i] = 1;
            m_split_specs.emplace_back(cur_pattern->Length(), begin_index, i);
            charset.Clear();
            begin_index = i + 1;
        } else if (nxt_pattern->LeftDelimiter() && !charset.Contains(nxt_pattern->LeftDelimiter())) {
            m_can_split[i] = 1;
            m_split_specs.emplace_back(nxt_pattern->LeftDelimiter(), begin_index, i);
            charset.Clear();
            begin_index = i + 1;
        } else if (charset.IsDisjoint(nxt_pattern->CharSet())) {
            m_can_split[i] = 1;
            m_split_specs.emplace_back(charset, begin_index, i);
            charset.Clear();
            begin_index = i + 1;
        }
    }
}

void Parser::SplitPatternsReverse() {
    CharacterSet charset;
    Pattern* cur_pattern = nullptr;
    Pattern* pre_pattern = GetPattern(m_patterns.size() - 1);
    for (size_t len = m_patterns.size(), end_index = len - 1, i = len - 1; i >= 1; i--) {
        cur_pattern = pre_pattern;
        pre_pattern = GetPattern(i - 1);

        if (m_can_split[i - 1]) {
            charset.Clear();
            end_index = i - 1;
            continue;
        }

        charset.SetUnion(cur_pattern->CharSet());
        if ((i == len - 1 || m_can_split[i]) && cur_pattern->IsFixedLength()) {
            m_can_split[i - 1] = 1;
            m_split_specs_rev.emplace_back(cur_pattern->Length(), i, end_index);
            charset.Clear();
            end_index = i - 1;
        } else if (pre_pattern->RightDelimiter() && !charset.Contains(pre_pattern->LeftDelimiter())) {
            m_can_split[i - 1] = 1;
            m_split_specs_rev.emplace_back(pre_pattern->RightDelimiter(), i, end_index);
            charset.Clear();
            end_index = i - 1;
        } else if (charset.IsDisjoint(pre_pattern->CharSet())) {
            m_can_split[i - 1] = 1;
            m_split_specs_rev.emplace_back(charset, i, end_index);
            charset.Clear();
            end_index = i - 1;
        }
    }
}

void Parser::GenPatternGroups() {
    m_pattern_groups.clear();
    for (size_t len = m_patterns.size(), i = 0; i < len;) {
        size_t j = i;
        while (j + 1 < len && !m_can_split[j])
            ++j;
        std::vector<RegexPattern*> regex_patterns;
        for (size_t k = i; k <= j; k++) {
            auto regex_pattern = dynamic_cast<RegexPattern*>(m_patterns[k].get());
            if (regex_pattern)
                regex_patterns.push_back(regex_pattern);
            ++k;
        }
        if (!regex_patterns.empty()) {
            std::string regexp;
            for (size_t k = i; k <= j; k++)
                regexp += GetPattern(k)->Expression();
            m_pattern_groups.emplace_back(std::regex(regexp), regex_patterns, i, j);
        }
        i = j + 1;
    }
}

bool Parser::SplitSingleLine(const std::string& line) {
    if (line.empty())
        return false;
    m_split_line_indices.clear();
    m_split_line_indices.resize(m_patterns.size());
    for (const auto& split_spec : m_split_specs) {
        size_t index = split_spec.begin_index ? m_split_line_indices[split_spec.begin_index - 1] + 1 : 0;
        if (split_spec.type == SplitType::LENGTH) {
            if (index + split_spec.len > line.size())
                return false;
            index += split_spec.len;
        } else if (split_spec.type == SplitType::DELIMITER) {
            while (index < line.size() && line[index] != split_spec.delimiter)
                ++index;
        } else {
            while (index < line.size() && split_spec.charset.Contains(line[index]))
                ++index;
        }
        if (!index)
            return false;
        m_split_line_indices[split_spec.end_index] = index - 1;
    }

    m_split_line_indices.back() = line.size() - 1;
    for (const auto& split_spec : m_split_specs_rev) {
        size_t index = m_split_line_indices[split_spec.end_index];
        if (split_spec.type == SplitType::LENGTH) {
            if (index < split_spec.len)
                return false;
            index -= split_spec.len;
        } else if (split_spec.type == SplitType::DELIMITER) {
            while (line[index] != split_spec.delimiter) {
                --index;
                if (!index)
                    return false;
            }
        } else {
            while (split_spec.charset.Contains(line[index])) {
                --index;
                if (!index)
                    return false;
            }
        }
        m_split_line_indices[split_spec.begin_index - 1] = index;
    }
    return true;
}

Log Parser::ProcessSingleLine(const std::string& line) const {
    Log log;
    std::smatch matches;
    for (const auto& pattern_group : m_pattern_groups) {
        size_t line_begin = pattern_group.begin_index ? m_split_line_indices[pattern_group.begin_index - 1] + 1 : 0;
        size_t line_end = m_split_line_indices[pattern_group.end_index];
        if (pattern_group.regex_patterns.size() == 1 && pattern_group.regex_patterns[0]->IsFullCapture()) {
            log.AddContent(pattern_group.regex_patterns[0]->Description(),
                           line.substr(line_begin, line_end - line_begin + 1));
        } else {
            if (std::regex_match(
                    line.begin() + line_begin, line.begin() + line_end + 1, matches, pattern_group.regex)) {
                for (size_t i = 1; i < matches.size(); i++)
                    log.AddContent(pattern_group.regex_patterns[i - 1]->Description(), matches[i].str());
            }
        }
    }
    return log;
}

#ifdef DEBUG
void Parser::LogPatterns(const std::string& message) const {
    std::cerr << message << '\n';
    size_t index = 0;
    for (const auto& pattern : m_patterns) {
        if (dynamic_cast<LiteralPattern*>(pattern.get())) {
            auto literal_pattern = dynamic_cast<LiteralPattern*>(pattern.get());
            std::cerr << '[' << index << "] \"" << literal_pattern->Expression() << '"' << '\n';
        } else {
            auto regex_pattern = dynamic_cast<RegexPattern*>(pattern.get());
            std::cerr << '[' << index << "] \"" << regex_pattern->Expression() << '"' << ' '
                      << regex_pattern->Description() << '\n';
        }
        ++index;
    }
}

void Parser::LogSplits(const std::string& message) const {
    std::cerr << message << '\n';
    for (auto p : m_can_split)
        std::cerr << static_cast<int>(p);
    std::cerr << '\n';
    for (const auto& split_spec : m_split_specs) {
        if (split_spec.type == SplitType::LENGTH)
            std::cerr << "[" << split_spec.begin_index << ", " << split_spec.end_index << "] "
                      << "Split (" << split_spec.end_index << ", " << split_spec.end_index + 1
                      << ") by length: " << split_spec.len << '\n';
        else if (split_spec.type == SplitType::DELIMITER)
            std::cerr << "[" << split_spec.begin_index << ", " << split_spec.end_index << "] "
                      << "Split (" << split_spec.end_index << ", " << split_spec.end_index + 1 << ") by delimiter: '"
                      << split_spec.delimiter << "'\n";
        else
            std::cerr << "[" << split_spec.begin_index << ", " << split_spec.end_index << "] "
                      << "Split (" << split_spec.end_index << ", " << split_spec.end_index + 1
                      << ") by character set: " << split_spec.charset.Count() << '\n';
    }
    for (const auto& split_spec : m_split_specs_rev) {
        if (split_spec.type == SplitType::LENGTH)
            std::cerr << "[" << split_spec.begin_index << ", " << split_spec.end_index << "] "
                      << "Split (" << split_spec.begin_index - 1 << ", " << split_spec.begin_index
                      << ") by length: " << split_spec.len << '\n';
        else if (split_spec.type == SplitType::DELIMITER)
            std::cerr << "[" << split_spec.begin_index << ", " << split_spec.end_index << "] "
                      << "Split (" << split_spec.end_index << ", " << split_spec.end_index + 1 << ") by delimiter: '"
                      << split_spec.delimiter << "'\n";
        else
            std::cerr << "[" << split_spec.begin_index << ", " << split_spec.end_index << "] "
                      << "Split (" << split_spec.begin_index - 1 << ", " << split_spec.begin_index
                      << ") by character set: " << split_spec.charset.Count() << '\n';
    }
}

void Parser::LogPatternGroups(const std::string& message) const {
    std::cerr << message << '\n';
    for (const auto& pattern_group : m_pattern_groups) {
        std::cerr << "[" << pattern_group.begin_index << ", " << pattern_group.end_index << "] ";
        for (auto pattern : pattern_group.regex_patterns)
            std::cerr << pattern->Expression() << ' ';
        std::cerr << '\n';
    }
}

void Parser::LogSplitLineIndices(const std::string& message) const {
    std::cerr << message << '\n';
    for (size_t index : m_split_line_indices) {
        if (index)
            std::cerr << index << ' ';
    }
    std::cerr << '\n';
}
#endif
} // namespace tomcat_parser