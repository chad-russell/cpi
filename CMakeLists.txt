cmake_minimum_required(VERSION 3.7)
project(cpi)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(/usr/local/opt/llvm@6/include)

execute_process(
        COMMAND
        "/usr/local/opt/llvm@6/bin/llvm-config"
        --cxxflags
        OUTPUT_VARIABLE LLVM_CXXFLAGS
        OUTPUT_STRIP_TRAILING_WHITESPACE)

execute_process(
        COMMAND
        "/usr/local/opt/llvm@6/bin/llvm-config"
        --ldflags
        OUTPUT_VARIABLE LLVM_LDFLAGS
        OUTPUT_STRIP_TRAILING_WHITESPACE)

execute_process(
        COMMAND
        "/usr/local/opt/llvm@6/bin/llvm-config"
        --system-libs
        OUTPUT_VARIABLE LLVM_SYSTEMLIBS
        OUTPUT_STRIP_TRAILING_WHITESPACE)

execute_process(
        COMMAND
        "/usr/local/opt/llvm@6/bin/llvm-config"
        --libs
        OUTPUT_VARIABLE LLVM_LIBS
        OUTPUT_STRIP_TRAILING_WHITESPACE)

set(CMAKE_CXX_FLAGS "${LLVM_CXXFLAGS} ${LLVM_LDFLAGS} ${LLVM_SYSTEMLIBS} ${LLVM_LIBS}")

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
        src/util.h src/llvmgen.cpp)

add_executable(cpi ${SOURCE_FILES})
