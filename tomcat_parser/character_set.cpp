#include "character_set.h"

namespace tomcat_parser {
CharacterSet::CharacterSet(const std::string& str) {
    for (char c : str) {
        m_set.set(static_cast<size_t>(c));
    }
}

void CharacterSet::SetUnion(const CharacterSet& set) {
    m_set |= set.m_set;
}

void CharacterSet::Clear() {
    m_set.reset();
}

bool CharacterSet::IsDisjoint(const CharacterSet& set) const {
    return (m_set & set.m_set).none();
}

bool CharacterSet::Contains(char c) const {
    return m_set.test(static_cast<size_t>(c));
}

size_t CharacterSet::Count() const {
    return m_set.count();
}
} // namespace tomcat_parser