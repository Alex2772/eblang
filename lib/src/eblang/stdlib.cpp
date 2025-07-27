//
// Created by alex2772 on 7/27/25.
//

#include <iostream>
#include "stdlib.h"

using namespace eblang;

namespace {
eblang::Value println(eblang::Context& state, const std::vector<eblang::Value>& args) {
    for (auto& arg : args) {
        std::cout << arg;
    }
    std::cout << std::endl;
    return std::monostate();
}
}   // namespace

void eblang::stdlib::init(eblang::State& state) { state.context().functions["println"] = { println }; }
