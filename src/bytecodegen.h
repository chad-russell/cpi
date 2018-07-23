#ifndef BYTECODEGEN_H
#define BYTECODEGEN_H

#include <unordered_map>
#include <queue>

#include "node.h"
#include "assembler.h"

class BytecodeGen {
public:
    vector<unsigned char> instructions = {};
    hash_t<uint32_t, uint64_t> *fnTable;

    SourceMap sourceMap = {};
    int64_t currentFnStackSize;

    queue<Node *> toProcess;
    bool processFnDecls = false;

    vector_t<Node *> generatedNodes = vector_init<Node *>(16);
    vector_t<Node *> externalFnTable = vector_init<Node *>(8);

    bool isMainFn;

    // for going back at the end and setting call/jump locations
    // I guess this is like a poor man's linker
    hash_t<int64_t, Node *> *fixups;

    void binopHelper(string instructionStr, Node *node, int32_t scale = 1);

    void gen(Node *node);
    void fixup();

    void storeValue(Node *node, int32_t offset);

    BytecodeGen();
};

bool hasNoLocalByDefault(Node *node);

#endif // BYTECODEGEN_H