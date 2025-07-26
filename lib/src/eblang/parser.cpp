//
// Created by alex2772 on 7/26/25.
//

#include "parser.h"
#include "expression.h"
#include "fmt/format.h"
#include <stdexcept>

template <typename... Lambdas>
struct match : Lambdas... {
    using Lambdas::operator()...;
};

template <typename... Lambdas>
match(Lambdas...) -> match<Lambdas...>;

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
                          break;
                      }
                      throw std::runtime_error(fmt::format("Expected ',' or ')' to close argument list, got {}", typeid(n).name()));
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
        int rightBindingPower = std::visit(
            match {
              [](token::Plus token) { return 0; },
              [](token::Minus token) { return 0; },
              [](token::Asterisk token) { return 10; },
              [](token::Slash token) { return 10; },
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
