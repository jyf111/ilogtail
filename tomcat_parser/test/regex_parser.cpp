#include "regex_parser.h"

namespace tomcat_parser {
void RegexParser::Process(const std::string& format, const std::vector<std::string>& input, std::vector<Log>& output) {
    if (format.empty() || !GenPattern(format))
        return; // ill-formed format

    for (const auto& line : input) {
        output.push_back(ProcessSingleLine(line));
    }
}

bool RegexParser::GenPattern(const std::string& format) {
    m_regex_patterns.clear();

    const char CONTROL = '%';
    std::string regexp;
    for (size_t len = format.size(), i = 0; i < len;) {
        if (format[i] == CONTROL) {
            if (i + 1 == len)
                return false;
            if (format[i + 1] == CONTROL) { // %%
                regexp += CONTROL;
                i += 2;
            } else if (format[i + 1] == '{') { // %{xxx}
                size_t j = i + 1;
                while (j < len && format[j] != '}')
                    ++j;
                if (j + 1 >= len)
                    return false;
                std::unique_ptr<RegexPattern> regex_pattern
                    = m_pattern_factory.GenRegexPattern(format.substr(i + 1, j - i + 1)); // [i + 1, j + 1]
                if (!regex_pattern)
                    return false;
                m_regex_patterns.push_back(std::move(regex_pattern));
                regexp += m_regex_patterns.back()->Expression();
                i = j + 2;
            } else {
                std::unique_ptr<RegexPattern> regex_pattern
                    = m_pattern_factory.GenRegexPattern(format.substr(i + 1, 1)); // [i + 1, i + 1]
                if (!regex_pattern)
                    return false;
                m_regex_patterns.push_back(std::move(regex_pattern));
                regexp += m_regex_patterns.back()->Expression();
                i += 2;
            }
        } else {
            size_t j = i + 1;
            while (j < len && format[j] != CONTROL)
                ++j;
            regexp += format.substr(i, j - i); // [i, j - 1]
            i = j;
        }
    }
    m_regex = std::regex(regexp);
    return true;
}

Log RegexParser::ProcessSingleLine(const std::string& line) const {
    Log log;
    std::smatch matches;
    if (std::regex_match(line.begin(), line.end(), matches, m_regex)) {
        for (size_t i = 1; i < matches.size(); i++) {
            log.AddContent(m_regex_patterns[i - 1]->Description(), matches[i].str());
        }
    }
    return log;
}
} // namespace tomcat_parser