#include <iostream>
#include "value.h"
#include "util.h"

namespace {
void impl(std::ostream& os, const eblang::Value& value) {
    std::visit(
        eblang::match {
          [&](std::monostate) { os << "<void>"; },
          [&](auto&& v) { os << v; },
        },
        value);
}
}   // namespace

std::ostream& eblang::operator<<(std::ostream& os, const eblang::Value& value) {
    impl(os, value);
    return os;
}
