//
// Created by alex2772 on 7/26/25.
//

#include "parser.h"
#include "expression.h"
#include "fmt/format.h"
#include "util.h"
#include <stdexcept>
#include <cassert>

std::unique_ptr<eblang::expression::Base> eblang::Parser::parseExpression(int leftBindingPower) {
    // pratt parsing
    auto lhs = std::visit(
        match {
          [](token::Integer token) -> std::unique_ptr<expression::Base> {
              return std::make_unique<expression::Constant>(token.value);
          },
          [](token::String token) -> std::unique_ptr<expression::Base> {
              return std::make_unique<expression::Constant>(std::move(token.value));
          },
          [&](token::LPar token) -> std::unique_ptr<expression::Base> {
              auto lhs = parseExpression(0);
              auto n = take();
              if (!std::holds_alternative<token::RPar>(n)) {
                  throw std::runtime_error(fmt::format("Expected ')' to close '(', got {}", typeid(n).name()));
              }
              return lhs;
          },
          [&](token::Identifier identifier) -> std::unique_ptr<expression::Base> {
              // 1. variable reference
              // 2. function call
              if (std::holds_alternative<token::LPar>(peek())) {
                  take();
                  std::vector<std::unique_ptr<expression::Base>> args;
                  for (;;) {
                      auto n = peek();
                      if (std::holds_alternative<token::RPar>(n)) {
                          take();
                          if (!args.empty()) {
                              throw std::runtime_error("Unexpected ')' after ','");
                          }
                          break;
                      }
                      args.push_back(parseExpression());
                      if (std::holds_alternative<token::Comma>(peek())) {
                          take();
                          continue;
                      }
                      if (std::holds_alternative<token::RPar>(peek())) {
                          take();
                          break;
                      }
                      throw std::runtime_error(
                          fmt::format("Expected ',' or ')' to close argument list, got {}", typeid(n).name()));
                  }
                  return std::make_unique<expression::FunctionCall>(std::move(identifier.value), std::move(args));
              }
              return std::make_unique<expression::VariableReference>(std::move(identifier.value));
          },
          [](auto&& token) -> std::unique_ptr<expression::Base> {
              throw std::runtime_error(fmt::format("Unexpected token: {}", typeid(token).name()));
          },
        },
        take());
    for (;;) {
        if (mTokens.empty()) {
            break;
        }
        if (std::holds_alternative<token::RPar>(peek())) {
            break;
        }
        if (std::holds_alternative<token::Semicolon>(peek())) {
            break;
        }
        if (std::holds_alternative<token::Comma>(peek())) {
            break;
        }
        if (std::holds_alternative<token::Equal>(peek())) {
            take();
            auto rhs = parseExpression(0);
            auto variableReference = dynamic_cast<expression::VariableReference*>(lhs.get());
            if (!variableReference) {
                throw std::runtime_error("Expected variable reference on left side of assignment");
            }
            lhs = std::make_unique<expression::VariableAssignment>(std::move(variableReference->name), std::move(rhs));
            break;
        }

        int rightBindingPower = std::visit(
            match {
              [](token::Equal2 token) { return 0; },
              [](token::Plus token) { return 10; },
              [](token::Minus token) { return 10; },
              [](token::Asterisk token) { return 20; },
              [](token::Slash token) { return 20; },
              [](auto&& token) -> int {
                  throw std::runtime_error(fmt::format("Unexpected token: {}", typeid(token).name()));
              },
            },
            peek());

        if (rightBindingPower < leftBindingPower) {
            return lhs;
        }

        auto opToken = take();
        auto rhs = parseExpression(rightBindingPower);

        lhs = std::visit(
            match {
              [&](token::Equal2 token) -> std::unique_ptr<expression::Base> {
                  return std::make_unique<expression::Binary<std::equal_to<>>>(std::move(lhs), std::move(rhs));
              },
              [&](token::Plus token) -> std::unique_ptr<expression::Base> {
                  return std::make_unique<expression::Binary<std::plus<>>>(std::move(lhs), std::move(rhs));
              },
              [&](token::Minus token) -> std::unique_ptr<expression::Base> {
                  return std::make_unique<expression::Binary<std::minus<>>>(std::move(lhs), std::move(rhs));
              },
              [&](token::Asterisk token) -> std::unique_ptr<expression::Base> {
                  return std::make_unique<expression::Binary<std::multiplies<>>>(std::move(lhs), std::move(rhs));
              },
              [&](token::Slash token) -> std::unique_ptr<expression::Base> {
                  return std::make_unique<expression::Binary<std::divides<>>>(std::move(lhs), std::move(rhs));
              },
              [](auto&& token) -> std::unique_ptr<expression::Base> {
                  throw std::runtime_error(fmt::format("Unexpected token: {}", typeid(token).name()));
              },
            },
            opToken);
    }
    return lhs;
}

eblang::expression::CommandSequence eblang::Parser::parseCommandSequence() {
    std::vector<std::unique_ptr<expression::Base>> expressions;
    while (!mTokens.empty()) {
        if (auto keyword = std::get_if<token::Keyword>(&peek())) {
            if (*keyword == token::Keyword::IF) {
                expressions.push_back(parseIfStatement());
                break;
            }
        }
        if (std::holds_alternative<token::RCurlyBracket>(peek())) {
            break;
        }
        if (std::holds_alternative<token::Semicolon>(peek())) {
            take();
            continue;
        }
        expressions.push_back(parseExpression());
    }
    return expressions;
}

std::unique_ptr<eblang::expression::If> eblang::Parser::parseIfStatement() {
    assert(std::get<token::Keyword>(peek()) == token::Keyword::IF);
    take();
    if (!std::holds_alternative<token::LPar>(take())) {
        throw std::runtime_error("Expected '(' after 'if'");
    }
    auto condition = parseExpression();
    if (!std::holds_alternative<token::RPar>(take())) {
        throw std::runtime_error("Expected ')' after condition");
    }
    if (!std::holds_alternative<token::LCurlyBracket>(peek())) {
        throw std::runtime_error("Expected '{' after ')'");
    }
    auto body = parseCommandBlock();
    return std::make_unique<eblang::expression::If>(std::move(condition), std::move(body));
}

eblang::expression::CommandSequence eblang::Parser::parseCommandBlock() {
    auto f1 = take();
    assert(std::holds_alternative<token::LCurlyBracket>(f1));
    auto result = parseCommandSequence();
    auto f2 = take();
    if (!std::holds_alternative<token::RCurlyBracket>(f2)) {
        throw std::runtime_error("Expected '}' that closes the command block");
    }
    return result;
}
