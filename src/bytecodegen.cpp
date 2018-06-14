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

void BytecodeGen::binopHelper(string instructionStr, Node *node, int32_t scale) {
    string bytecodeStr;

    auto resolvedLhsType = resolve(node->binopData.lhs->typeInfo);
    auto resolvedRhsType = resolve(node->binopData.lhs->typeInfo);

    auto isBoolean = false;

    NodeTypekind kind;
    if (resolvedLhsType->typeData.kind == NodeTypekind::POINTER || resolvedRhsType->typeData.kind == NodeTypekind::POINTER) {
        // pointer arithmetic
        kind = NodeTypekind::POINTER;
    } else if (startsWith(&instructionStr, "EQ") || startsWith(&instructionStr, "NEQ")
            || startsWith(&instructionStr, "GT") || startsWith(&instructionStr, "GE")
            || startsWith(&instructionStr, "LT") || startsWith(&instructionStr, "LE")
            || startsWith(&instructionStr, "GT") || startsWith(&instructionStr, "GE")
            || startsWith(&instructionStr, "UGT") || startsWith(&instructionStr, "UGE")
            || startsWith(&instructionStr, "ULT") || startsWith(&instructionStr, "ULE")
            || startsWith(&instructionStr, "SGT") || startsWith(&instructionStr, "SGE")
            || startsWith(&instructionStr, "SLT") || startsWith(&instructionStr, "SLE")) {
        // comparison, so boolean
        isBoolean = true;
        kind = resolvedLhsType->typeData.kind;
    }
    else {
        kind = resolvedLhsType->typeData.kind;
    }

    string toAppend;
    switch (kind) {
        case NodeTypekind::I8: {
            toAppend = "I8";
            bytecodeStr = "RELI8";
        } break;
        case NodeTypekind::BOOLEAN:
        case NodeTypekind::BOOLEAN_LITERAL:
        case NodeTypekind::I32: {
            toAppend = "I32";
            bytecodeStr = "RELI32";
        } break;
        case NodeTypekind::POINTER:
        case NodeTypekind::I64: {
            toAppend = "I64";
            bytecodeStr = "RELI64";
        } break;
        case NodeTypekind::F32: {
            toAppend = "F32";
            bytecodeStr = "RELF32";
        } break;
        case NodeTypekind::F64: {
            toAppend = "F64";
            bytecodeStr = "RELF64";
        } break;
        default:
            assert(false);
    }

    if (isBoolean) {
        bytecodeStr = "RELI32";
    }

    instructionStr.append(toAppend);

    auto inst = AssemblyLexer::nameToInstruction[instructionStr];
    append(instructions, inst);

    auto bytecodeInst = AssemblyLexer::nameToInstruction[bytecodeStr];
    append(node->bytecode, bytecodeInst);

    append(instructions, bytecodeInst);
    append(instructions, toBytes(node->binopData.lhsTemporary->localOffset));

    append(instructions, bytecodeInst);
    append(instructions, toBytes(node->binopData.rhsTemporary->localOffset));

    if (scale > 1) {
        append(instructions, toBytes32(scale));
    }

    append(instructions, toBytes32(node->localOffset));
    append(node->bytecode, toBytes32(node->localOffset));
}

