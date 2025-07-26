//
// Created by alex2772 on 7/26/25.
//

#include "state.h"
#include "lexer.h"
#include "parser.h"

using namespace eblang;

eblang::Value State::evaluate(std::string_view code) {
    auto tokens = lexer::process(code);
    Parser parser(tokens);
    return parser.parseExpression()->evaluate(mContext);
}
