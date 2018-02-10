#include "bytecodegen.h"

#include <string.h>

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

    append(instructions, bytecodeInst);
    append(instructions, toBytes(node->locals[0]->offset));

    append(instructions, bytecodeInst);
    append(instructions, toBytes(node->locals[1]->offset));

    append(instructions, toBytes(static_cast<int32_t>(node->locals[2]->offset)));
    append(node->bytecode, toBytes(static_cast<int32_t>(node->locals[2]->offset)));
}

void BytecodeGen::gen(Node *node) {
    // avoids generating code for a fn decl within another fn decl
    // todo(chad): make sure this is the right way to do this
    if (node->type == NodeType::FN_DECL && !processFnDecls) {
        if (node->fnDeclData.isLiteral && node->bytecode.empty()) {
            // placeholder
            append(node->bytecode, Instruction::CONSTI32);
            append(node->bytecode, toBytes(static_cast<int32_t>(node->fnDeclData.tableIndex)));
        }

        toProcess.push(node);
        return;
    }

    if (node->gen) { return; }
    node->gen = true;

    if (node->sourceMapStatement) {
        sourceMap.statements.push_back(SourceMapStatement{
                instructions.size(),
                instructions.size(),

                node->region.start.line,
                node->region.start.byteIndex,
                node->region.end.byteIndex
        });
    }

    switch (node->type) {
        case NodeType::FN_DECL: {
            processFnDecls = false;

            node->fnDeclData.instOffset = instructions.size();

            fnTable.insert({node->fnDeclData.tableIndex, node->fnDeclData.instOffset});

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

            // copy the value into our local
//            storeValue(instructions, node->retData.value, node->locals[0]->offset);

            // NOTE(CHAD): @REMEMBER: when doing codegen for assignment, there should just be a totally different
            // codepath. The number of cases to handle is small (symbol, decl, deref, dot)
            // and the should dramatically simplify the code for the normal paths.
            // normally we can just always assume we want the actual data, and for lvalue handling
            // we will focus on generating an offset to write to.

            copyBytes(instructions, 0, node->retData.value->localOffset, typeSize(node->retData.value->typeInfo));

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
                case NodeTypekind::INT_LITERAL:
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
            copyBytes(instructions, node->locals[0]->offset, data.initialValue->localOffset, typeSize(data.type));
            node->localOffset = node->locals[0]->offset;
        } break;
        case NodeType::ASSIGN: {
            auto data = node->assignData;

            gen(data.rhs);

            auto resolvedDecl = resolve(data.lhs);
            if (resolvedDecl->type == NodeType::DECL) {
                auto localOffset = static_cast<int32_t>(resolvedDecl->localOffset);
                storeValue(node->bytecode, data.rhs, localOffset);
            } else if (resolvedDecl->type == NodeType::DEREF) {
                gen(data.lhs);

                // store rvalue into it's slot
                storeValue(node->bytecode, data.rhs, data.rhs->localOffset);

                auto localOffset = static_cast<int32_t>(resolve(resolvedDecl->nodeData)->localOffset);

                append(node->bytecode, Instruction::STORE);

                append(node->bytecode, Instruction::I32);
                append(node->bytecode, toBytes(static_cast<int32_t>(localOffset)));

                append(node->bytecode, Instruction::CONSTI32);
                append(node->bytecode, toBytes(static_cast<int32_t>(data.rhs->localOffset)));

                append(node->bytecode, toBytes(static_cast<int32_t>(typeSize(resolvedDecl->nodeData->typeInfo))));
            } else if (resolvedDecl->type == NodeType::DOT) {
                gen(data.lhs);

                // store the rvalue into it's slot
                storeValue(node->bytecode, data.rhs, data.rhs->localOffset);

                auto foundParam = resolvedDecl->dotData.resolved;
                auto offsetWords = foundParam->localOffset;
                auto paramSize = typeSize(foundParam->typeInfo);

                auto localOffset = static_cast<int32_t>(resolvedDecl->dotData.lhs->localOffset);

                if (resolvedDecl->dotData.autoDeref && offsetWords > 0) {
                    append(instructions, Instruction::ADDI32);

                    append(instructions, Instruction::I32);
                    append(instructions, toBytes(resolvedDecl->dotData.autoDerefStorage->localOffset));

                    append(instructions, Instruction::CONSTI32);
                    append(instructions, toBytes(offsetWords));

                    append(instructions, toBytes(resolvedDecl->dotData.autoDerefStorage->localOffset));
                }

                append(node->bytecode, Instruction::STORE);

                if (resolvedDecl->dotData.autoDeref) {
                    append(node->bytecode, Instruction::I32);
                    append(node->bytecode, toBytes(static_cast<int32_t>(resolvedDecl->dotData.autoDerefStorage->localOffset)));
                } else {
                    append(node->bytecode, Instruction::CONSTI32);
                    append(node->bytecode, toBytes(static_cast<int32_t>(localOffset + offsetWords)));
                }

                append(node->bytecode, Instruction::CONSTI32);
                append(node->bytecode, toBytes(static_cast<int32_t>(data.rhs->localOffset)));

                append(node->bytecode, toBytes(static_cast<int32_t>(paramSize)));
            } else {
                assert(false);
            }
        } break;
        case NodeType::SYMBOL: {
            auto resolved = resolveNode(node);
            auto resolvedType = resolve(resolved->typeInfo);
            gen(resolved);

            auto localOffset = resolved->localOffset;
            switch (resolvedType->typeData.kind) {
                case NodeTypekind::FN: {
                    if (resolved->type == NodeType::FN_DECL) {
                        append(node->bytecode, Instruction::CONSTI32);
                        append(node->bytecode, toBytes(static_cast<int32_t>(resolved->fnDeclData.tableIndex)));
                    } else {
                        assert(resolved->type == NodeType::DECL || resolved->type == NodeType::DECL_PARAM);
                        append(node->bytecode, Instruction::I32);
                        append(node->bytecode, toBytes(static_cast<int32_t>(localOffset)));
                    }
                } break;
                case NodeTypekind::POINTER:
                case NodeTypekind::I32: {
                    append(node->bytecode, Instruction::I32);
                    append(node->bytecode, toBytes(static_cast<int32_t>(localOffset)));
                } break;
                case NodeTypekind::INT_LITERAL:
                case NodeTypekind::I64: {
                    append(node->bytecode, Instruction::I64);
                    append(node->bytecode, toBytes(static_cast<int32_t>(localOffset)));
                } break;
                case NodeTypekind::STRUCT: {
                    // todo(chad): anything?
                } break;
                default: assert(false);
            }

            node->localOffset = resolved->localOffset;
        } break;
        case NodeType::BINOP: {
            gen(node->binopData.lhs);
            gen(node->binopData.rhs);

            // store lhs into our first local
            storeValue(instructions, node->binopData.lhs, node->locals[0]->offset);

            // store rhs into our second local
            storeValue(instructions, node->binopData.rhs, node->locals[1]->offset);

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
            auto paramCount = node->fnCallData.params.size();
            int32_t totalParamsSize = 0;
            for (auto i = 0; i < paramCount; i++) {
                auto paramValue = node->fnCallData.params[i]->paramData.value;
                resolve(paramValue);
                totalParamsSize += typeSize(paramValue->typeInfo);
            }

            if (totalParamsSize > 0) {
                // push the params (in reverse order!)
                for (auto i = static_cast<int32_t>(paramCount - 1); i >= 0; i--) {
                    auto paramValue = node->fnCallData.params[i]->paramData.value;
                    gen(paramValue);
                }

                append(instructions, Instruction::BUMPSP);
                append(instructions, toBytes(totalParamsSize));

                auto paramAccum = 0;
                for (auto i = static_cast<int32_t>(paramCount - 1); i >= 0; i--) {
                    auto paramValue = node->fnCallData.params[i]->paramData.value;
                    auto paramSize = typeSize(paramValue->typeInfo);
                    gen(paramValue);
                    storeValue(instructions, resolve(paramValue), static_cast<int32_t>(currentFnStackSize + paramAccum));
                    paramAccum += paramSize;
                }
            }

            auto resolvedFn = resolve(node->fnCallData.fn);
            if (resolvedFn->type == NodeType::FN_DECL) {
                append(instructions, Instruction::CALL);
                fixups.insert({instructions.size(), resolvedFn});
                append(instructions, toBytes(static_cast<int32_t>(999)));
            } else if (resolvedFn->type == NodeType::DECL) {
                append(instructions, Instruction::CALLI);
                append(instructions, Instruction::I32);
                append(instructions, toBytes(static_cast<int32_t>(resolvedFn->localOffset)));
            } else if (resolvedFn->type == NodeType::DEREF) {
                gen(resolvedFn);
                storeValue(instructions, resolvedFn, node->fnCallData.fn->localOffset);

                append(instructions, Instruction::CALLI);
                append(instructions, Instruction::I32);
                append(instructions, toBytes(static_cast<int32_t>(node->fnCallData.fn->localOffset)));
            } else {
                gen(resolvedFn);
                append(instructions, Instruction::CALLI);
                append(instructions, resolvedFn->bytecode);
            }

            if (totalParamsSize > 0) {
                append(instructions, Instruction::BUMPSP);
                append(instructions, toBytes(-totalParamsSize));
            }

            copyBytes(instructions,
                      node->localOffset,
                      currentFnStackSize + totalParamsSize + 8, // + 8 for the saving of the return address and base pointer
                      typeSize(node->fnCallData.fn->typeInfo->typeData.fnTypeData.returnType));

            append(node->bytecode, Instruction::I32);
            append(node->bytecode, toBytes(static_cast<int32_t>(node->localOffset)));

            toProcess.push(resolvedFn);
        } break;
        case NodeType::DECL_PARAM: {
            append(node->bytecode, Instruction::I32);
            append(node->bytecode, toBytes(static_cast<int32_t>(node->localOffset)));
        } break;
        case NodeType::DEREF: {
            gen(node->nodeData);

            append(instructions, Instruction::STORE);
            append(instructions, Instruction::CONSTI32);
            append(instructions, toBytes(node->locals[0]->offset));
            append(instructions, Instruction::I32);
            append(instructions, toBytes(node->nodeData->localOffset));
            append(instructions, toBytes(typeSize(node->typeInfo)));

            node->localOffset = node->locals[0]->offset;
        } break;
        case NodeType::ADDRESS_OF: {
            gen(node->nodeData);

            auto resolved = resolve(node->nodeData);
            switch (resolved->type) {
                case NodeType::INT_LITERAL: {
                    storeValue(instructions, node->nodeData, node->locals[0]->offset);
                } break;
                default: assert(false);
            }

            append(instructions, Instruction::STORECONST);
            append(instructions, Instruction::CONSTI32);
            append(instructions, toBytes(node->locals[1]->offset));
            append(instructions, Instruction::CONSTI32);
            append(instructions, toBytes(node->locals[0]->offset));

            node->localOffset = node->locals[1]->offset;
        } break;
        case NodeType::TYPE: {
            // just a type declaration, nothing to do here!
        } break;
        case NodeType::DOT: {
            auto data = node->dotData;

            auto resolvedTypeInfo = resolve(resolve(data.lhs)->typeInfo);

            if (resolvedTypeInfo->typeData.kind == NodeTypekind::STRUCT) {
                // codegen the lhs
                gen(data.lhs);

                // find the offset in bytes of this param from the base of the struct
                auto structParams = resolvedTypeInfo->typeData.structTypeData.params;
                auto foundParam = data.resolved;
                auto offsetWords = foundParam->localOffset;

                node->localOffset = data.lhs->localOffset;
                node->localStorageOffset = offsetWords;
            }
            else if (resolvedTypeInfo->typeData.kind == NodeTypekind::POINTER) {
                gen(data.lhs);

                auto pointerCount = 0;
                while (resolvedTypeInfo->typeData.kind == NodeTypekind::POINTER) {
                    resolvedTypeInfo = resolve(resolvedTypeInfo->typeData.pointerTypeData.underlyingType);
                    pointerCount += 1;
                }
                assert(resolvedTypeInfo->typeData.kind == NodeTypekind::STRUCT);

                auto storedInDerefStorage = false;
                for (auto i = 0; i < pointerCount - 1; i++) {
                    storedInDerefStorage = true;

                    append(instructions, Instruction::STORE);

                    append(instructions, Instruction::CONSTI32);
                    append(instructions, toBytes(static_cast<int32_t>(data.autoDerefStorage->localOffset)));

                    append(instructions, Instruction::I32);
                    if (i == 0) {
                        append(instructions, toBytes(static_cast<int32_t>(data.lhs->localOffset)));
                    } else {
                        append(instructions, toBytes(static_cast<int32_t>(data.autoDerefStorage->localOffset)));
                    }

                    append(instructions, toBytes(static_cast<int32_t>(4)));
                }

                // find the offset in bytes of this param from the base of the struct
                auto structParams = resolvedTypeInfo->typeData.structTypeData.params;
                auto foundParam = data.resolved;
                auto offsetWords = foundParam->localOffset;

                if (storedInDerefStorage) {
                    node->localOffset = data.autoDerefStorage->localOffset;
                } else {
                    node->localOffset = data.lhs->localOffset;
                }

                node->localStorageOffset = offsetWords;
            }
            else {
                assert(false);
            }

            if (node->dotData.nodeLocalStorage != nullptr) {
                storeValue(instructions, node, node->dotData.nodeLocalStorage->localOffset);

                append(node->bytecode, Instruction::I32);
                append(node->bytecode, toBytes(node->dotData.nodeLocalStorage->localOffset));
            } else {
                append(node->bytecode, Instruction::I32);
                append(node->bytecode, toBytes(node->localOffset));
            }
        } break;
        default: assert(false);
    }

    if (node->sourceMapStatement) {
        sourceMap.statements.back().instEndIndex = instructions.size() + node->bytecode.size();
    }
}