void BytecodeGen::gen(Node *node) {
    // avoids generating code for a fn decl within another fn decl
    if (node->type == NodeType::FN_DECL && !processFnDecls) {
        if (node->fnDeclData.isLiteral && node->bytecode.empty()) {
            // placeholder
            append(node->bytecode, Instruction::RELCONSTI32);
            append(node->bytecode, toBytes32(node->fnDeclData.tableIndex));
        }

        toProcess.push(node);
        return;
    }

    if (node->gen) { return; }
    node->gen = true;

    for (auto stmt : node->preStmts) {
        gen(stmt);
    }

    if (node->skipAllButPostStmts) {
        for (auto stmt : node->postStmts) {
            gen(stmt);
        }

        return;
    }

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

            auto didTerminate = false;
            for (auto stmt : node->fnDeclData.body) {
                // todo(chad): @Hack? Basically why would we gen a fn or type decl if it's not being called...
                if (stmt->type != NodeType::FN_DECL && stmt->type != NodeType::TYPE) {
                    gen(stmt);
                }
                if (stmt->type == NodeType::RET || stmt->type == NodeType::PANIC) { didTerminate = true; }
            }
            if (!didTerminate) {
                if (isMainFn) {
                    append(instructions, Instruction::EXIT);
                } else {
                    append(instructions, Instruction::RET);
                }
            }

            currentFnStackSize = savedCurrentFnStackSize;
        } break;
        case NodeType::RET: {
            // todo(chad): don't do any storing if this is just 'ret' with no value

            gen(node->retData.value);

            storeValue(node->retData.value, 0);

            if (isMainFn) {
                append(instructions, Instruction::EXIT);
            } else {
                append(instructions, Instruction::RET);
            }
        } break;
        case NodeType::INT_LITERAL: {
            switch (node->typeInfo->typeData.kind) {
                case NodeTypekind::I32: {
                    append(node->bytecode, Instruction::CONSTI32);
                    auto litData = static_cast<int32_t>(node->intLiteralData.value);
                    append(node->bytecode, toBytes(litData));
                }
                    break;
                case NodeTypekind::I8: {
                    append(node->bytecode, Instruction::CONSTI8);
                    auto litData = static_cast<int8_t>(node->intLiteralData.value);
                    append(node->bytecode, toBytes(litData));
                }
                    break;
                case NodeTypekind::INT_LITERAL:
                case NodeTypekind::I64: {
                    append(node->bytecode, Instruction::CONSTI64);
                    auto litData = node->intLiteralData.value;
                    append(node->bytecode, toBytes(litData));
                }
                    break;
                default:
                    assert(false);
            }
        } break;
        case NodeType::SIZEOF: {
            append(node->bytecode, Instruction::CONSTI64);
            auto litData = static_cast<int64_t>(typeSize(node->nodeData));
            append(node->bytecode, toBytes(litData));
        } break;
        case NodeType::NIL_LITERAL: {
            append(node->bytecode, Instruction::CONSTI64);
            append(node->bytecode, toBytes64(0));
        } break;
        case NodeType::FLOAT_LITERAL: {
            switch (node->typeInfo->typeData.kind) {
                case NodeTypekind::FLOAT_LITERAL:
                case NodeTypekind::F32: {
                    append(node->bytecode, Instruction::CONSTF32);
                    auto litData = static_cast<float>(node->floatLiteralData.value);
                    append(node->bytecode, toBytes(litData));
                }
                    break;
                case NodeTypekind::F64: {
                    append(node->bytecode, Instruction::CONSTF64);
                    auto litData = node->floatLiteralData.value;
                    append(node->bytecode, toBytes(litData));
                }
                    break;
                default:
                    assert(false);
            }
        }
            break;
        case NodeType::BOOLEAN_LITERAL: {
            append(node->bytecode, Instruction::CONSTI32);
            auto litData = static_cast<int32_t>(node->boolLiteralData.value ? 1 : 0);
            append(node->bytecode, toBytes(litData));
        } break;
        case NodeType::STRUCT_LITERAL: {
            // nothing to do here! wait until we actually need to store it somewhere
        } break;
        case NodeType::DECL: {
            auto data = node->declData;

            auto localOffset = node->localOffset;

            if (data.initialValue != nullptr) {
                gen(data.initialValue);
                auto resolvedInitialValue = resolve(data.initialValue);
                gen(resolvedInitialValue);
                storeValue(resolvedInitialValue, localOffset);
            }
        } break;
        case NodeType::ASSIGN: {
            auto data = node->assignData;

            gen(data.rhs);

            auto resolvedDecl = resolve(data.lhs);
            if (resolvedDecl->type == NodeType::DECL) {
                auto localOffset = static_cast<int32_t>(resolvedDecl->localOffset);
                storeValue(data.rhs, localOffset);
            } else if (resolvedDecl->type == NodeType::DEREF) {
                gen(data.lhs);

                // store rvalue into it's slot
                storeValue(data.rhs, data.rhs->localOffset);

                auto localOffset = static_cast<int32_t>(resolve(resolvedDecl->derefData.target)->localOffset);

                append(instructions, Instruction::STORE);

                append(instructions, Instruction::RELI32);
                append(instructions, toBytes(localOffset));

                append(instructions, Instruction::RELCONSTI32);
                append(instructions, toBytes(data.rhs->localOffset));

                assert(resolvedDecl->derefData.target->typeInfo->typeData.kind == NodeTypekind::POINTER);
                append(instructions, toBytes32(typeSize(resolvedDecl->typeInfo)));
            } else if (resolvedDecl->type == NodeType::DOT) {
                // store rvalue into it's slot
                storeValue(data.rhs, data.rhs->localOffset);

                gen(resolvedDecl);

                append(instructions, Instruction::STORE);

                if (resolvedDecl->dotData.pointerIsRelative) {
                    append(instructions, Instruction::RELI32);
                    append(instructions, toBytes(resolvedDecl->localOffset));
                } else {
                    append(instructions, Instruction::RELCONSTI32);
                    append(instructions, toBytes(resolvedDecl->localOffset));
                }

                append(instructions, Instruction::RELCONSTI32);
                append(instructions, toBytes32(data.rhs->localOffset));

                append(instructions, toBytes32(typeSize(resolvedDecl->typeInfo)));
            } else {
                assert(false);
            }
        } break;
        case NodeType::SYMBOL: {
            auto resolved = resolve(node);
            auto resolvedType = resolve(resolved->typeInfo);
            gen(resolved);

            auto localOffset = resolved->localOffset;
            switch (resolvedType->typeData.kind) {
                case NodeTypekind::FN: {
                    if (resolved->type == NodeType::FN_DECL) {
                        append(node->bytecode, Instruction::RELCONSTI32);
                        append(node->bytecode, toBytes32(resolved->fnDeclData.tableIndex));
                    } else {
                        assert(resolved->type == NodeType::DECL || resolved->type == NodeType::DECL_PARAM);
                        append(node->bytecode, Instruction::RELI32);
                        append(node->bytecode, toBytes32(localOffset));
                    }
                } break;
                case NodeTypekind::I8: {
                    append(node->bytecode, Instruction::RELI8);
                    append(node->bytecode, toBytes32(localOffset));
                } break;
                case NodeTypekind::BOOLEAN_LITERAL:
                case NodeTypekind::BOOLEAN:
                case NodeTypekind::I32: {
                    append(node->bytecode, Instruction::RELI32);
                    append(node->bytecode, toBytes32(localOffset));
                } break;
                case NodeTypekind::POINTER:
                case NodeTypekind::INT_LITERAL:
                case NodeTypekind::I64: {
                    append(node->bytecode, Instruction::RELI64);
                    append(node->bytecode, toBytes32(localOffset));
                } break;
                case NodeTypekind::FLOAT_LITERAL:
                case NodeTypekind::F32: {
                    append(node->bytecode, Instruction::RELF32);
                    append(node->bytecode, toBytes32(localOffset));
                } break;
                case NodeTypekind::F64: {
                    append(node->bytecode, Instruction::RELF64);
                    append(node->bytecode, toBytes32(localOffset));
                } break;
                case NodeTypekind::NONE:
                case NodeTypekind::STRUCT: {
                    // nothing to do
                } break;
                default:
                    assert(false);
            }

            node->localOffset = resolved->localOffset;
        } break;
        case NodeType::BINOP: {
            if (node->binopData.type == LexerTokenType::AND) {
                // a and b ====> { result := false; if a { if b { result = true; } }

                append(node->bytecode, Instruction::RELI32);
                append(node->bytecode, toBytes(node->localOffset));

                // initially store false
                append(instructions, Instruction::STORECONST);
                append(instructions, Instruction::RELCONSTI32);
                append(instructions, toBytes(node->localOffset));
                append(instructions, Instruction::CONSTI32);
                append(instructions, toBytes32((int32_t) 0));

                gen(node->binopData.lhs);

                append(instructions, Instruction::JUMPIF);
                append(instructions, node->binopData.lhs->bytecode);

                append(instructions, Instruction::CONSTI32);
                auto trueBranchOverwrite = instructions.size();
                append(instructions, toBytes32(888));

                append(instructions, Instruction::CONSTI32);
                auto falseBranchOverwrite = instructions.size();
                append(instructions, toBytes32(999));

                auto instSize = static_cast<int32_t>(instructions.size());
                memcpy(&instructions[trueBranchOverwrite], &instSize, sizeof(int32_t));

                {
                    // second 'if'
                    gen(node->binopData.rhs);

                    append(instructions, Instruction::JUMPIF);
                    append(instructions, node->binopData.rhs->bytecode);

                    append(instructions, Instruction::CONSTI32);
                    auto trueBranchOverwrite2 = instructions.size();
                    append(instructions, toBytes32(888));

                    append(instructions, Instruction::CONSTI32);
                    auto falseBranchOverwrite2 = instructions.size();
                    append(instructions, toBytes32(999));

                    instSize = static_cast<int32_t>(instructions.size());
                    memcpy(&instructions[trueBranchOverwrite2], &instSize, sizeof(int32_t));

                    // set to true
                    append(instructions, Instruction::STORECONST);
                    append(instructions, Instruction::RELCONSTI32);
                    append(instructions, toBytes(node->localOffset));
                    append(instructions, Instruction::CONSTI32);
                    append(instructions, toBytes32((int32_t) 1));

                    instSize = static_cast<int32_t>(instructions.size());
                    memcpy(&instructions[falseBranchOverwrite2], &instSize, sizeof(int32_t));
                }

                unsigned long skipElseBranchOverwrite;
                append(instructions, Instruction::JUMP);
                skipElseBranchOverwrite = instructions.size();
                append(instructions, toBytes32(999));

                instSize = static_cast<int32_t>(instructions.size());
                memcpy(&instructions[falseBranchOverwrite], &instSize, sizeof(int32_t));

                instSize = static_cast<int32_t>(instructions.size());
                memcpy(&instructions[skipElseBranchOverwrite], &instSize, sizeof(int32_t));
            } else if (node->binopData.type == LexerTokenType::OR) {
                // a or b ====> { result := false; if a { result = true; } else if b { result = true; } }

                append(node->bytecode, Instruction::RELI32);
                append(node->bytecode, toBytes(node->localOffset));

                // initially store true
                append(instructions, Instruction::STORECONST);
                append(instructions, Instruction::RELCONSTI32);
                append(instructions, toBytes(node->localOffset));
                append(instructions, Instruction::CONSTI32);
                append(instructions, toBytes32((int32_t) 0));

                gen(node->binopData.lhs);

                append(instructions, Instruction::JUMPIF);
                append(instructions, node->binopData.lhs->bytecode);

                append(instructions, Instruction::CONSTI32);
                auto trueBranchOverwrite = instructions.size();
                append(instructions, toBytes32(888));

                append(instructions, Instruction::CONSTI32);
                auto falseBranchOverwrite = instructions.size();
                append(instructions, toBytes32(999));

                auto instSize = static_cast<int32_t>(instructions.size());
                memcpy(&instructions[trueBranchOverwrite], &instSize, sizeof(int32_t));

                // store true
                append(instructions, Instruction::STORECONST);
                append(instructions, Instruction::RELCONSTI32);
                append(instructions, toBytes(node->localOffset));
                append(instructions, Instruction::CONSTI32);
                append(instructions, toBytes32((int32_t) 1));

                unsigned long skipElseBranchOverwrite = 0;
                append(instructions, Instruction::JUMP);
                skipElseBranchOverwrite = instructions.size();
                append(instructions, toBytes32(999));

                instSize = static_cast<int32_t>(instructions.size());
                memcpy(&instructions[falseBranchOverwrite], &instSize, sizeof(int32_t));

                {
                    // else stmts
                    gen(node->binopData.rhs);

                    append(instructions, Instruction::JUMPIF);
                    append(instructions, node->binopData.rhs->bytecode);

                    append(instructions, Instruction::CONSTI32);
                    trueBranchOverwrite = instructions.size();
                    append(instructions, toBytes32(888));

                    append(instructions, Instruction::CONSTI32);
                    falseBranchOverwrite = instructions.size();
                    append(instructions, toBytes32(999));

                    instSize = static_cast<int32_t>(instructions.size());
                    memcpy(&instructions[trueBranchOverwrite], &instSize, sizeof(int32_t));

                    // store true
                    append(instructions, Instruction::STORECONST);
                    append(instructions, Instruction::RELCONSTI32);
                    append(instructions, toBytes(node->localOffset));
                    append(instructions, Instruction::CONSTI32);
                    append(instructions, toBytes32((int32_t) 1));

                    instSize = static_cast<int32_t>(instructions.size());
                    memcpy(&instructions[falseBranchOverwrite], &instSize, sizeof(int32_t));
                }

                instSize = static_cast<int32_t>(instructions.size());
                memcpy(&instructions[skipElseBranchOverwrite], &instSize, sizeof(int32_t));
            } else {
                gen(node->binopData.lhs);
                gen(node->binopData.rhs);

                storeValue(node->binopData.lhs, node->binopData.lhsTemporary->localOffset);
                storeValue(node->binopData.rhs, node->binopData.rhsTemporary->localOffset);

                auto scale = node->binopData.rhsScale;

                switch (node->binopData.type) {
                    case LexerTokenType::ADD: {
                        if (scale > 1) {
                            binopHelper("ADD_S_", node, scale);
                        } else {
                            binopHelper("ADD", node);
                        }
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
                    case LexerTokenType::EQ_EQ: {
                        binopHelper("EQ", node);
                    } break;
                    case LexerTokenType::NE: {
                        binopHelper("NEQ", node);
                    } break;
                    case LexerTokenType::LT: {
                        binopHelper("SLT", node);
                    } break;
                    case LexerTokenType::GT: {
                        binopHelper("SGT", node);
                    } break;
                    case LexerTokenType::GE: {
                        binopHelper("SGE", node);
                    } break;
                    default:
                        assert(false);
                }
            }
        } break;
        case NodeType::FN_CALL: {
            if (!node->fnCallData.hasRuntimeParams) { break; }

            auto paramCount = node->fnCallData.params.size();
            int32_t totalParamsSize = 0;
            for (auto i = 0; i < paramCount; i++) {
                auto paramValue = node->fnCallData.params[i]->valueParamData.value;
                resolve(paramValue);
                totalParamsSize += typeSize(paramValue->typeInfo);
            }

            if (totalParamsSize > 0) {
                // push the params (in reverse order!)
                auto paramAccum = 0;
                for (auto i = static_cast<int32_t>(paramCount - 1); i >= 0; i--) {
                    auto paramValue = node->fnCallData.params[i]->valueParamData.value;
                    gen(paramValue);
                }

                for (auto i = static_cast<int32_t>(paramCount - 1); i >= 0; i--) {
                    auto paramValue = node->fnCallData.params[i]->valueParamData.value;
                    auto paramSize = typeSize(paramValue->typeInfo);
                    storeValue(resolve(paramValue), static_cast<int32_t>(currentFnStackSize + paramAccum));
                    paramAccum += paramSize;
                }

                append(instructions, Instruction::BUMPSP);
                append(instructions, toBytes(totalParamsSize));
            }

            auto resolvedFn = resolve(node->fnCallData.fn);
            if (resolvedFn->type == NodeType::FN_DECL) {
                append(instructions, Instruction::CALL);
                fixups.insert({instructions.size(), resolvedFn});
                append(instructions, toBytes32(999));
            } else if (resolvedFn->type == NodeType::DECL) {
                append(instructions, Instruction::CALLI);
                append(instructions, Instruction::RELI32);
                append(instructions, toBytes32(resolvedFn->localOffset));
            } else if (resolvedFn->type == NodeType::DEREF) {
                gen(resolvedFn);

                storeValue(resolvedFn, node->fnCallData.fn->localOffset);

                append(instructions, Instruction::CALLI);
                append(instructions, Instruction::RELI32);
                append(instructions, toBytes32(node->fnCallData.fn->localOffset));
            } else if (resolvedFn->type == NodeType::DOT) {
                gen(resolvedFn);

                storeValue(resolvedFn, node->fnCallData.fn->localOffset);

                assert(resolvedFn->isLocal || resolvedFn->isBytecodeLocal);
                append(instructions, Instruction::CALLI);
                append(instructions, Instruction::RELI32);
                append(instructions, toBytes(resolvedFn->localOffset));
            } else {
                gen(resolvedFn);

                storeValue(resolvedFn, node->fnCallData.fn->localOffset);

                append(instructions, Instruction::CALLI);
                append(instructions, resolvedFn->bytecode);
            }

            if (totalParamsSize > 0) {
                append(instructions, Instruction::BUMPSP);
                append(instructions, toBytes(-totalParamsSize));
            }

            // copy bytes
            auto returnTypeSize = typeSize(resolvedFn->typeInfo->typeData.fnTypeData.returnType);
            if (returnTypeSize > 0) {
                append(instructions, Instruction::STORE);
                append(instructions, Instruction::RELCONSTI32);
                append(instructions, toBytes32(node->localOffset));
                append(instructions, Instruction::RELCONSTI32);
                append(instructions, toBytes32(currentFnStackSize + totalParamsSize + 8));
                append(instructions, toBytes32(returnTypeSize));
            }

            append(node->bytecode, Instruction::RELI32);
            append(node->bytecode, toBytes32(node->localOffset));

            toProcess.push(resolvedFn);
        } break;
        case NodeType::DECL_PARAM: {
            append(node->bytecode, Instruction::RELI32);
            append(node->bytecode, toBytes32(node->localOffset));
        } break;
        case NodeType::DEREF: {
            gen(node->derefData.target);

            storeValue(node->derefData.target, node->derefData.target->localOffset);

            if (node->isLocal || node->isBytecodeLocal) {
                append(instructions, Instruction::STORE);
                append(instructions, Instruction::RELCONSTI32);
                append(instructions, toBytes(node->localOffset));
                append(instructions, Instruction::RELI32);
                append(instructions, toBytes(node->derefData.target->localOffset));
                append(instructions, toBytes(typeSize(node->typeInfo)));
            }
        } break;
        case NodeType::ADDRESS_OF: {
            gen(node->nodeData);

            if (hasNoLocalByDefault(node->nodeData)) {
                auto constOffset = node->nodeData->localOffset;
                storeValue(node->nodeData, constOffset);
            }

            append(node->bytecode, Instruction::RELCONSTI32);
            append(node->bytecode, toBytes(node->nodeData->localOffset));

            if (node->isLocal || node->isBytecodeLocal) {
                append(instructions, Instruction::STORECONST);
                append(instructions, Instruction::RELCONSTI32);
                append(instructions, toBytes(node->localOffset));
                append(instructions, Instruction::RELCONSTI32);
                append(instructions, toBytes(node->nodeData->localOffset));
            }
        } break;
        case NodeType::TYPE: {
            // just a type declaration, nothing to do here!
        } break;
        case NodeType::DOT: {
            auto foundParam = node->dotData.resolved;
            auto offsetWords = foundParam->localOffset;

            gen(node->dotData.lhs);
            if (hasNoLocalByDefault(node->dotData.lhs)) {
                storeValue(node->dotData.lhs, node->dotData.lhs->localOffset);
            }

            auto resolvedTypeInfo = resolve(resolve(node->dotData.lhs)->typeInfo);

            auto pointerCount = 0;

            while (resolvedTypeInfo->typeData.kind == NodeTypekind::POINTER) {
                resolvedTypeInfo = resolve(resolvedTypeInfo->typeData.pointerTypeData.underlyingType);
                pointerCount += 1;
            }
            for (auto i = 0; i < pointerCount - 1; i++) {
                append(instructions, Instruction::STORE);

                append(instructions, Instruction::RELCONSTI32);
                append(instructions, toBytes(node->dotData.autoDerefStorage->localOffset));

                append(instructions, Instruction::RELI32);
                if (i == 0) {
                    append(instructions, toBytes32(node->dotData.lhs->localOffset));
                } else {
                    append(instructions, toBytes32(node->dotData.autoDerefStorage->localOffset));
                }

                append(instructions, toBytes32(4));
            }

            // dot is one of the few places where we actually *set* the node->localOffset, instead of just storing to it.
            // This is both for performance and to enable lvalue semantics. So we need to know where we are 'hijacking' our
            // localOffset from.
            auto hijackedOffsetLocation = node->dotData.lhs->localOffset;
            if (pointerCount > 1) {
                hijackedOffsetLocation = node->dotData.autoDerefStorage->localOffset;
            }

            auto lhsRel = node->dotData.lhs->type == NodeType::DOT && node->dotData.lhs->dotData.pointerIsRelative;

            // if we are an autoderef doing a dot on a relative lhs, then we need to
            // load it again, as it will be a double-pointer from our perspective
            if (lhsRel && node->dotData.autoDerefStorage) {
                node->dotData.pointerIsRelative = true;

                append(instructions, Instruction::STORE);

                append(instructions, Instruction::RELCONSTI32);
                append(instructions, toBytes(hijackedOffsetLocation));

                append(instructions, Instruction::RELI32);
                append(instructions, toBytes(hijackedOffsetLocation));

                append(instructions, toBytes32(4));
            }

            if (node->dotData.autoDerefStorage) {
                node->dotData.pointerIsRelative = true;

                append(instructions, Instruction::ADDI32);

                append(instructions, Instruction::RELI32);
                append(instructions, toBytes(hijackedOffsetLocation));

                append(instructions, Instruction::CONSTI32);
                append(instructions, toBytes(offsetWords));

                append(instructions, toBytes(node->dotData.autoDerefStorage->localOffset));

                node->isBytecodeLocal = true;
                node->localOffset = node->dotData.autoDerefStorage->localOffset;
            } else if (lhsRel) {
                node->dotData.pointerIsRelative = true;

                append(instructions, Instruction::ADDI32);

                append(instructions, Instruction::RELI32);
                append(instructions, toBytes(hijackedOffsetLocation));

                append(instructions, Instruction::CONSTI32);
                append(instructions, toBytes(offsetWords));

                append(instructions, toBytes(hijackedOffsetLocation));

                node->isBytecodeLocal = true;
                node->localOffset = hijackedOffsetLocation;
            } else {
                node->dotData.pointerIsRelative = false;

                node->isBytecodeLocal = true;
                node->localOffset = hijackedOffsetLocation + offsetWords;
            }
        } break;
        case NodeType::PANIC: {
            append(instructions, Instruction::PANIC);
        } break;
        case NodeType::IF: {
            gen(node->ifData.condition);
            if (node->ifData.condition->isLocal || node->ifData.condition->isBytecodeLocal) {
                storeValue(node->ifData.condition, node->ifData.condition->localOffset);
            }

            append(instructions, Instruction::JUMPIF);

            if (node->ifData.condition->isLocal || node->ifData.condition->isBytecodeLocal) {
                assert(node->ifData.condition->isLocal || node->ifData.condition->isBytecodeLocal);

                append(instructions, Instruction::RELI32);
                append(instructions, toBytes(node->ifData.condition->localOffset));
            }
            else {
                assert(!node->ifData.condition->bytecode.empty());
                append(instructions, node->ifData.condition->bytecode);
            }

            append(instructions, Instruction::CONSTI32);
            auto trueBranchOverwrite = instructions.size();
            append(instructions, toBytes32(888));

            append(instructions, Instruction::CONSTI32);
            auto falseBranchOverwrite = instructions.size();
            append(instructions, toBytes32(999));

            auto instSize = static_cast<int32_t>(instructions.size());
            memcpy(&instructions[trueBranchOverwrite], &instSize, sizeof(int32_t));

            for (const auto &stmt: node->ifData.stmts) {
                gen(stmt);
            }

            unsigned long skipElseBranchOverwrite = 0;
            auto hasElse = !node->ifData.elseStmts.empty();
            if (hasElse) {
                append(instructions, Instruction::JUMP);

                skipElseBranchOverwrite = instructions.size();
                append(instructions, toBytes32(999));
            }

            instSize = static_cast<int32_t>(instructions.size());
            memcpy(&instructions[falseBranchOverwrite], &instSize, sizeof(int32_t));

            if (hasElse) {
                for (const auto &stmt: node->ifData.elseStmts) {
                    gen(stmt);
                }

                instSize = static_cast<int32_t>(instructions.size());
                memcpy(&instructions[skipElseBranchOverwrite], &instSize, sizeof(int32_t));
            }
        } break;
        case NodeType::WHILE: {
            auto jumpBackToInst = instructions.size();

            gen(node->whileData.condition);

            append(instructions, Instruction::JUMPIF);
            append(instructions, node->whileData.condition->bytecode);

            append(instructions, Instruction::CONSTI32);
            auto trueBranchOverwrite = instructions.size();
            append(instructions, toBytes32(888));

            append(instructions, Instruction::CONSTI32);
            auto falseBranchOverwrite = instructions.size();
            append(instructions, toBytes32(999));

            auto instSize = static_cast<int32_t>(instructions.size());
            memcpy(&instructions[trueBranchOverwrite], &instSize, sizeof(int32_t));

            for (const auto &stmt: node->whileData.stmts) {
                gen(stmt);
            }

            append(instructions, Instruction::JUMP);
            append(instructions, toBytes32(jumpBackToInst));

            instSize = static_cast<int32_t>(instructions.size());
            memcpy(&instructions[falseBranchOverwrite], &instSize, sizeof(int32_t));
        } break;
        case NodeType::RUN: {
            // work should already be done, just need to go with whatever it resolved itself to
            gen(node->resolved);
        } break;
        case NodeType::ARRAY_INDEX: {
            gen(node->resolved);
        } break;
        case NodeType::CAST: {
            gen(node->castData.value);
        } break;
        case NodeType::STRING_LITERAL: {
            gen(node->resolved);
        } break;
        case NodeType::UNARY_NEG: {
            gen(node->nodeData);
        } break;
        case NodeType::MALLOC: {
            gen(node->nodeData);

            append(node->bytecode, Instruction::MALLOC);
            append(node->bytecode, node->nodeData->bytecode);
        } break;
        case NodeType::FREE: {
            gen(node->nodeData);

            if (node->nodeData->isLocal || node->nodeData->isBytecodeLocal) {
                storeValue(node->nodeData, node->nodeData->localOffset);
                append(instructions, Instruction::FREE);
                append(instructions, Instruction::RELI64);
                append(instructions, toBytes32(node->nodeData->localOffset));
            }
            else {
                assert(!node->nodeData->bytecode.empty());

                append(instructions, Instruction::FREE);
                append(instructions, node->nodeData->bytecode);
            }
        } break;
        case NodeType::ARRAY_LITERAL: {
            // nothing to do here! wait until we actually need to store it somewhere
        } break;
        case NodeType::FOR: {
            for (auto n : node->forData.rewritten) {
                gen(n);
            }
        } break;
        case NodeType::TAGCHECK: {
            assert(node->resolved);
            gen(node->resolved);
            node->bytecode = node->resolved->bytecode;

            if (node->isLocal || node->isBytecodeLocal) {
                storeValue(node->nodeData, node->nodeData->localOffset);
            }
        } break;
        case NodeType::HEAPIFY: {
            gen(node->nodeData);
            storeValue(node->nodeData, node->nodeData->localOffset);

            auto sizeInBytes = typeSize(node->typeInfo->typeData.pointerTypeData.underlyingType);

            // malloc numBytes and put the pointer in our localOffset
            append(instructions, Instruction::MALLOC);
            append(instructions, Instruction::CONSTI64);
            append(instructions, toBytes64(sizeInBytes));
            append(instructions, toBytes(node->localOffset));

            // store into the value pointed to by our local offset the value in nodeData
            append(instructions, Instruction::STORE);

            append(instructions, Instruction::RELI32);
            append(instructions, toBytes(node->localOffset));

            append(instructions, Instruction::RELCONSTI32);
            append(instructions, toBytes(node->nodeData->localOffset));

            append(instructions, toBytes32(sizeInBytes));

            append(node->bytecode, Instruction::RELCONSTI64);
            append(node->bytecode, toBytes32(node->localOffset));
        } break;
        case NodeType::TYPEINFO: {
            gen(node->resolved);
        } break;
        case NodeType::PUTS: {
            gen(node->nodeData);

            assert(node->nodeData->isLocal || node->nodeData->isBytecodeLocal);
            storeValue(node->nodeData, node->nodeData->localOffset);
            append(instructions, Instruction::PUTS);
            append(instructions, Instruction::RELCONSTI32);
            append(instructions, toBytes32(node->nodeData->localOffset));
        } break;
        default:
            assert(false);
    }

    for (auto stmt : node->postStmts) {
        gen(stmt);
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

// todo(chad): If this supported a non-constant offest (i.e. runtime offset), then a lot of the 'store rvalue into slot' stuff could go away
void BytecodeGen::storeValue(Node *node, int32_t offset) {
    node = resolve(node);

    switch (node->type) {
        case NodeType::BOOLEAN_LITERAL: {
            append(instructions, Instruction::STORECONST);

            append(instructions, Instruction::RELCONSTI32);
            append(instructions, toBytes(offset));

            append(instructions, node->bytecode);
        } break;
        case NodeType::INT_LITERAL:
        case NodeType::SIZEOF: {
            append(instructions, Instruction::STORECONST);

            append(instructions, Instruction::RELCONSTI32);
            append(instructions, toBytes(offset));

            append(instructions, node->bytecode);
        } break;
        case NodeType::NIL_LITERAL: {
            append(instructions, Instruction::STORECONST);

            append(instructions, Instruction::RELCONSTI32);
            append(instructions, toBytes(offset));

            append(instructions, node->bytecode);
        } break;
        case NodeType::FLOAT_LITERAL: {
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
            append(instructions, toBytes32(node->fnDeclData.tableIndex));
        } break;
        case NodeType::ADDRESS_OF: {
            append(instructions, Instruction::STORECONST);

            append(instructions, Instruction::RELCONSTI32);
            append(instructions, toBytes(offset));

            append(instructions, Instruction::RELCONSTI32);
            append(instructions, toBytes(node->nodeData->localOffset));
        } break;
        case NodeType::DEREF: {
            append(instructions, Instruction::STORE);

            append(instructions, Instruction::RELCONSTI32);
            append(instructions, toBytes(offset));

            append(instructions, Instruction::RELI32);
            auto resolvedDeref = resolve(node->derefData.target);
            append(instructions, toBytes(resolvedDeref->localOffset));

            append(instructions, toBytes32(typeSize(node->typeInfo)));
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
            gen(node);

            append(instructions, Instruction::STORE);
            append(instructions, Instruction::RELCONSTI32);
            append(instructions, toBytes32(offset));

            if (node->dotData.pointerIsRelative) {
                append(instructions, Instruction::RELI32);
                append(instructions, toBytes(node->localOffset));
            } else {
                append(instructions, Instruction::RELCONSTI32);
                append(instructions, toBytes(node->localOffset));
            }

            append(instructions, toBytes32(typeSize(node->typeInfo)));
        } break;
        case NodeType::STRUCT_LITERAL: {
            if (node->typeInfo->typeData.structTypeData.coercedType != nullptr
                && node->typeInfo->typeData.structTypeData.coercedType->typeData.structTypeData.isSecretlyEnum) {
                // we need to store the tag and the value.
                auto tagIndex = node->typeInfo->typeData.structTypeData.params[0]->declParamData.index;
                auto value = node->structLiteralData.params[0];

                auto tagValue = new Node();
                tagValue->type = NodeType::INT_LITERAL;
                // todo(chad): allow the user to specify the size of the tag (or maybe do it automatically?)
                // todo(chad): maybe do this sooner - like in semantic back when we recognize this case?
                tagValue->typeInfo = new Node(NodeTypekind::I32);
                tagValue->intLiteralData.value = tagIndex;

                assert(value->type == NodeType::VALUE_PARAM);

                gen(tagValue);
                gen(value->valueParamData.value);

                storeValue(tagValue, offset);
                storeValue(value->valueParamData.value, offset + 4);
            }
            else {
                auto sizeSoFar = 0;
                for (const auto &param : node->structLiteralData.params) {
                    gen(param->valueParamData.value);

                    storeValue(param->valueParamData.value, offset + sizeSoFar);
                    sizeSoFar += typeSize(param->valueParamData.value->typeInfo);
                }
            }
        } break;
        case NodeType::ARRAY_LITERAL: {
            storeValue(node->arrayLiteralData.structLiteralRepresentation, offset);
        } break;
        case NodeType::CAST: {
            storeValue(node->castData.value, offset);
        } break;
        case NodeType::UNARY_NEG: {
            storeValue(node->nodeData, offset);
        } break;
        case NodeType::MALLOC: {
            append(node->bytecode, toBytes(offset));
            append(instructions, node->bytecode);
        } break;
        case NodeType::HEAPIFY: {
            append(instructions, Instruction::STORE);

            append(instructions, Instruction::RELCONSTI32);
            append(instructions, toBytes32(offset));

            append(instructions, Instruction::RELCONSTI32);
            append(instructions, toBytes(node->localOffset));

            // the size is the size of a pointer -- or 8 bytes
            append(instructions, toBytes32(8));
        } break;
        default:
            assert(false);
    }
}
