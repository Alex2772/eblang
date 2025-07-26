//
// Created by alex2772 on 7/26/25.
//

#include "lexer.h"
#include "fmt/format.h"
#include <charconv>
#include <map>
#include <stdexcept>

using namespace eblang;
using namespace eblang::token;

std::vector<token::Any> eblang::lexer::process(std::string_view code) {
    int line = 1;
    std::vector<token::Any> result;

    for (auto it = code.begin(); it != code.end(); ++it) {
        auto remainingString = std::ranges::subrange(it, code.end());
        auto findBlank = [&] {
            return std::ranges::find_if(remainingString, [](char c) {
                switch (c) {
                    case ' ':
                    case '\t':
                    case '\n':
                        return true;
                    default:
                        return false;
                }
            });
        };
        switch (*it) {
            default:
                if (std::isalpha(*it)) {
                    auto blank = std::ranges::find_if(remainingString, [](char c) {
                        return !std::isalnum(c) && c != '_';
                    });
                    std::string_view valueString(it, blank);
                    it = std::prev(blank);

                    static std::map<std::string_view, Keyword> keywords = {
                            {"return", Keyword::RETURN},
                            {"fn", Keyword::FN},
                    };

                    if (auto keyword = keywords.find(valueString); keyword != keywords.end()) {
                        result.push_back(keyword->second);
                        break;
                    }
                    result.push_back(Identifier{.value = std::string(valueString)});
                    break;
                }

                if (std::isdigit(*it)) {
                    auto end = std::ranges::find_if(remainingString, [](char c) {
                        return !std::isdigit(c);
                    });
                    std::string_view digitString(it, end);
                    int integer = 0;
                    std::from_chars(digitString.data(), digitString.data() + digitString.size(), integer);
                    result.push_back(Integer{.value = integer});
                    it = std::prev(end);
                    break;
                }

                throw std::runtime_error(fmt::format("Unexpected character: '{}', at line {}", *it, line));

            case ' ':
            case '\r':
            case '\t':
                break;

            case '\n':
                ++line;
                break;

            case '(':
                result.push_back(LPar{});
                break;
            case ')':
                result.push_back(RPar{});
                break;
            case '{':
                result.push_back(LCurlyBracket{});
                break;
            case '}':
                result.push_back(RCurlyBracket{});
                break;
            case ';':
                result.push_back(Semicolon{});
                break;
            case '+':
                result.push_back(Plus{});
                break;
            case '-':
                result.push_back(Minus{});
                break;
            case '*':
                result.push_back(Asterisk{});
                break;
            case '/':
                result.push_back(Slash{});
                break;
            case ',':
                result.push_back(Comma{});
                break;

            case '"':
                auto end = std::ranges::find(std::ranges::subrange(std::next(remainingString.begin()), remainingString.end()), '"');
                if (end == remainingString.end()) {
                    throw std::runtime_error(fmt::format("string literal is not finished, at line {}", line));
                }
                result.push_back(String{.value = std::string(std::next(it), end)});
                it = end;
                break;
        }
        if (it == code.end()) {
            break;
        }
    }
    return result;
}
