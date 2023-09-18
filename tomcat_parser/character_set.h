#ifndef TOMCAT_PARSER_CHARACTER_SET_H
#define TOMCAT_PARSER_CHARACTER_SET_H

#include <bitset>
#include <string>

namespace tomcat_parser {
// Using a bitset to represent a character set (considering only ASCII now).
// Each bit at index 'i' represents the character with the ASCII value of 'i' in the set
class CharacterSet {
public:
    CharacterSet() = default;
    explicit CharacterSet(const std::string& str);

    // Performs a set union between this set and the input set
    void SetUnion(const CharacterSet& set);

    void Clear();

    // Checks if this set is disjoint with the input set
    bool IsDisjoint(const CharacterSet& set) const;

    // Checks if this set contains character 'c'
    bool Contains(char c) const;

    size_t Count() const;

private:
    std::bitset<128> m_set;
};
} // namespace tomcat_parser
#endif // TOMCAT_PARSER_CHARACTER_SET_H