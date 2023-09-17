#ifndef TOMCAT_PARSER_TEST_RANDOM_LOG_H
#define TOMCAT_PARSER_TEST_RANDOM_LOG_H

#include <random>
#include <string>

#include "character_set.h"
#include "pattern_factory.h"

namespace tomcat_parser {
class RandomLog {
public:
    RandomLog();

    std::string GenFormat(size_t min_length);

    std::string GenLog(const std::string& format, size_t min_length);

private:
    std::string GenString(const std::string& charset, size_t length);
    std::string GenString(const CharacterSet& charset, size_t length);

    std::string m_formats;
    std::string m_keyformats;

    PatternFactory m_pattern_factory;

    std::mt19937 m_rng;
};
} // namespace tomcat_parser
#endif // TOMCAT_PARSER_TEST_RANDOM_LOG_H