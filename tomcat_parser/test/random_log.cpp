#include "random_log.h"

#include <algorithm>
#include <iostream>

#include "pattern_factory.h"
#include "util.h"
namespace tomcat_parser {
RandomLog::RandomLog()
    : m_formats("aAbBDFhHIlmpqrsStTuUvX"),
      m_keyformats("aiocrspt"),
      m_pattern_factory(),
      m_rng(std::random_device()()) {
}

std::string RandomLog::GenFormat(size_t min_length) {
    std::string format;
    std::string charset = PrintableStr();
    charset.erase(std::remove(charset.begin(), charset.end(), '%'), charset.end());
    charset.erase(std::remove(charset.begin(), charset.end(), '\n'), charset.end());

    while (format.size() < min_length) {
        switch (m_rng() % 3) {
            case 0:
                format += GenString(charset, m_rng() % 8);
                break;
            case 1:
                format += '%';
                format += m_formats[m_rng() % m_formats.size()];
                break;
            default:
                format += '%';
                format += '{' + GenString(AlphaStr() + DigitStr(), m_rng() % 5 + 1) + '}';
                format += m_keyformats[m_rng() % m_keyformats.size()];
        }
    }
    return format;
}

std::string RandomLog::GenLog(const std::string& format, size_t min_length) {
    const char CONTROL = '%';
    std::string log;
    int try_cnt = 0;
    do {
        ++try_cnt;
        log.clear();
        for (size_t len = format.size(), i = 0; i < len;) {
            if (format[i] == CONTROL) {
                if (i + 1 == len)
                    return "";
                if (format[i + 1] == CONTROL) {
                    log += CONTROL;
                    i += 2;
                } else {
                    if (format[i + 1] == '{') {
                        size_t j = i + 2;
                        while (j < len && format[j] != '}')
                            ++j;
                        if (j + 1 >= len)
                            return "";
                        auto pattern
                            = m_pattern_factory.GenRegexPattern(format.substr(i + 1, j - i + 1)); // [i + 1, j + 1]
                        if (!pattern)
                            return "";
                        log += GenString(pattern->CharSet(),
                                         pattern->IsFixedLength() ? pattern->Length()
                                                                  : min_length - m_rng() % min_length);
                        i = j + 2;
                    } else {
                        auto pattern = m_pattern_factory.GenRegexPattern(format.substr(i + 1, 1)); // [i + 1, i + 1]
                        if (!pattern)
                            return "";
                        if (format[i + 1] == 't')
                            log += '[';
                        log += GenString(pattern->CharSet(),
                                         pattern->IsFixedLength() ? pattern->Length()
                                                                  : min_length - m_rng() % min_length);
                        if (format[i + 1] == 't')
                            log += ']';
                        i += 2;
                    }
                }
            } else {
                log += format[i];
                ++i;
            }
        }
    } while (log.size() < min_length || try_cnt < 100);
    return log.size() >= min_length ? log : "";
}

std::string RandomLog::GenString(const std::string& charset, size_t length) {
    std::string str;
    std::uniform_int_distribution<int> dist(0, charset.size() - 1);
    for (size_t i = 0; i < length; i++) {
        str += charset[dist(m_rng)];
    }
    return str;
}

std::string RandomLog::GenString(const CharacterSet& charset, size_t length) {
    std::string str;
    for (char c = '\0';; c++) {
        if (charset.Contains(c))
            str += c;
        if (c == '\177')
            break;
    }
    return GenString(str, length);
}
} // namespace tomcat_parser