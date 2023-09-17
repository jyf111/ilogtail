#ifndef TOMCAT_PARSER_UTIL_H
#define TOMCAT_PARSER_UTIL_H

#include <memory>
#include <string>

namespace tomcat_parser {
template <typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args) {
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

const std::string& AlphaStr();

const std::string& DigitStr();

const std::string& NonSpaceStr();

const std::string& PrintableStr();

const std::string& UniverseStr();
} // namespace tomcat_parser
#endif // TOMCAT_PARSER_UTIL_H