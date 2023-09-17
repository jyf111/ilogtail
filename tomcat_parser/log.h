#ifndef TOMCAT_PARSER_LOG_H
#define TOMCAT_PARSER_LOG_H

#include <string>
#include <vector>

namespace tomcat_parser {
class Content {
public:
    Content(const std::string& key, const std::string& value);

    const std::string& Key() const;
    const std::string& Value() const;

private:
    std::string key;
    std::string value;
};

class Log {
public:
    void AddContent(const std::string& key, const std::string& value);

    const std::vector<Content>& Contents() const;

private:
    std::vector<Content> contents;
};
} // namespace tomcat_parser
#endif // TOMCAT_PARSER_LOG_H