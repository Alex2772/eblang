#pragma once

#include <map>
#include <string>
#include "value.h"
#include "function.h"
#include <optional>

namespace eblang {
struct Context {
    std::map<std::string, Function> functions;
    std::map<std::string, Value> variables;
    std::optional<Value> returnValue;
};
}