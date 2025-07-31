# Expressively Bare Language (eblang)

Video: https://youtu.be/Zk676WxebzE

## Overview

Expressively Bare Language (eblang) is a minimalist programming language designed with simplicity and expressiveness in
mind. It aims to provide a clean and straightforward syntax.

This programming language is not intended for real world applications; instead, it is a demonstration of implementing
your own language.

## Project Structure

- `src/` - Source code of the eblang compiler/interpreter
- `lib/` - Core library components
- `example_programs/` - Example programs written in eblang
- `cmake-build-debug/` - Build directory (generated)

## Building from Source

### Prerequisites

- CMake (3.x or higher)
- C++ compiler with C++11 support or higher

### Build Instructions

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

# Usage

```bash
./eblang example_programs/hello_world.eblang
```
