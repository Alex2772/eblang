#pragma once

#include <functional>
#include "value.h"

namespace eblang {

struct Context;

struct Function {
    std::function<Value(Context&)> value;

    struct Argument {
        std::string name;
    };

    std::vector<Argument> args;
};
}