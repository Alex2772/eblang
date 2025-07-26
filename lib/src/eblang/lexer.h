#pragma once

#include "eblang/token/any_token.h"
#include <string_view>
#include <vector>


/**
 * @details
 * Lexer is the system that converts a string into tokens. For example:
 *
 * ```
 * fn main() {
 *     return 228 + 322;
 * }
 * ```
 *
 * Will be converted into:
 * ```
 * std::vector<Token> {
 *   Keyword::FN,
 *   Identifier{ "main" },
 *   LPar,
 *   RPar,
 *   LCurlyBracket,
 *   Keyword::RETURN,
 *   Integer{ 228 },
 *   Plus,
 *   Integer{ 322 },
 *   Semicolon,
 *   RCurlyBracket,
 * }
 * ```
 */
namespace eblang::lexer {
    std::vector<token::Any> process(std::string_view code);
}
