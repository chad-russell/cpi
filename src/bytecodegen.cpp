#include "bytecodegen.h"
#include "assembler.h"

void BytecodeGen::gen(Node *node) {
    switch (node->type) {
        case NodeType::FN_DECL: {
            for (auto stmt : node->fnDeclData.body) {
                gen(stmt);
            }
        } break;
        case NodeType::RET: {
            // 0 is always the relative offset of the return value, so...
            // store 0 {whatever the node data is}
            instructions.push_back(static_cast<unsigned char>(Instruction::STORE));
            auto zero = toBytes(static_cast<int32_t>(0));
            instructions.insert(instructions.end(), zero.begin(), zero.end());
            gen(node->nodeData);
            instructions.push_back(static_cast<unsigned char>(Instruction::EXIT));
        } break;
        case NodeType::INT_LITERAL: {
            instructions.push_back(static_cast<unsigned char>(Instruction::CONSTI32));
            auto value = toBytes(static_cast<int32_t>(node->intLiteralData.value));
            instructions.insert(instructions.end(), value.begin(), value.end());
        } break;
        default: assert(false);
    }
}