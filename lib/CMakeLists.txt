project(eblang_lib)

file(GLOB_RECURSE SRCS src/*.cpp)
add_library(eblang_lib STATIC ${SRCS})
target_include_directories(eblang_lib PUBLIC src)
target_link_libraries(eblang_lib PUBLIC fmt::fmt)

file(GLOB_RECURSE TEST_SRCS test/*.cpp)

add_executable(eblang_tests ${TEST_SRCS})
target_link_libraries(eblang_tests gtest gtest_main eblang_lib)
