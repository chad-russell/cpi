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
            bytecodeStr = "RELI32";
        } break;
        case NodeTypekind::I64: {
            instructionStr.append("I64");
            bytecodeStr = "RELI64";
        } break;
        default: assert(false);
    }

    auto inst = AssemblyLexer::nameToInstruction[instructionStr];
    append(instructions, inst);

    auto bytecodeInst = AssemblyLexer::nameToInstruction[bytecodeStr];
    append(node->bytecode, bytecodeInst);

    append(instructions, Instruction::RELI32);
    append(instructions, toBytes(node->binopData.lhsTemporary->localOffset));

    append(instructions, Instruction::RELI32);
    append(instructions, toBytes(node->binopData.rhsTemporary->localOffset));

    append(instructions, toBytes(static_cast<int32_t>(node->localOffset)));
    append(node->bytecode, toBytes(static_cast<int32_t>(node->localOffset)));
}

void BytecodeGen::gen(Node *node) {
    // avoids generating code for a fn decl within another fn decl
    // todo(chad): make sure this is the right way to do this
    if (node->type == NodeType::FN_DECL && !processFnDecls) {
        if (node->fnDeclData.isLiteral && node->bytecode.empty()) {
            // placeholder
            append(node->bytecode, Instruction::RELCONSTI32);
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

            gen(node->retData.value);

            storeValue(node->bytecode, node->retData.value, 0);

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

            auto resolvedInitialValue = resolveNode(data.initialValue);
            gen(resolvedInitialValue);

            auto localOffset = node->localOffset;

            storeValue(node->bytecode, data.initialValue, localOffset);
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

                auto localOffset = static_cast<int32_t>(resolve(resolvedDecl->derefData.target)->localOffset);

                append(node->bytecode, Instruction::STORE);

                append(node->bytecode, Instruction::RELI32);
                append(node->bytecode, toBytes(localOffset));

                append(node->bytecode, Instruction::RELCONSTI32);
                append(node->bytecode, toBytes(data.rhs->localOffset));

                assert(resolvedDecl->derefData.target->typeInfo->typeData.kind == NodeTypekind::POINTER);
                append(node->bytecode, toBytes(static_cast<int32_t>(typeSize(resolvedDecl->derefData.target->typeInfo->typeData.pointerTypeData.underlyingType))));
            } else if (resolvedDecl->type == NodeType::DOT) {
                gen(data.lhs);

                // store the rvalue into it's slot
                storeValue(node->bytecode, data.rhs, data.rhs->localOffset);

                auto foundParam = resolvedDecl->dotData.resolved;
                auto offsetWords = foundParam->localOffset;
                auto paramSize = typeSize(foundParam->typeInfo);

                auto localOffset = static_cast<int32_t>(resolvedDecl->dotData.lhs->localOffset);

                if (resolvedDecl->dotData.autoDeref) {
                    append(instructions, Instruction::ADDI32);

                    append(instructions, Instruction::RELI32);
                    append(instructions, toBytes(resolvedDecl->localOffset));

                    append(instructions, Instruction::RELCONSTI32);
                    append(instructions, toBytes(offsetWords));

                    append(instructions, toBytes(resolvedDecl->dotData.autoDerefStorage->localOffset));
                }

                append(node->bytecode, Instruction::STORE);

                if (resolvedDecl->dotData.autoDeref) {
                    append(node->bytecode, Instruction::RELI32);
                    append(node->bytecode, toBytes(static_cast<int32_t>(resolvedDecl->dotData.autoDerefStorage->localOffset)));
                } else {
                    append(node->bytecode, Instruction::RELCONSTI32);
                    append(node->bytecode, toBytes(static_cast<int32_t>(localOffset + offsetWords)));
                }

                append(node->bytecode, Instruction::RELCONSTI32);
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
                        append(node->bytecode, Instruction::RELCONSTI32);
                        append(node->bytecode, toBytes(static_cast<int32_t>(resolved->fnDeclData.tableIndex)));
                    } else {
                        assert(resolved->type == NodeType::DECL || resolved->type == NodeType::DECL_PARAM);
                        append(node->bytecode, Instruction::RELI32);
                        append(node->bytecode, toBytes(static_cast<int32_t>(localOffset)));
                    }
                } break;
                case NodeTypekind::POINTER:
                case NodeTypekind::I32: {
                    append(node->bytecode, Instruction::RELI32);
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

            storeValue(instructions, node->binopData.lhs, node->binopData.lhsTemporary->localOffset);
            storeValue(instructions, node->binopData.rhs, node->binopData.rhsTemporary->localOffset);

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
                auto paramAccum = 0;
                for (auto i = static_cast<int32_t>(paramCount - 1); i >= 0; i--) {
                    auto paramValue = node->fnCallData.params[i]->paramData.value;
                    auto paramSize = typeSize(paramValue->typeInfo);
                    gen(paramValue);
                    storeValue(instructions, resolve(paramValue), static_cast<int32_t>(currentFnStackSize + paramAccum));
                    paramAccum += paramSize;
                }

                append(instructions, Instruction::BUMPSP);
                append(instructions, toBytes(totalParamsSize));
            }

            auto resolvedFn = resolve(node->fnCallData.fn);
            if (resolvedFn->type == NodeType::FN_DECL) {
                append(instructions, Instruction::CALL);
                fixups.insert({instructions.size(), resolvedFn});
                append(instructions, toBytes(static_cast<int32_t>(999)));
            } else if (resolvedFn->type == NodeType::DECL) {
                append(instructions, Instruction::CALLI);
                append(instructions, Instruction::RELI32);
                append(instructions, toBytes(static_cast<int32_t>(resolvedFn->localOffset)));
            } else if (resolvedFn->type == NodeType::DEREF) {
                gen(resolvedFn);

                storeValue(instructions, resolvedFn, node->fnCallData.fn->localOffset);

                append(instructions, Instruction::CALLI);
                append(instructions, Instruction::RELI32);
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

            // copy bytes
            append(instructions, Instruction::STORE);
            append(instructions, Instruction::RELCONSTI32);
            append(instructions, toBytes(static_cast<int32_t>(node->localOffset)));
            append(instructions, Instruction::RELCONSTI32);
            append(instructions, toBytes(static_cast<int32_t>(currentFnStackSize + totalParamsSize + 8)));
            append(instructions, toBytes(static_cast<int32_t>(typeSize(node->fnCallData.fn->typeInfo->typeData.fnTypeData.returnType))));

            append(node->bytecode, Instruction::RELI32);
            append(node->bytecode, toBytes(static_cast<int32_t>(node->localOffset)));

            toProcess.push(resolvedFn);
        } break;
        case NodeType::DECL_PARAM: {
            append(node->bytecode, Instruction::RELI32);
            append(node->bytecode, toBytes(static_cast<int32_t>(node->localOffset)));
        } break;
        case NodeType::DEREF: {
            gen(node->derefData.target);

            if (isConstant(node->derefData.target)) {
                storeValue(instructions, node->derefData.target, node->derefData.target->localOffset);
            }

            if (node->derefData.isRvalue) {
                append(instructions, Instruction::STORE);
                append(instructions, Instruction::RELCONSTI32);
                append(instructions, toBytes(node->localOffset));

                append(instructions, Instruction::RELI32);
                append(instructions, toBytes(node->derefData.target->localOffset));

                assert(node->derefData.target->typeInfo->typeData.kind == NodeTypekind::POINTER);
                append(instructions, toBytes(typeSize(node->derefData.target->typeInfo->typeData.pointerTypeData.underlyingType)));
            }
        } break;
        case NodeType::ADDRESS_OF: {
            gen(node->nodeData);

            if (isConstant(node->nodeData)) {
                auto constOffset = static_cast<int32_t>(node->nodeData->localOffset);
                storeValue(instructions, node->nodeData, constOffset);
            }

            append(node->bytecode, Instruction::RELCONSTI32);
            append(node->bytecode, toBytes(static_cast<int32_t>(node->nodeData->localOffset)));
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
}

void BytecodeGen::storeValue(vector<unsigned char> &instructions, Node *node, int32_t offset) {
    node = resolve(node);

    switch (node->type) {
        case NodeType::INT_LITERAL: {
            append(instructions, Instruction::STORECONST);

            append(instructions, Instruction::RELCONSTI32);
            append(instructions, toBytes(offset));

            append(instructions, node->bytecode);
        } break;
        case NodeType::FN_DECL: {
            append(instructions, Instruction::STORECONST);

            append(instructions, Instruction::RELCONSTI32);
            append(instructions, toBytes(offset));

            append(instructions, Instruction::CONSTI32);
            append(instructions, toBytes(static_cast<int32_t>(node->fnDeclData.tableIndex)));
        } break;
        case NodeType::ADDRESS_OF: {
            append(instructions, Instruction::STORECONST);

            append(instructions, Instruction::RELCONSTI32);
            append(instructions, toBytes(offset));

            append(instructions, Instruction::RELCONSTI32);
            append(instructions, toBytes(node->nodeData->localOffset + node->nodeData->localStorageOffset));
        } break;
        case NodeType::DEREF: {
            append(instructions, Instruction::STORE);

            append(instructions, Instruction::RELCONSTI32);
            append(instructions, toBytes(offset));

            append(instructions, Instruction::RELI32);
            auto resolvedDeref = resolve(node->derefData.target);
            append(instructions, toBytes(resolvedDeref->localOffset));

            append(instructions, toBytes(static_cast<int32_t>(typeSize(node->derefData.target->typeInfo))));
        } break;
        case NodeType::DECL_PARAM:
        case NodeType::FN_CALL:
        case NodeType::BINOP:
        case NodeType::DECL: {
            append(instructions, Instruction::STORE);

            append(instructions, Instruction::RELCONSTI32);
            append(instructions, toBytes(offset));

            append(instructions, Instruction::RELCONSTI32);
            append(instructions, toBytes(node->localOffset));

            append(instructions, toBytes(typeSize(node->typeInfo)));
        } break;
        case NodeType::DOT: {
            auto foundParam = node->dotData.resolved;
            auto offsetWords = foundParam->localOffset;
            auto paramSize = typeSize(foundParam->typeInfo);

            if (node->dotData.lhs->type == NodeType::DEREF) {
                append(instructions, Instruction::STORE);

                append(instructions, Instruction::RELI32);
                append(instructions, toBytes(offset));

                append(instructions, Instruction::RELCONSTI32);
                append(instructions, toBytes(static_cast<int32_t>(node->dotData.lhs->localOffset + offsetWords)));

                append(instructions, toBytes(static_cast<int32_t>(paramSize)));
            } else {
                if (node->dotData.autoDeref) {
                    append(instructions, Instruction::ADDI32);

                    append(instructions, Instruction::RELI32);
                    append(instructions, toBytes(node->localOffset));

                    append(instructions, Instruction::RELCONSTI32);
                    append(instructions, toBytes(offsetWords));

                    append(instructions, toBytes(node->localOffset));
                }

                append(instructions, Instruction::STORE);

                append(instructions, Instruction::RELCONSTI32);
                append(instructions, toBytes(static_cast<int32_t>(offset)));

                if (node->dotData.autoDeref) {
                    append(instructions, Instruction::RELI32);
                    append(instructions, toBytes(node->localOffset));
                } else {
                    append(instructions, Instruction::RELCONSTI32);
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
    switch (resolve(node)->type) {
        case NodeType::INT_LITERAL:
        case NodeType::ADDRESS_OF:
        case NodeType::DEREF:
        case NodeType::FN_DECL:
            return true;
        default: return false;
    }
}
