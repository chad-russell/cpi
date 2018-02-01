#ifndef BYTECODEGEN_H
#define BYTECODEGEN_H

#include "node.h"
#include "assembler.h"

class BytecodeGen {
public:
    vector<unsigned char> instructions;
    vector<unsigned long> sourceMap;

    void gen(Node *node);
};

#endif // BYTECODEGEN_H