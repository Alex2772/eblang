#pragma once

#include <utility>
#include <memory>

#include "value.h"
namespace eblang::expression {
    struct Base {
        virtual ~Base() = default;

        virtual Value evaluate() = 0;
    };

    struct Constant: Base {
        explicit Constant(Value value) : value(std::move(value)) {}
        ~Constant() override = default;

        Value value;
        Value evaluate() override {
            return value;
        }
    };

    template<std::invocable<int, int> Op>
    struct Binary: Base {
        Binary(std::unique_ptr<expression::Base> lhs, std::unique_ptr<expression::Base> rhs) : lhs(std::move(lhs)), rhs(std::move(rhs)) {}
        ~Binary() override = default;

        std::unique_ptr<expression::Base> lhs;
        std::unique_ptr<expression::Base> rhs;

        Value evaluate() override {
            return Op{}(std::get<int>(lhs->evaluate()), std::get<int>(rhs->evaluate()));
        }
    };
}