void BytecodeGen::fixup() {
    // fixup statically known function instruction offsets
    for (auto fixup : fixups) {
        auto node = fixup.second;
        assert(node->type == NodeType::FN_DECL);

        auto instOffset = static_cast<int32_t>(node->fnDeclData.instOffset);
        memcpy(&instructions[fixup.first], &instOffset, sizeof(int32_t));
    }
}

void BytecodeGen::copyBytes(vector<unsigned char> &bc, int64_t to, int64_t from, int64_t size) {
    append(bc, Instruction::STORE);

    append(bc, Instruction::CONSTI32);
    append(bc, toBytes(static_cast<int32_t>(to)));

    append(bc, Instruction::CONSTI32);
    append(bc, toBytes(static_cast<int32_t>(from)));

    append(bc, toBytes(static_cast<int32_t>(size)));
}

void BytecodeGen::storeValue(vector<unsigned char> &instructions, Node *node, int32_t offset) {
    // if it's a constant, we need to do a STORE
    node = resolve(node);

    switch (node->type) {
        case NodeType::INT_LITERAL: {
            append(instructions, Instruction::STORECONST);

            append(instructions, Instruction::CONSTI32);
            append(instructions, toBytes(offset));

            append(instructions, node->bytecode);
        } break;
        case NodeType::FN_DECL: {
            append(instructions, Instruction::STORECONST);

            append(instructions, Instruction::CONSTI32);
            append(instructions, toBytes(offset));

            append(instructions, Instruction::CONSTI32);
            append(instructions, toBytes(static_cast<int32_t>(node->fnDeclData.tableIndex)));
        } break;
        case NodeType::ADDRESS_OF: {
            append(instructions, Instruction::STORECONST);

            append(instructions, Instruction::CONSTI32);
            append(instructions, toBytes(offset));

            append(instructions, Instruction::CONSTI32);
            append(instructions, toBytes(node->locals[0]->offset + node->nodeData->localStorageOffset));
        } break;
        case NodeType::DEREF: {
            append(instructions, Instruction::STORE);

            append(instructions, Instruction::CONSTI32);
            append(instructions, toBytes(offset));

            append(instructions, Instruction::I32);
            auto resolvedDeref = resolve(node->nodeData);
            append(instructions, toBytes(resolvedDeref->localOffset));

            assert(node->nodeData->typeInfo->typeData.kind == NodeTypekind::POINTER);
            append(instructions, toBytes(static_cast<int32_t>(typeSize(node->nodeData->typeInfo->typeData.pointerTypeData.underlyingType))));
        } break;
        case NodeType::BINOP: {
            copyBytes(instructions, offset, node->locals[2]->offset, typeSize(node->typeInfo));
        } break;
        case NodeType::DECL: {
            copyBytes(instructions, offset, node->locals[0]->offset, typeSize(node->typeInfo));
        } break;
        case NodeType::DECL_PARAM:
        case NodeType::FN_CALL: {
            copyBytes(instructions, offset, node->localOffset, typeSize(node->typeInfo));
        } break;
        case NodeType::DOT: {
            auto foundParam = node->dotData.resolved;
            auto offsetWords = foundParam->localOffset;
            auto paramSize = typeSize(foundParam->typeInfo);

            if (node->dotData.lhs->type == NodeType::DEREF) {
                append(instructions, Instruction::STORE);

                append(instructions, Instruction::I32);
                append(instructions, toBytes(static_cast<int32_t>(offset)));

                append(instructions, Instruction::I32);
                append(instructions, toBytes(static_cast<int32_t>(node->dotData.lhs->localOffset + offsetWords)));

                append(instructions, toBytes(static_cast<int32_t>(paramSize)));
            } else {
                if (node->dotData.autoDeref && offsetWords > 0) {
                    append(instructions, Instruction::ADDI32);

                    append(instructions, Instruction::I32);
                    append(instructions, toBytes(node->localOffset));

                    append(instructions, Instruction::CONSTI32);
                    append(instructions, toBytes(offsetWords));

                    append(instructions, toBytes(node->localOffset));
                }

                append(instructions, Instruction::STORE);

                append(instructions, Instruction::CONSTI32);
                append(instructions, toBytes(static_cast<int32_t>(offset)));

                if (node->dotData.autoDeref) {
                    append(instructions, Instruction::I32);
                    append(instructions, toBytes(node->localOffset));
                } else {
                    append(instructions, Instruction::CONSTI32);
                    append(instructions, toBytes(static_cast<int32_t>(node->dotData.lhs->localOffset + offsetWords)));
                }

                append(instructions, toBytes(static_cast<int32_t>(paramSize)));
            }
        } break;
        default:
            assert(false);
    }
}

bool isConstant(Node *node) {
//    return node->isLocal;

    switch (resolve(node)->type) {
        case NodeType::INT_LITERAL:
        case NodeType::ADDRESS_OF:
        case NodeType::DEREF:
        case NodeType::FN_DECL:
            return true;
        default: return false;
    }
}
