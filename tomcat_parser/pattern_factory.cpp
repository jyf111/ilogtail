#include "pattern_factory.h"

#include "util.h"

namespace tomcat_parser {
PatternFactory::PatternFactory() {
    // see <https://tomcat.apache.org/tomcat-8.5-doc/config/valve.html#Access_Logging>
    m_format_table['a'] = {
        .expr = R"((?:\d{1,3}\.){3}\d{1,3}|([0-9A-Faf]{0,4}:){2,7}([0-9A-Fa-f]{0,4}))",
        .charset = "abcdefABCDEF.:" + DigitStr(),
        .desc = "remoteAddr",
        .left_delimiter = '\0',
        .right_delimiter = '\0',
        .len = 0,
        .full_capture = true,
    }; // ipv4 or ipv6
    m_format_table['A'] = {
        .expr = R"((?:\d{1,3}\.){3}\d{1,3}|([0-9A-Faf]{0,4}:){2,7}([0-9A-Fa-f]{0,4}))",
        .charset = "abcdefABCDEF.:" + DigitStr(),
        .desc = "localAddr",
        .left_delimiter = '\0',
        .right_delimiter = '\0',
        .len = 0,
        .full_capture = true,
    };
    m_format_table['b'] = {
        .expr = R"((\d+|-))",
        .charset = "-" + DigitStr(),
        .desc = "size",
        .left_delimiter = '\0',
        .right_delimiter = '\0',
        .len = 0,
        .full_capture = true,
    }; // 0 is '-'
    m_format_table['B'] = {
        .expr = R"((\d+))",
        .charset = DigitStr(),
        .desc = "byteSentNC",
        .left_delimiter = '\0',
        .right_delimiter = '\0',
        .len = 0,
        .full_capture = true,
    };
    m_format_table['D'] = {
        .expr = R"((\d+))",
        .charset = DigitStr(),
        .desc = "elapsedTime",
        .left_delimiter = '\0',
        .right_delimiter = '\0',
        .len = 0,
        .full_capture = true,
    };
    m_format_table['F'] = {
        .expr = R"((\d+))",
        .charset = DigitStr(),
        .desc = "firstByteTime",
        .left_delimiter = '\0',
        .right_delimiter = '\0',
        .len = 0,
        .full_capture = true,
    };
    m_format_table['h'] = {
        .expr = R"((\S+))",
        .charset = NonSpaceStr(),
        .desc = "host",
        .left_delimiter = '\0',
        .right_delimiter = '\0',
        .len = 0,
        .full_capture = true,
    };
    m_format_table['H'] = {
        .expr = R"((\S+))",
        .charset = NonSpaceStr(),
        .desc = "protocol",
        .left_delimiter = '\0',
        .right_delimiter = '\0',
        .len = 0,
        .full_capture = true,
    };
    m_format_table['I'] = {
        .expr = R"((\S+))",
        .charset = NonSpaceStr(),
        .desc = "threadName",
        .left_delimiter = '\0',
        .right_delimiter = '\0',
        .len = 0,
        .full_capture = true,
    };
    m_format_table['l'] = {
        .expr = "(-)",
        .charset = "-",
        .desc = "logicalUserName",
        .left_delimiter = '-',
        .right_delimiter = '-',
        .len = 1,
        .full_capture = true,
    }; // always '-'
    m_format_table['m'] = {
        .expr = R"(([a-zA-Z]+))",
        .charset = AlphaStr(),
        .desc = "method",
        .left_delimiter = '\0',
        .right_delimiter = '\0',
        .len = 0,
        .full_capture = true,
    };
    m_format_table['p'] = {
        .expr = R"((\d+))",
        .charset = DigitStr(),
        .desc = "port",
        .left_delimiter = '\0',
        .right_delimiter = '\0',
        .len = 0,
        .full_capture = true,
    };
    m_format_table['q'] = {
        .expr = R"((\S*))",
        .charset = NonSpaceStr(),
        .desc = "query",
        .left_delimiter = '\0',
        .right_delimiter = '\0',
        .len = 0,
        .full_capture = true,
    }; // can be empty
    m_format_table['r'] = {
        .expr = R"((.+?))",
        .charset = AlphaStr() + DigitStr()
            + "_.-~!*'();:@&=+$,/?#[]", // see <https://stackoverflow.com/questions/1856785/characters-allowed-in-a-url>
        .desc = "request",
        .left_delimiter = '\0',
        .right_delimiter = '\0',
        .len = 0,
        .full_capture = true,
    };
    m_format_table['s'] = {
        .expr = R"((\d+))",
        .charset = DigitStr(),
        .desc = "statusCode",
        .left_delimiter = '\0',
        .right_delimiter = '\0',
        .len = 0,
        .full_capture = true,
    };
    m_format_table['S'] = {
        .expr = R"((\S+))",
        .charset = NonSpaceStr(),
        .desc = "sessionId",
        .left_delimiter = '\0',
        .right_delimiter = '\0',
        .len = 0,
        .full_capture = true,
    };
    m_format_table['t'] = {.expr = R"(\[(.+)\])",
                           .charset = UniverseStr(),
                           .desc = "time",
                           .left_delimiter = '\0',
                           .right_delimiter = '\0',
                           .len = 0,
                           .full_capture = false};
    m_format_table['T'] = {
        .expr = R"((\d+))",
        .charset = DigitStr(),
        .desc = "elapsedTimeS",
        .left_delimiter = '[',
        .right_delimiter = ']',
        .len = 0,
        .full_capture = true,
    };
    m_format_table['u'] = {
        .expr = R"((\S+))",
        .charset = NonSpaceStr(),
        .desc = "user",
        .left_delimiter = '\0',
        .right_delimiter = '\0',
        .len = 0,
        .full_capture = true,
    };
    m_format_table['U'] = {
        .expr = R"((\S+))",
        .charset = NonSpaceStr(),
        .desc = "path",
        .left_delimiter = '\0',
        .right_delimiter = '\0',
        .len = 0,
        .full_capture = true,
    };
    m_format_table['v'] = {
        .expr = R"((\S+))",
        .charset = NonSpaceStr(),
        .desc = "localServerName",
        .left_delimiter = '\0',
        .right_delimiter = '\0',
        .len = 0,
        .full_capture = true,
    };
    m_format_table['X'] = {
        .expr = R"(([X+-]))",
        .charset = "X+-",
        .desc = "connectionStatus",
        .left_delimiter = '\0',
        .right_delimiter = '\0',
        .len = 1,
        .full_capture = true,
    };

    m_keyformat_table['a'] = {
        .expr = R"((\S+))",
        .charset = NonSpaceStr(),
        .desc = "remoteAddr-",
        .left_delimiter = '\0',
        .right_delimiter = '\0',
        .len = 0,
        .full_capture = true,
    };
    // formally, "c", "i", "o", "r" or "s" results in a sub object with subkey "xxx",
    // not a key value pair with key "key-xxx"
    m_keyformat_table['i'] = {
        .expr = R"((.+?))",
        .charset = UniverseStr(),
        .desc = "requestHeaders-",
        .left_delimiter = '\0',
        .right_delimiter = '\0',
        .len = 0,
        .full_capture = true,
    };
    m_keyformat_table['o'] = {
        .expr = R"((.+?))",
        .charset = UniverseStr(),
        .desc = "responseHeaders-",
        .left_delimiter = '\0',
        .right_delimiter = '\0',
        .len = 0,
        .full_capture = true,
    };
    m_keyformat_table['c'] = {
        .expr = R"((.+?))",
        .charset = UniverseStr(),
        .desc = "cookies-",
        .left_delimiter = '\0',
        .right_delimiter = '\0',
        .len = 0,
        .full_capture = true,
    };
    m_keyformat_table['r'] = {
        .expr = R"((.+?))",
        .charset = UniverseStr(),
        .desc = "requestAttributes-",
        .left_delimiter = '\0',
        .right_delimiter = '\0',
        .len = 0,
        .full_capture = true,
    };
    m_keyformat_table['s'] = {
        .expr = R"((.+?))",
        .charset = UniverseStr(),
        .desc = "sessionAttributes-",
        .left_delimiter = '\0',
        .right_delimiter = '\0',
        .len = 0,
        .full_capture = true,
    };
    m_keyformat_table['p'] = {
        .expr = R"((\d+))",
        .charset = DigitStr(),
        .desc = "port-",
        .left_delimiter = '\0',
        .right_delimiter = '\0',
        .len = 0,
        .full_capture = true,
    };
    m_keyformat_table['t'] = {
        .expr = R"((.+?))",
        .charset = UniverseStr(),
        .desc = "time-",
        .left_delimiter = '\0',
        .right_delimiter = '\0',
        .len = 0,
        .full_capture = true,
    };
}

std::unique_ptr<LiteralPattern> PatternFactory::GenLiteralPattern(const std::string& expression) const {
    return make_unique<LiteralPattern>(expression);
}

// Returns nullptr when the expression is an invalid format control text
std::unique_ptr<RegexPattern> PatternFactory::GenRegexPattern(const std::string& expression) const {
    size_t p1 = 0;
    if (expression[p1] == '{') { // {xxx}a
        size_t p2 = expression.find_last_of('}');
        if (p2 != std::string::npos && expression.size() == p2 + 2) {
            auto iter = m_keyformat_table.find(expression[p2 + 1]);
            if (iter != m_format_table.end()) {
                return make_unique<RegexPattern>(iter->second.expr,
                                                 iter->second.left_delimiter,
                                                 iter->second.right_delimiter,
                                                 CharacterSet(iter->second.charset),
                                                 iter->second.len,
                                                 iter->second.desc
                                                     + expression.substr(p1 + 1, p2 - p1 - 1), // [p1 + 1, p2 - 1]
                                                 iter->second.full_capture);
            }
        }
    } else { // a
        if (expression.size() == 1) {
            auto iter = m_format_table.find(expression[0]);
            if (iter != m_format_table.end()) {
                return make_unique<RegexPattern>(iter->second.expr,
                                                 iter->second.left_delimiter,
                                                 iter->second.right_delimiter,
                                                 CharacterSet(iter->second.charset),
                                                 iter->second.len,
                                                 iter->second.desc,
                                                 iter->second.full_capture);
            }
        }
    }
    return nullptr;
}
} // namespace tomcat_parser