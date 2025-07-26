#pragma once

#include "value.h"
#include "expression.h"
#include <any>
#include <string_view>

namespace eblang {
    class State {
    public:
        Value evaluate(std::string_view code);

        Context& context() { return mContext; }

    private:
        Context mContext;
    };
}