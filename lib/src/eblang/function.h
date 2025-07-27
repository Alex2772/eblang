#pragma once

#include <functional>
#include "value.h"

namespace eblang {

struct Context;

struct Function {
    std::function<Value(Context& context, std::vector<Value> args)> value;
};
}