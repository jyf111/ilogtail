#ifndef TOMCAT_PARSER_TEST_RANDOM_LOG_H
#define TOMCAT_PARSER_TEST_RANDOM_LOG_H

#include <random>
#include <string>

#include "character_set.h"
#include "pattern_factory.h"

namespace tomcat_parser {
// Random format and log generator
class RandomLog {
public:
    RandomLog();

    // Generates a random format with a minimum length of 'min_length'
    std::string GenFormat(size_t min_length);

    // Generates a random log with a minimum length of 'min_length' based on the input format
    std::string GenLog(const std::string& format, size_t min_length);

private:
    // Generates a random string of length 'length' based on the input character set
    std::string GenString(const std::string& charset, size_t length);
    std::string GenString(const CharacterSet& charset, size_t length);

    const std::string m_formats;
    const std::string m_keyformats;

    PatternFactory m_pattern_factory;

    std::mt19937 m_rng;
};
} // namespace tomcat_parser
#endif // TOMCAT_PARSER_TEST_RANDOM_LOG_H