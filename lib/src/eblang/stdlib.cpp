//
// Created by alex2772 on 7/27/25.
//

#include <iostream>
#include "stdlib.h"
#include "util.h"

using namespace eblang;

namespace {
eblang::Value println(eblang::Context& state, const std::vector<eblang::Value>& args) {
    for (auto& arg : args) {
        std::cout << arg;
    }
    std::cout << std::endl;
    return std::monostate();
}

eblang::Value input(eblang::Context& state, const std::vector<eblang::Value>& args) {
    std::string str;
    std::cin >> str;
    return str;
}

eblang::Value toint(eblang::Context& state, const std::vector<eblang::Value>& args) {
    if (args.size() != 1) {
        throw std::runtime_error("toint expects 1 argument");
    }
    return std::visit(
        eblang::match {
          [](auto&& v) -> int { return int(v); },
          [](const std::string& s) -> int {
              return std::stoi(s);
          },
          [](std::monostate) -> int { throw std::runtime_error("toint: can't convert void to int"); },
        },
        args[0]);
}
}   // namespace

void eblang::stdlib::init(eblang::State& state) {
#define DEFINE(name) state.context().functions[#name] = { name }

    DEFINE(println);
    DEFINE(input);
    DEFINE(toint);
}
