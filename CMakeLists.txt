cmake_minimum_required(VERSION 3.7)
project(cpi)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

set(CMAKE_BUILD_TYPE Debug)
#set(CMAKE_BUILD_TYPE Release)

set(SOURCE_FILES
        src/assembler.cpp
        src/assembler.h
        src/bytecodegen.cpp
        src/bytecodegen.h
        src/interpreter.cpp
        src/interpreter.h
        src/lexer.cpp
        src/lexer.h
        src/main.cpp
        src/node.cpp
        src/node.h
        src/parser.cpp
        src/parser.h
        src/semantic.cpp
        src/semantic.h
        src/util.cpp
        src/util.h)

add_executable(cpi ${SOURCE_FILES})