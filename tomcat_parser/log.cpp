#include "log.h"

namespace tomcat_parser {
Content::Content(const std::string& key, const std::string& value) : key(key), value(value) {
}

const std::string& Content::Key() const {
    return key;
}
const std::string& Content::Value() const {
    return value;
}

void Log::AddContent(const std::string& key, const std::string& value) {
    contents.emplace_back(key, value);
}

const std::vector<Content>& Log::Contents() const {
    return contents;
}
} // namespace tomcat_parser