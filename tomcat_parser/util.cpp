#include "util.h"

namespace tomcat_parser {
const std::string& AlphaStr() {
    static const std::string ALPHA = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    return ALPHA;
}

const std::string& DigitStr() {
    static const std::string DIGIT = "0123456789";
    return DIGIT;
}

const std::string& NonSpaceStr() {
    static std::string NONSPACE;
    if (NONSPACE.empty()) {
        for (char c = '\0';; c++) {
            if (!std::iscntrl(c) && !std::isspace(c))
                NONSPACE += c;
            if (c == '\177')
                break;
        }
    }
    return NONSPACE;
}

const std::string& PrintableStr() {
    static std::string PRINTABLE;
    if (PRINTABLE.empty()) {
        for (char c = '\0';; c++) {
            if (std::isprint(c))
                PRINTABLE += c;
            if (c == '\177')
                break;
        }
    }
    return PRINTABLE;
}

const std::string& UniverseStr() {
    static std::string UNIVERSE;
    if (UNIVERSE.empty()) {
        for (char c = '\0';; c++) {
            if (!std::iscntrl(c))
                UNIVERSE += c;
            if (c == '\177')
                break;
        }
    }
    return UNIVERSE;
}
} // namespace tomcat_parser