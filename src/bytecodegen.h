#ifndef BYTECODEGEN_H
#define BYTECODEGEN_H

#include <unordered_map>
#include <queue>

#include "node.h"
#include "assembler.h"

class BytecodeGen {
public:
    vector<unsigned char> instructions;
    SourceMap sourceMap;
    int64_t currentFnStackSize;
    queue<Node *> toProcess;
    bool isMainFn;
    unordered_map<int64_t, Node *> fixups;

    void binopHelper(string instructionStr, Node *node);

    void gen(Node *node);
};

#endif // BYTECODEGEN_H