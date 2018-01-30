#ifndef BYTECODEGEN_H
#define BYTECODEGEN_H

#include "node.h"

class BytecodeGen {
public:
    vector<unsigned char> instructions;

    void gen(Node *node);
};

#endif // BYTECODEGEN_H