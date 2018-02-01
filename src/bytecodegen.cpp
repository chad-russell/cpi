#include "bytecodegen.h"

void append(vector<unsigned char> &instructions, vector<unsigned char> newInstructions) {
    instructions.insert(instructions.end(), newInstructions.begin(), newInstructions.end());
}

void append(vector<unsigned char> &instructions, unsigned char instruction) {
    instructions.push_back(instruction);
}

void append(vector<unsigned char> &instructions, Instruction instruction) {
    append(instructions, static_cast<unsigned char>(instruction));
}

Node *resolveNode(Node *node) {
    if (node == nullptr) { return nullptr; }

    auto resolved = node->resolved;
    if (resolved == nullptr) { return node; }

    while (resolved->resolved != nullptr) {
        resolved = resolved->resolved;
    }

    return resolved;
}

void BytecodeGen::gen(Node *node) {
    if (node->gen) { return; }
    node->gen = true;

    if (node->sourceMapStatement) {
        vector<unsigned long> moreSourceMap = {node->region.start.line, node->region.start.col,
                                               node->region.end.line, node->region.end.col,
                                               instructions.size()};
        sourceMap.insert(sourceMap.end(), moreSourceMap.begin(), moreSourceMap.end());
    }

    switch (node->type) {
        case NodeType::FN_DECL: {
            for (auto stmt : node->fnDeclData.body) {
                gen(stmt);
                append(instructions, stmt->bytecode);
            }
        } break;
        case NodeType::RET: {
            // 0 is always the relative offset of the return value, so...
            // store 0 {whatever the node data is}
            gen(node->retData.value);

            append(node->bytecode, Instruction::STORE);
            append(node->bytecode, toBytes(static_cast<int32_t>(0)));
            append(node->bytecode, node->retData.value->bytecode);
            append(node->bytecode, Instruction::EXIT);
        } break;
        case NodeType::INT_LITERAL: {
            append(node->bytecode, Instruction::CONSTI32);
            auto litData = static_cast<int32_t>(node->intLiteralData.value);
            append(node->bytecode, toBytes(litData));
        } break;
        case NodeType::DECL: {
            auto data = node->declData;
            if (data.initialValue == nullptr) { return; }

            gen(data.initialValue);

            auto resolvedInitialValue = resolveNode(data.initialValue);
            gen(resolvedInitialValue);

            auto localOffset = node->localOffset;

            append(node->bytecode, Instruction::STORE);
            append(node->bytecode, toBytes(static_cast<int32_t>(localOffset)));
            append(node->bytecode, data.initialValue->bytecode);
        } break;
        case NodeType::SYMBOL: {
            auto resolved = resolveNode(node);
            auto localOffset = resolved->localOffset;
            append(node->bytecode, Instruction::I32);
            append(node->bytecode, toBytes(static_cast<int32_t>(localOffset)));
        } break;
        default: assert(false);
    }
}
