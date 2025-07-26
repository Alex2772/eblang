#pragma once


#include "eblang/token/any_token.h"
#include "expression.h"
#include <memory>
#include <span>

namespace eblang {

    class Parser {
    public:
        explicit Parser(std::span<eblang::token::Any> tokens) : mTokens(tokens) {}

        std::unique_ptr<eblang::expression::Base> parseExpression() {
            return parseExpression(0);
        }

    private:
        std::span<eblang::token::Any> mTokens;

        std::unique_ptr<eblang::expression::Base> parseExpression(int leftBindingPower);

        const token::Any& peek() const {
            return mTokens.front();
        }

        token::Any take() {
            auto token = std::move(mTokens.front());
            mTokens = mTokens.subspan(1);
            return token;
        }
    };

}