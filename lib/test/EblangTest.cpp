#include "eblang/state.h"
#include <gtest/gtest.h>

TEST(Eblang, Case1) {
    eblang::State g;
    EXPECT_EQ(std::get<int>(g.evaluate("1 + 2 * 3")), 7);
}

TEST(Eblang, Case2) {
    eblang::State g;
    EXPECT_EQ(std::get<int>(g.evaluate("2 * 3 + 1")), 7);
}

TEST(Eblang, Case3) {
    eblang::State g;
    EXPECT_EQ(std::get<int>(g.evaluate("(1 + 2) * 3")), 9);
}

TEST(Eblang, Case4) {
    eblang::State g;
    EXPECT_EQ(std::get<int>(g.evaluate("(2 * 3) + 1")), 7);
}

TEST(Eblang, Case5) {
    eblang::State g;
    EXPECT_EQ(std::get<int>(g.evaluate("3 * (1 + 2)")), 9);
}
TEST(Eblang, Case6) {
    eblang::State g;
    EXPECT_EQ(std::get<int>(g.evaluate("(3 * 1) + 2")), 5);
}
