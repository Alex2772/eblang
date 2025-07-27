#pragma once

namespace eblang {
template <typename... Lambdas>
struct match : Lambdas... {
    using Lambdas::operator()...;
};

template <typename... Lambdas>
match(Lambdas...) -> match<Lambdas...>;
}   // namespace eblang
