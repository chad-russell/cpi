#ifndef BYTECODEGEN_H
#define BYTECODEGEN_H

#include <unordered_map>
#include <queue>

#include "node.h"
#include "assembler.h"

struct Fixup {
    int64_t instOffset;
    Node *node;
};

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
    uint32_t genId = 1;

    int64_t debugLocalOffset = 0;

    // for going back at the end and setting call/jump locations
    // I guess this is like a poor man's linker
    vector_t<Fixup> fixups;

    void binopHelper(string instructionStr, Node *node, int32_t scale = 1);

    void gen(Node *node);
    void genDot(Node *node);
    void fixup();

    void storeValue(Node *node, int64_t offset);

    BytecodeGen();
};

bool hasNoLocalByDefault(Node *node);

#endif // BYTECODEGEN_H