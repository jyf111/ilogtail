#ifndef TOMCAT_PARSER_CHARACTER_SET_H
#define TOMCAT_PARSER_CHARACTER_SET_H

#include <bitset>
#include <string>
#include <vector>

namespace tomcat_parser {
class CharacterSet {
public:
    CharacterSet() = default;
    explicit CharacterSet(const std::string& str);

    void SetUnion(const CharacterSet& set);

    void Clear();

    bool IsDisjoint(const CharacterSet& set) const;

    bool Contains(char c) const;

    size_t Count() const;

private:
    std::bitset<128> m_set; // ASCII
};
} // namespace tomcat_parser
#endif // TOMCAT_PARSER_CHARACTER_SET_H