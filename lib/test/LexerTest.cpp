#include "eblang/lexer.h"
#include "eblang/token/any_token.h"
#include <gtest/gtest.h>

using namespace eblang::token;

TEST(Lexer, Case1) {
    auto processed = eblang::lexer::process(R"(
fn main() {
    return 228 + 322;
}
)");
    std::vector<Any> expected{
        Keyword::FN,
        Identifier { .value = "main" },
        LPar{},
        RPar{},
        LCurlyBracket{},
        Keyword::RETURN,
        Integer { .value = 228 },
        Plus{},
        Integer { .value = 322 },
        Semicolon{},
        RCurlyBracket{},
    };

    EXPECT_EQ(processed, expected);
}
