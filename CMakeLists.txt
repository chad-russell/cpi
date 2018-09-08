cmake_minimum_required(VERSION 3.5.1)
project(cpi)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# MAC OS
include_directories(/usr/local/opt/llvm@6/include /usr/include/ffi)
set(LLVM_CONFIG_PATH "/usr/local/opt/llvm@6/bin/llvm-config")

# WINDOWS
#include_directories(/usr/include/llvm-5.0)
#include_directories(/usr/include/llvm-c-5.0)
#set(LLVM_CONFIG_PATH "/usr/bin/llvm-config-5.0")

execute_process(
        COMMAND
        ${LLVM_CONFIG_PATH}
        --cxxflags
        OUTPUT_VARIABLE LLVM_INCLUDEDIR
        OUTPUT_STRIP_TRAILING_WHITESPACE)

execute_process(
        COMMAND
        ${LLVM_CONFIG_PATH}
        --ldflags
        OUTPUT_VARIABLE LLVM_LDFLAGS
        OUTPUT_STRIP_TRAILING_WHITESPACE)

execute_process(
        COMMAND
        ${LLVM_CONFIG_PATH}
        --system-libs
        OUTPUT_VARIABLE LLVM_SYSTEMLIBS
        OUTPUT_STRIP_TRAILING_WHITESPACE)

execute_process(
        COMMAND
        ${LLVM_CONFIG_PATH}
        --libs
        OUTPUT_VARIABLE LLVM_LIBS
        OUTPUT_STRIP_TRAILING_WHITESPACE)

set(CMAKE_CXX_FLAGS "${LLVM_INCLUDEDIR} ${LLVM_LDFLAGS} ${LLVM_SYSTEMLIBS} ${LLVM_LIBS} -Qunused-arguments")

set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

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
        src/util.h
        src/llvmgen.cpp
        src/container.h)

add_executable(cpi ${SOURCE_FILES})

target_link_libraries(cpi ffi zmq)
