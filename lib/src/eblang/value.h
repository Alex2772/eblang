#include <variant>
#include <string>

namespace eblang {
    using Value = std::variant<int, std::string>;
}