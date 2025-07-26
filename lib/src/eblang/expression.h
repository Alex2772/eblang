#pragma once

#include <utility>
#include <memory>
#include <map>
#include <functional>
#include "context.h"
#include "fmt/format.h"

namespace eblang::expression {

struct Base {
    virtual ~Base() = default;

    virtual Value evaluate(Context& context) = 0;
};

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
        return Op {}(std::get<int>(lhs->evaluate(context)), std::get<int>(rhs->evaluate(context)));
    }
};

struct FunctionCall : Base {
    FunctionCall(std::string name, std::vector<std::unique_ptr<expression::Base>> args)
      : name(std::move(name)), args(std::move(args)) {}
    ~FunctionCall() override = default;
    std::string name;
    std::vector<std::unique_ptr<expression::Base>> args;

    Value evaluate(Context& context) override {
        auto it = context.functions.find(name);
        if (it == context.functions.end()) {
            throw std::runtime_error(fmt::format("Function not found: {}", name));
        }
        if (it->second.args.size() != args.size()) {
            throw std::runtime_error(fmt::format(
                "Function {} expects {} arguments, but {} were provided", name, it->second.args.size(), args.size()));
        }
        auto prevVariables = std::move(context.variables);
        for (std::size_t i = 0; i < args.size(); ++i) {
            context.variables[it->second.args[i].name] = args[i]->evaluate(context);
        }
        auto result = it->second.value(context);
        context.variables = std::move(prevVariables);
        return result;
    }
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
}   // namespace eblang::expression
