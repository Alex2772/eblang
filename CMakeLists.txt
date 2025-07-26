cmake_minimum_required(VERSION 3.31)
project(eblang)

set(CMAKE_CXX_STANDARD 20)

include(FetchContent)
FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.14.0
)
FetchContent_MakeAvailable(googletest)

FetchContent_Declare(
        fmt
        GIT_REPOSITORY https://github.com/fmtlib/fmt.git
        GIT_TAG 11.2.0
)
FetchContent_MakeAvailable(fmt)

add_subdirectory(lib)

add_executable(eblang main.cpp)
