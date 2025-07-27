#include <variant>
#include <string>

namespace eblang {
    using Value = std::variant<std::monostate /* aka void */, int, std::string>;
    std::ostream& operator<<(std::ostream& os, const Value& value);
}