#pragma once

#include <utility>
#include <memory>
#include <map>
#include <functional>
#include "context.h"
#include "fmt/format.h"
#include "util.h"

namespace eblang::expression {

struct Base {
    virtual ~Base() = default;

    virtual Value evaluate(Context& context) = 0;
};

using CommandSequence = std::vector<std::unique_ptr<eblang::expression::Base>>;

void execute(const CommandSequence& commands, Context& context);

struct Constant : Base {
    explicit Constant(Value value) : value(std::move(value)) {}
    ~Constant() override = default;

    Value value;
    Value evaluate(Context& context) override { return value; }
};

template <std::invocable<int, int> Op>
struct Binary : Base {
    Binary(std::unique_ptr<expression::Base> lhs, std::unique_ptr<expression::Base> rhs)
      : lhs(std::move(lhs)), rhs(std::move(rhs)) {}
    ~Binary() override = default;

    std::unique_ptr<expression::Base> lhs;
    std::unique_ptr<expression::Base> rhs;

    Value evaluate(Context& context) override {
        return std::visit(
            eblang::match {
              [](auto&& lhs, auto&& rhs) -> Value {
                  if constexpr (requires { Op{}(lhs, rhs); }) {
                      return Op{}(lhs, rhs);
                  }
                  throw std::runtime_error(fmt::format("Can't perform {} on {} and {}", typeid(Op).name(), typeid(lhs).name(), typeid(rhs).name()));
              },
            },
            lhs->evaluate(context), rhs->evaluate(context));
    }
};

struct VariableAssignment : Base {
    VariableAssignment(std::string name, std::unique_ptr<expression::Base> value)
      : name(std::move(name)), value(std::move(value)) {}
    ~VariableAssignment() override = default;
    std::string name;
    std::unique_ptr<expression::Base> value;

    Value evaluate(Context& context) override { return context.variables[name] = value->evaluate(context); }
};

struct FunctionCall : Base {
    FunctionCall(std::string name, std::vector<std::unique_ptr<expression::Base>> args)
      : name(std::move(name)), args(std::move(args)) {}
    ~FunctionCall() override = default;
    std::string name;
    std::vector<std::unique_ptr<expression::Base>> args;

    Value evaluate(Context& context) override;
};

struct VariableReference : Base {
    explicit VariableReference(std::string name) : name(std::move(name)) {}
    ~VariableReference() override = default;
    std::string name;
    Value evaluate(Context& context) override {
        auto it = context.variables.find(name);
        if (it == context.variables.end()) {
            throw std::runtime_error("Variable not found: " + name);
        }
        return it->second;
    }
};

struct If : Base {
    If(std::unique_ptr<expression::Base> condition, CommandSequence body)
      : condition(std::move(condition)), body(std::move(body)) {}
    ~If() override = default;

    std::unique_ptr<expression::Base> condition;
    CommandSequence body;

    Value evaluate(Context& context) override {
        if (std::get<int>(condition->evaluate(context)) != 0) {
            expression::execute(body, context);
        }
        return std::monostate {};
    }
};

struct Return : Base {
    explicit Return(std::unique_ptr<expression::Base> expression) : expression(std::move(expression)) {}
    ~Return() override = default;

    std::unique_ptr<expression::Base> expression;

    Value evaluate(Context& context) override {
        context.returnValue = expression != nullptr ? expression->evaluate(context) : std::monostate {};
        return std::monostate {};
    }
};

}   // namespace eblang::expression
