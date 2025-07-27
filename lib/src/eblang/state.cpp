//
// Created by alex2772 on 7/26/25.
//

#include "state.h"
#include "lexer.h"
#include "parser.h"

using namespace eblang;

eblang::Value State::evaluate(std::string_view expression) {
    auto tokens = lexer::process(expression);
    Parser parser(tokens);
    return parser.parseExpression()->evaluate(mContext);
}

void State::run(std::string_view code) {
    auto tokens = lexer::process(code);
    Parser parser(tokens);
    for (const auto& i : parser.parseCommandSequence()) {
        i->evaluate(mContext);
    }
}
