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

void BytecodeGen::binopHelper(string instructionStr, Node *node) {
    string bytecodeStr;

    auto kind = node->typeInfo->typeData.kind;
    switch (kind) {
        case NodeTypekind::I32: {
            instructionStr.append("I32");
            bytecodeStr = "I32";
        } break;
        case NodeTypekind::I64: {
            instructionStr.append("I64");
            bytecodeStr = "I64";
        } break;
        default: assert(false);
    }

    auto inst = AssemblyLexer::nameToInstruction[instructionStr];
    append(instructions, inst);

    auto bytecodeInst = AssemblyLexer::nameToInstruction[bytecodeStr];
    append(node->bytecode, bytecodeInst);

    append(instructions, node->binopData.lhs->bytecode);
    append(instructions, node->binopData.rhs->bytecode);

    append(instructions, toBytes(static_cast<int32_t>(node->localOffset)));
    append(node->bytecode, toBytes(static_cast<int32_t>(node->localOffset)));
}

void BytecodeGen::gen(Node *node) {
    if (node->gen) { return; }
    node->gen = true;

    if (node->sourceMapStatement) {
        sourceMap.statements.push_back(SourceMapStatement{
                node->region.start.line,
                node->region.start.byteIndex,
                node->region.end.byteIndex,
                instructions.size()
        });
    }

    switch (node->type) {
        case NodeType::FN_DECL: {
            node->fnDeclData.instOffset = instructions.size();

            auto stackSize = static_cast<int32_t>(node->fnDeclData.stackSize);
            append(instructions, Instruction::BUMPSP);
            append(instructions, toBytes(stackSize));

            auto savedCurrentFnStackSize = currentFnStackSize;
            currentFnStackSize = node->fnDeclData.stackSize;

            for (auto stmt : node->fnDeclData.body) {
                gen(stmt);
                append(instructions, stmt->bytecode);
            }

            currentFnStackSize = savedCurrentFnStackSize;
        } break;
        case NodeType::RET: {
            // todo(chad): don't do any storing if this is just 'ret' with no value

            // 0 is always the relative offset of the return value, so...
            // store 0 {whatever the node data is}
            gen(node->retData.value);

            append(node->bytecode, Instruction::STORE);
            append(node->bytecode, toBytes(static_cast<int32_t>(0)));
            append(node->bytecode, node->retData.value->bytecode);
            if (isMainFn) {
                append(node->bytecode, Instruction::EXIT);
            } else {
                append(node->bytecode, Instruction::RET);
            }
        } break;
        case NodeType::INT_LITERAL: {
            switch (node->typeInfo->typeData.kind) {
                case NodeTypekind::I32: {
                    append(node->bytecode, Instruction::CONSTI32);
                    auto litData = static_cast<int32_t>(node->intLiteralData.value);
                    append(node->bytecode, toBytes(litData));
                } break;
                case NodeTypekind::I64: {
                    append(node->bytecode, Instruction::CONSTI64);
                    auto litData = node->intLiteralData.value;
                    append(node->bytecode, toBytes(litData));
                } break;
                default: assert(false);
            }
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
            switch (resolved->typeInfo->typeData.kind) {
                case NodeTypekind::I32: {
                    append(node->bytecode, Instruction::I32);
                } break;
                case NodeTypekind::I64: {
                    append(node->bytecode, Instruction::I64);
                } break;
                default: assert(false);
            }
            append(node->bytecode, toBytes(static_cast<int32_t>(localOffset)));
        } break;
        case NodeType::BINOP: {
            gen(node->binopData.lhs);
            gen(node->binopData.rhs);

            switch (node->binopData.type) {
                case LexerTokenType::ADD: {
                    binopHelper("ADD", node);
                } break;
                case LexerTokenType::SUB: {
                    binopHelper("SUB", node);
                } break;
                case LexerTokenType::MUL: {
                    binopHelper("MUL", node);
                } break;
                case LexerTokenType::DIV: {
                    binopHelper("SDIV", node);
                } break;
                default: assert(false);
            }
        } break;
        case NodeType::FN_CALL: {
            int32_t paramSize = 0;

//            if (paramSize > 0) {
//                append(node->bytecode, Instruction::BUMPSP);
//                append(node->bytecode, toBytes(paramSize));
//            }

            // todo(chad): store params onto the stack

            append(instructions, Instruction::CALL);

            auto resolvedFn = resolve(node->fnCallData.fn);
            fixups.insert({instructions.size(), resolvedFn});

            append(instructions, toBytes(static_cast<int32_t>(999)));

            append(node->bytecode, Instruction::I64);
            // + 8 for the saving of the return address and base pointer
            append(node->bytecode, toBytes(static_cast<int32_t>(currentFnStackSize + 8)));

            toProcess.push(resolvedFn);
        } break;
        default: assert(false);
    }
}
