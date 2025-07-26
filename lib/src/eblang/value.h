#include <variant>
#include <string>

namespace eblang {
    using Value = std::variant<std::monostate /* aka void */, int, std::string>;
}