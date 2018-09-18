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

void makeStore(vector<unsigned char> &instructions,
               Instruction readInst, int64_t readOffset,
               Instruction writeInst, int64_t writeOffset,
               int32_t size) {
    if  (readInst == writeInst && readOffset == writeOffset) {
        return;
    }

    append(instructions, Instruction::STORE);

    append(instructions, readInst);
    append(instructions, toBytes(readOffset));

    append(instructions, writeInst);
    append(instructions, toBytes(writeOffset));

    append(instructions, toBytes(size));
}

Node *bytecodeResolve(Node *n) {
    if (n == nullptr) { return nullptr; }

    auto resolved = n;

    while (resolved->resolved != nullptr || resolved->staticValue != nullptr || resolved->bytecodeResolved != nullptr) {
        if (resolved->resolved != nullptr) {
            resolved = resolved->resolved;
        } else if (resolved->staticValue != nullptr) {
            resolved = resolved->staticValue;
        } else {
            resolved = resolved->bytecodeResolved;
        }
    }

    return resolved;
}

void BytecodeGen::binopHelper(string instructionStr, Node *node, int32_t scale) {
    string bytecodeStr;

    auto resolvedLhsType = bytecodeResolve(node->binopData.lhs->typeInfo);
    auto resolvedRhsType = bytecodeResolve(node->binopData.lhs->typeInfo);

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

    if (kind == NodeTypekind::ENUM) {
        kind = resolvedLhsType->typeData.enumTypeData.type->typeData.kind;
    }

    string toAppend;
    switch (kind) {
        case NodeTypekind::U8:
        case NodeTypekind::I8: {
            toAppend = "I8";
            bytecodeStr = "RELI8";
        } break;
        case NodeTypekind::U16:
        case NodeTypekind::I16: {
            toAppend = "I16";
            bytecodeStr = "RELI16";
        } break;
        case NodeTypekind::BOOLEAN:
        case NodeTypekind::BOOLEAN_LITERAL:
        case NodeTypekind::U32:
        case NodeTypekind::I32: {
            toAppend = "I32";
            bytecodeStr = "RELI32";
        } break;
        case NodeTypekind::POINTER:
        case NodeTypekind::U64:
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
            cpi_assert(false);
    }

    if (isBoolean) {
        bytecodeStr = "RELI32";
    }

    instructionStr.append(toAppend);

    auto inst = *hash_get(AssemblyLexer::nameToInstruction, instructionStr);
    append(instructions, inst);

    auto bytecodeInst = *hash_get(AssemblyLexer::nameToInstruction, bytecodeStr);
    append(node->bytecode, bytecodeInst);

    append(instructions, bytecodeInst);
    append(instructions, toBytes(node->binopData.lhsTemporary->localOffset));

    append(instructions, bytecodeInst);
    append(instructions, toBytes(node->binopData.rhsTemporary->localOffset));

    if (scale > 1) {
        append(instructions, toBytes32(scale));
    }

    append(instructions, toBytes(node->localOffset));
    append(node->bytecode, toBytes(node->localOffset));
}

void BytecodeGen::genDot(Node *node) {
    auto foundParam = node->dotData.resolved;
    auto offsetWords = foundParam->localOffset;

    gen(node->dotData.lhs);

    if (hasNoLocalByDefault(node->dotData.lhs)) {
        cpi_assert(node->dotData.lhs->isLocal);
        storeValue(node->dotData.lhs, node->dotData.lhs->localOffset);
    }

    auto resolvedTypeInfo = bytecodeResolve(bytecodeResolve(node->dotData.lhs)->typeInfo);

    auto pointerCount = 0;

    while (resolvedTypeInfo->typeData.kind == NodeTypekind::POINTER) {
        resolvedTypeInfo = bytecodeResolve(resolvedTypeInfo->typeData.pointerTypeData.underlyingType);
        pointerCount += 1;
    }
    for (auto i = 0; i < pointerCount - 1; i++) {
        int64_t readBytes;
        if (i == 0) {
            readBytes = node->dotData.lhs->localOffset;
        } else {
            readBytes = node->dotData.autoDerefStorage->localOffset;
        }

        makeStore(instructions, Instruction::RELCONSTI64, node->dotData.autoDerefStorage->localOffset, Instruction::I64, readBytes, 8);
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

        makeStore(instructions, Instruction::RELCONSTI64, hijackedOffsetLocation, Instruction::I64, hijackedOffsetLocation, 8);
    }

    if (node->dotData.autoDerefStorage) {
        node->dotData.pointerIsRelative = true;

        append(instructions, Instruction::ADDI64);

        append(instructions, Instruction::RELI64);
        append(instructions, toBytes(hijackedOffsetLocation));

        append(instructions, Instruction::CONSTI64);
        append(instructions, toBytes(offsetWords));

        append(instructions, toBytes(node->dotData.autoDerefStorage->localOffset));

        node->isBytecodeLocal = true;
        node->localOffset = node->dotData.autoDerefStorage->localOffset;
    } else if (lhsRel) {
        node->dotData.pointerIsRelative = true;

        append(instructions, Instruction::ADDI64);

        append(instructions, Instruction::RELI64);
        append(instructions, toBytes(hijackedOffsetLocation));

        append(instructions, Instruction::CONSTI64);
        append(instructions, toBytes(offsetWords));

        append(instructions, toBytes(hijackedOffsetLocation));

        node->isBytecodeLocal = true;
        node->localOffset = hijackedOffsetLocation;
    } else {
        node->dotData.pointerIsRelative = false;

        node->isBytecodeLocal = true;
        node->localOffset = hijackedOffsetLocation + offsetWords;
    }
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

    if (node->gen && !forcing) {
        return;
    }

    // when forcing, don't run the risk of adding double bytecode
    if (forcing) {
        node->bytecode = {};
    }

    if (this->debugLocalOffset != 0 && !node->debugBytecodeAdjusted) {
        node->debugBytecodeAdjusted = true;
        node->localOffset += this->debugLocalOffset;
    }

    node->gen = true;
    vector_append(this->generatedNodes, node);

    for (auto stmt : node->preStmts) {
        gen(stmt);
    }

    if (node->skipAllButPostStmts) {
        for (auto stmt : node->postStmts) {
            gen(stmt);
        }

        return;
    }

    if (node->sourceMapStatement && node->type != NodeType::FN_CALL) {
        sourceMap.statements.push_back(SourceMapStatement{ instructions.size(), instructions.size(), node });
    }

    switch (node->type) {
        case NodeType::FN_DECL: {
            processFnDecls = false;

            node->fnDeclData.instOffset = instructions.size();

            hash_insert(fnTable, node->fnDeclData.tableIndex, node->fnDeclData.instOffset);

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
                if (stmt->type == NodeType::RETURN || stmt->type == NodeType::PANIC) { didTerminate = true; }
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
        case NodeType::RETURN: {
            if (node->retData.value != nullptr) {
                gen(node->retData.value);
                storeValue(node->retData.value, 0);
            }

            sourceMap.statements.push_back(SourceMapStatement{ instructions.size(), instructions.size(), node });
            append(instructions, Instruction::NOP);

            if (isMainFn) {
                append(instructions, Instruction::EXIT);
            } else {
                append(instructions, Instruction::RET);
            }
        } break;
        case NodeType::INT_LITERAL: {
            switch (node->typeInfo->typeData.kind) {
                case NodeTypekind::U8:
                case NodeTypekind::I8: {
                    append(node->bytecode, Instruction::CONSTI8);
                    auto litData = static_cast<int8_t>(node->intLiteralData.value);
                    append(node->bytecode, toBytes(litData));
                } break;
                case NodeTypekind::U16:
                case NodeTypekind::I16: {
                    append(node->bytecode, Instruction::CONSTI16);
                    auto litData = static_cast<int16_t>(node->intLiteralData.value);
                    append(node->bytecode, toBytes(litData));
                } break;
                case NodeTypekind::U32:
                case NodeTypekind::I32: {
                    append(node->bytecode, Instruction::CONSTI32);
                    auto litData = static_cast<int32_t>(node->intLiteralData.value);
                    append(node->bytecode, toBytes(litData));
                } break;
                case NodeTypekind::U64:
                case NodeTypekind::I64:
                case NodeTypekind::INT_LITERAL: {
                    append(node->bytecode, Instruction::CONSTI64);
                    auto litData = node->intLiteralData.value;
                    append(node->bytecode, toBytes(litData));
                } break;
                case NodeTypekind::F32: {
                    append(node->bytecode, Instruction::CONSTF32);
                    auto litData = (float) node->intLiteralData.value;
                    append(node->bytecode, toBytes(litData));
                } break;
                case NodeTypekind::F64: {
                    append(node->bytecode, Instruction::CONSTF64);
                    auto litData = (double) node->intLiteralData.value;
                    append(node->bytecode, toBytes(litData));
                } break;
                default: cpi_assert(false);
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
                } break;
                case NodeTypekind::F64: {
                    append(node->bytecode, Instruction::CONSTF64);
                    auto litData = node->floatLiteralData.value;
                    append(node->bytecode, toBytes(litData));
                } break;
                default: cpi_assert(false);
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
            if (node->staticValue != nullptr) {
                gen(node->staticValue);
            }
            else {
                auto data = node->declData;

                auto localOffset = node->localOffset;

                if (data.initialValue != nullptr) {
                    gen(data.initialValue);
                    auto resolvedInitialValue = bytecodeResolve(data.initialValue);
                    gen(resolvedInitialValue);
                    storeValue(resolvedInitialValue, localOffset);
                }
            }
        } break;
        case NodeType::ASSIGN: {
            auto data = node->assignData;

            gen(data.rhs);

            auto resolvedDecl = bytecodeResolve(data.lhs);
            if (resolvedDecl->type == NodeType::DECL) {
                auto localOffset = static_cast<int64_t>(resolvedDecl->localOffset);
                storeValue(data.rhs, localOffset);
            } else if (resolvedDecl->type == NodeType::DEREF) {
                gen(data.lhs);

                // store rvalue into it's slot
                storeValue(data.rhs, data.rhs->localOffset);

                auto localOffset = static_cast<int64_t>(bytecodeResolve(resolvedDecl->nodeData)->localOffset);

                cpi_assert(resolve(resolvedDecl->nodeData->typeInfo)->typeData.kind == NodeTypekind::POINTER);
                makeStore(instructions, Instruction::I64, localOffset, Instruction::RELCONSTI64, data.rhs->localOffset, typeSize(resolvedDecl->typeInfo));
            } else if (resolvedDecl->type == NodeType::DOT) {
                // store rvalue into it's slot
                storeValue(data.rhs, data.rhs->localOffset);

                gen(resolvedDecl);

                if (resolvedDecl->dotData.pointerIsRelative) {
                    makeStore(instructions, Instruction::I64, resolvedDecl->localOffset,
                              Instruction::RELCONSTI64, data.rhs->localOffset, typeSize(resolvedDecl->typeInfo));
                } else {
                    makeStore(instructions, Instruction::RELCONSTI64, resolvedDecl->localOffset,
                              Instruction::RELCONSTI64, data.rhs->localOffset, typeSize(resolvedDecl->typeInfo));
                }
            } else {
                cpi_assert(false);
            }
        } break;
        case NodeType::SYMBOL: {
            auto resolved = bytecodeResolve(node);
            auto resolvedType = bytecodeResolve(resolved->typeInfo);
            gen(resolved);

            auto localOffset = resolved->localOffset;
            switch (resolvedType->typeData.kind) {
                case NodeTypekind::FN: {
                    if (resolved->type == NodeType::FN_DECL) {
                        append(node->bytecode, Instruction::RELCONSTI32);
                        append(node->bytecode, toBytes32(resolved->fnDeclData.tableIndex));
                    } else {
                        cpi_assert(resolved->type == NodeType::DECL || resolved->type == NodeType::DECL_PARAM);
                        append(node->bytecode, Instruction::RELI64);
                        append(node->bytecode, toBytes(localOffset));
                    }
                } break;
                case NodeTypekind::U8:
                case NodeTypekind::I8: {
                    append(node->bytecode, Instruction::RELI8);
                    append(node->bytecode, toBytes(localOffset));
                } break;
                case NodeTypekind::U16:
                case NodeTypekind::I16: {
                    append(node->bytecode, Instruction::RELI64);
                    append(node->bytecode, toBytes(localOffset));
                } break;
                case NodeTypekind::BOOLEAN_LITERAL:
                case NodeTypekind::BOOLEAN:
                case NodeTypekind::U32:
                case NodeTypekind::I32: {
                    append(node->bytecode, Instruction::RELI64);
                    append(node->bytecode, toBytes(localOffset));
                } break;
                case NodeTypekind::POINTER:
                case NodeTypekind::INT_LITERAL:
                case NodeTypekind::U64:
                case NodeTypekind::I64: {
                    append(node->bytecode, Instruction::RELI64);
                    append(node->bytecode, toBytes(localOffset));
                } break;
                case NodeTypekind::FLOAT_LITERAL:
                case NodeTypekind::F32: {
                    append(node->bytecode, Instruction::RELF32);
                    append(node->bytecode, toBytes(localOffset));
                } break;
                case NodeTypekind::F64: {
                    append(node->bytecode, Instruction::RELF64);
                    append(node->bytecode, toBytes(localOffset));
                } break;
                case NodeTypekind::ENUM:
                case NodeTypekind::NONE:
                case NodeTypekind::STRUCT: {
                    // nothing to do
                } break;
                default:
                    cpi_assert(false);
            }

            node->localOffset = resolved->localOffset;
        } break;
        case NodeType::BINOP: {
            auto resolved = resolve(node);
            if (resolved != node) {
                gen(resolved);
                return;
            }

            if (node->binopData.type == LexerTokenType::VERTICAL_BAR) {
                gen(bytecodeResolve(node));
            }
            else if (node->binopData.type == LexerTokenType::AND) {
                // a and b ====> { result := false; if a { if b { result = true; } }

                append(node->bytecode, Instruction::RELI64);
                append(node->bytecode, toBytes(node->localOffset));

                // initially store false
                append(instructions, Instruction::STORECONST);
                append(instructions, Instruction::RELCONSTI64);
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
                    append(instructions, Instruction::RELCONSTI64);
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

                append(node->bytecode, Instruction::RELI64);
                append(node->bytecode, toBytes(node->localOffset));

                // initially store true
                append(instructions, Instruction::STORECONST);
                append(instructions, Instruction::RELCONSTI64);
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
                append(instructions, Instruction::RELCONSTI64);
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
                    append(instructions, Instruction::RELCONSTI64);
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

                auto lhsKind = node->binopData.lhs->typeInfo->typeData.kind;
                auto rhsKind = node->binopData.rhs->typeInfo->typeData.kind;

                auto isFloat = (lhsKind == NodeTypekind::FLOAT_LITERAL || lhsKind == NodeTypekind::F32 || lhsKind == NodeTypekind::F64
                                || rhsKind == NodeTypekind::FLOAT_LITERAL || rhsKind == NodeTypekind::F32 || rhsKind == NodeTypekind::F64);

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
                        if (isFloat) {
                            binopHelper("DIV", node);
                        }
                        else {
                            binopHelper("SDIV", node);
                        }
                    } break;
                    case LexerTokenType::EQ_EQ: {
                        binopHelper("EQ", node);
                    } break;
                    case LexerTokenType::NE: {
                        binopHelper("NEQ", node);
                    } break;
                    case LexerTokenType::LT: {
                        if (isFloat) {
                            binopHelper("LT", node);
                        }
                        else {
                            binopHelper("SLT", node);
                        }
                    } break;
                    case LexerTokenType::LE: {
                        if (isFloat) {
                            binopHelper("LE", node);
                        }
                        else {
                            binopHelper("SLE", node);
                        }
                    } break;
                    case LexerTokenType::GT: {
                        if (isFloat) {
                            binopHelper("GT", node);
                        }
                        else {
                            binopHelper("SGT", node);
                        }
                    } break;
                    case LexerTokenType::GE: {
                        if (isFloat) {
                            binopHelper("GE", node);
                        }
                        else {
                            binopHelper("SGE", node);
                        }
                    } break;
                    case LexerTokenType::MOD: {
                        if (isFloat) {
                            binopHelper("REM", node);
                        }
                        else {
                            binopHelper("SREM", node);
                        }
                    } break;
                    case LexerTokenType::BITAND: {
                        append(instructions, Instruction::BITAND);
                        append(instructions, toBytes32(typeSize(node->binopData.lhs->typeInfo)));
                        append(instructions, toBytes(node->binopData.lhsTemporary->localOffset));
                        append(instructions, toBytes(node->binopData.rhsTemporary->localOffset));
                        append(instructions, toBytes(node->localOffset));
                    } break;
                    case LexerTokenType::BITOR: {
                        append(instructions, Instruction::BITOR);
                        append(instructions, toBytes32(typeSize(node->binopData.lhs->typeInfo)));
                        append(instructions, toBytes(node->binopData.lhsTemporary->localOffset));
                        append(instructions, toBytes(node->binopData.rhsTemporary->localOffset));
                        append(instructions, toBytes(node->localOffset));
                    } break;
                    case LexerTokenType::BITXOR: {
                        append(instructions, Instruction::BITXOR);
                        append(instructions, toBytes32(typeSize(node->binopData.lhs->typeInfo)));
                        append(instructions, toBytes(node->binopData.lhsTemporary->localOffset));
                        append(instructions, toBytes(node->binopData.rhsTemporary->localOffset));
                        append(instructions, toBytes(node->localOffset));
                    } break;
                    case LexerTokenType::BITSHL: {
                        append(instructions, Instruction::BITSHL);
                        append(instructions, toBytes32(typeSize(node->binopData.lhs->typeInfo)));
                        append(instructions, toBytes(node->binopData.lhsTemporary->localOffset));
                        append(instructions, toBytes(node->binopData.rhsTemporary->localOffset));
                        append(instructions, toBytes(node->localOffset));
                    } break;
                    case LexerTokenType::BITSHR: {
                        append(instructions, Instruction::BITSHR);
                        append(instructions, toBytes32(typeSize(node->binopData.lhs->typeInfo)));
                        append(instructions, toBytes(node->binopData.lhsTemporary->localOffset));
                        append(instructions, toBytes(node->binopData.rhsTemporary->localOffset));
                        append(instructions, toBytes(node->localOffset));
                    } break;
                    default:
                        cpi_assert(false);
                }
            }
        } break;
        case NodeType::FN_CALL: {
            if (!node->fnCallData.hasRuntimeParams) { break; }

            auto resolvedFn = bytecodeResolve(node->fnCallData.fn);

            if (resolvedFn->type != NodeType::FN_DECL && resolvedFn->type != NodeType::DECL) {
                gen(resolvedFn);
                storeValue(resolvedFn, node->fnCallData.fn->localOffset);
            }

            auto paramCount = node->fnCallData.params.length;
            int32_t totalParamsSize = 0;
            for (unsigned int i = 0; i < paramCount; i++) {
                auto paramValue = vector_at(node->fnCallData.params, i)->paramData.value;
                totalParamsSize += typeSize(paramValue->typeInfo);
            }

            if (totalParamsSize > 0) {
                // push the params (in reverse order!)
                auto paramAccum = 0;
                for (auto i = static_cast<int32_t>(paramCount - 1); i >= 0; i--) {
                    auto paramValue = vector_at(node->fnCallData.params, i)->paramData.value;
                    gen(paramValue);
                }

                for (auto i = static_cast<int32_t>(paramCount - 1); i >= 0; i--) {
                    auto paramValue = vector_at(node->fnCallData.params, i)->paramData.value;
                    auto paramSize = typeSize(paramValue->typeInfo);
                    storeValue(bytecodeResolve(paramValue), static_cast<int32_t>(currentFnStackSize + paramAccum));
                    paramAccum += paramSize;
                }

                append(instructions, Instruction::BUMPSP);
                append(instructions, toBytes(totalParamsSize));
            }

            sourceMap.statements.push_back(SourceMapStatement{ instructions.size(), instructions.size(), node });

            if (resolvedFn->type == NodeType::FN_DECL) {
                if (resolvedFn->fnDeclData.isExternal) {
                    append(instructions, Instruction::CALLE);
                    append(instructions, toBytes32(this->externalFnTable.length));
                    vector_append(this->externalFnTable, node);
                }
                else {
                    append(instructions, Instruction::CALL);
                    vector_append(fixups, {(int64_t) instructions.size(), resolvedFn});
                    append(instructions, toBytes32(999));
                }
            } else if (resolvedFn->type == NodeType::DECL) {
                append(instructions, Instruction::CALLI);
                append(instructions, Instruction::RELI64);
                append(instructions, toBytes(resolvedFn->localOffset));
            } else if (resolvedFn->type == NodeType::DEREF) {
                append(instructions, Instruction::CALLI);
                append(instructions, Instruction::RELI64);
                append(instructions, toBytes(node->fnCallData.fn->localOffset));
            } else if (resolvedFn->type == NodeType::DOT) {
                cpi_assert(resolvedFn->isLocal || resolvedFn->isBytecodeLocal);
                append(instructions, Instruction::CALLI);
                append(instructions, Instruction::RELI64);
                append(instructions, toBytes(resolvedFn->localOffset));
            } else {
                append(instructions, Instruction::CALLI);
                append(instructions, resolvedFn->bytecode);
            }

            // copy bytes
            auto returnTypeSize = typeSize(resolve(resolvedFn->typeInfo)->typeData.fnTypeData.returnType);
            if (returnTypeSize > 0) {
                makeStore(instructions, Instruction::RELCONSTI64, node->localOffset, Instruction::RELCONSTI64, currentFnStackSize + totalParamsSize + 8, returnTypeSize);
            }

            if (totalParamsSize > 0) {
                append(instructions, Instruction::BUMPSP);
                append(instructions, toBytes(-totalParamsSize));
            }

            append(node->bytecode, Instruction::RELI64);
            append(node->bytecode, toBytes(node->localOffset));

            toProcess.push(resolvedFn);
        } break;
        case NodeType::DECL_PARAM: {
            append(node->bytecode, Instruction::RELI64);
            append(node->bytecode, toBytes(node->localOffset));
        } break;
        case NodeType::DEREF: {
            gen(node->nodeData);

            storeValue(node->nodeData, node->nodeData->localOffset);

            if (node->isLocal || node->isBytecodeLocal) {
                makeStore(instructions, Instruction::RELCONSTI64, node->localOffset, Instruction::I64, node->nodeData->localOffset, typeSize(node->typeInfo));
            }
        } break;
        case NodeType::ADDRESS_OF: {
            auto resolved = resolve(node);
            if (resolved != node) {
                gen(resolved);
                node->isLocal = resolved->isLocal;
                node->isBytecodeLocal = resolved->isBytecodeLocal;
                node->localOffset = resolved->localOffset;
                node->bytecode = resolved->bytecode;
                break;
            }

            gen(node->nodeData);

            if (hasNoLocalByDefault(node->nodeData) && (node->nodeData->isLocal || node->nodeData->isBytecodeLocal)) {
                storeValue(node->nodeData, node->nodeData->localOffset);
            }

            if (node->isLocal || node->isBytecodeLocal) {
                append(instructions, Instruction::STORECONST);
                append(instructions, Instruction::RELCONSTI64);
                append(instructions, toBytes(node->localOffset));
                append(instructions, Instruction::RELI64);
                append(instructions, toBytes(node->nodeData->localOffset));
            }
        } break;
        case NodeType::TYPE: {
            // just a type declaration, nothing to do here!
        } break;
        case NodeType::DOT: {
            if (node->resolved != nullptr) {
                gen(resolve(node));
            }
            else {
                genDot(node);
            }
        } break;
        case NodeType::PANIC: {
            append(instructions, Instruction::PANIC);
        } break;
        case NodeType::IF: {
            auto resolvedCondition = bytecodeResolve(node->ifData.condition);

            gen(resolvedCondition);
            if (resolvedCondition->isLocal || resolvedCondition->isBytecodeLocal) {
                storeValue(resolvedCondition, resolvedCondition->localOffset);
            }

            append(instructions, Instruction::JUMPIF);

            if (resolvedCondition->isLocal || resolvedCondition->isBytecodeLocal) {
                cpi_assert(resolvedCondition->isLocal || resolvedCondition->isBytecodeLocal);

                append(instructions, Instruction::RELI64);
                append(instructions, toBytes(resolvedCondition->localOffset));
            }
            else {
                cpi_assert(!resolvedCondition->bytecode.empty());
                append(instructions, resolvedCondition->bytecode);
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
            auto hasElse = node->ifData.elseStmts.length != 0;
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

            auto resolvedCondition = resolve(node->whileData.condition);
            gen(resolvedCondition);

            if (resolvedCondition->isLocal || resolvedCondition->isBytecodeLocal) {
                storeValue(resolvedCondition, resolvedCondition->localOffset);
            }

            append(instructions, Instruction::JUMPIF);
            if (resolvedCondition->isLocal || resolvedCondition->isBytecodeLocal) {
                cpi_assert(resolvedCondition->isLocal || resolvedCondition->isBytecodeLocal);

                append(instructions, Instruction::RELI64);
                append(instructions, toBytes(resolvedCondition->localOffset));
            }
            else {
                cpi_assert(!resolvedCondition->bytecode.empty());
                append(instructions, resolvedCondition->bytecode);
            }

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

            storeValue(node->castData.value, node->castData.value->localOffset);

            auto fromType = resolve(resolve(node->castData.value)->typeInfo);
            auto toType = resolve(node->typeInfo);

            // handle numeric conversions
            if (isNumericType(fromType)
                && isNumericType(toType)
                && fromType->typeData.kind != toType->typeData.kind) {
                append(instructions, Instruction::CONVERT);

                append(instructions, toBytes32(fromType->typeData.kind));
                append(instructions, toBytes(node->castData.value->localOffset));

                append(instructions, toBytes32(toType->typeData.kind));
                append(instructions, toBytes(node->localOffset));
            }
            else {
                // copy the bytes from the value's localOffset to the node's localOffset
                makeStore(instructions, Instruction::RELCONSTI64, node->localOffset, Instruction::RELCONSTI64, node->castData.value->localOffset, typeSize(node->typeInfo));
            }
        } break;
        case NodeType::UNARY_NEG: {
            gen(node->unaryNegData.rewritten);
        } break;
        case NodeType::UNARY_NOT: {
            gen(node->nodeData);

            if (node->nodeData->isLocal) {
                storeValue(node->nodeData, node->nodeData->localOffset);
            }
            storeValue(node->nodeData, node->localOffset);

            append(instructions, Instruction::NOT);
            append(instructions, toBytes(node->localOffset));

            node->isBytecodeLocal = true;
        } break;
        case NodeType::UNARY_BITNOT: {
            gen(node->nodeData);

            if (node->nodeData->isLocal) {
                storeValue(node->nodeData, node->nodeData->localOffset);
            }
            storeValue(node->nodeData, node->localOffset);

            append(instructions, Instruction::BITNOT);
            append(instructions, toBytes(typeSize(node->typeInfo)));
            append(instructions, toBytes(node->localOffset));

            node->isBytecodeLocal = true;
        } break;
        case NodeType::ARRAY_LITERAL: {
            // nothing to do here! wait until we actually need to store it somewhere
        } break;
        case NodeType::FOR: {
            if (node->forData.isStatic) {
                for (auto n : node->forData.staticStmts) {
                    gen(n);
                }
            }
            else {
                for (auto n : node->forData.rewritten) {
                    gen(n);
                }
            }
        } break;
        case NodeType::TAGCHECK: {
            cpi_assert(node->resolved);
            gen(node->resolved);
            node->bytecode = node->resolved->bytecode;

            if (node->isLocal || node->isBytecodeLocal) {
                storeValue(node->nodeData, node->nodeData->localOffset);
            }
        } break;
        case NodeType::PUTS: {
            gen(node->nodeData);

            auto resolvedNodeData = resolve(node->nodeData);
            cpi_assert(resolvedNodeData->isLocal || resolvedNodeData->isBytecodeLocal);

            storeValue(resolvedNodeData, resolvedNodeData->localOffset);
            append(instructions, Instruction::PUTS);
            append(instructions, Instruction::RELCONSTI64);
            append(instructions, toBytes(resolvedNodeData->localOffset));
        } break;
        case NodeType::FIELDSOF: {
            gen(node->resolved);
        } break;
        case NodeType::ISKIND: {
            gen(node->resolved);
            node->bytecode = node->resolved->bytecode;
        } break;
        case NodeType::END_SCOPE: {
            append(instructions, Instruction::NOP);
        } break;
        case NodeType::ALIAS: {
            gen(node->declData.initialValue);
            node->bytecode = node->declData.initialValue->bytecode;
        } break;
        case NodeType::ENUM_LITERAL: {
            gen(node->enumLiteralData.value);
            node->bytecode = node->enumLiteralData.value->bytecode;
        } break;
        case NodeType::STRING_LITERAL:
        case NodeType::MODULE:
        case NodeType::DEFER:
        case NodeType::LINK:
        case NodeType::TYPEOF:
        case NodeType::RETURNTYPEOF:
        case NodeType::PARAMETERIZED_TYPE:
        case NodeType::IMPORT: {
            // nothing to do!
        } break;
        default:
            cpi_assert(false);
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
    for (auto&& f : fixups) {
        auto node = f.node;
        cpi_assert(node->type == NodeType::FN_DECL);
        auto instOffset = static_cast<int32_t>(node->fnDeclData.instOffset);
        memcpy(&instructions[f.instOffset], &instOffset, sizeof(int32_t));
    }
}

// todo(chad): If this supported a non-constant offest (i.e. runtime offset), then a lot of the 'store rvalue into slot' stuff could go away
void BytecodeGen::storeValue(Node *node, int64_t offset) {
    node = bytecodeResolve(node);

    switch (node->type) {
        case NodeType::BOOLEAN_LITERAL: {
            append(instructions, Instruction::STORECONST);

            append(instructions, Instruction::RELCONSTI64);
            append(instructions, toBytes(offset));

            append(instructions, node->bytecode);
        } break;
        case NodeType::INT_LITERAL:
        case NodeType::SIZEOF: {
            append(instructions, Instruction::STORECONST);

            append(instructions, Instruction::RELCONSTI64);
            append(instructions, toBytes(offset));

            append(instructions, node->bytecode);
        } break;
        case NodeType::NIL_LITERAL: {
            append(instructions, Instruction::STORECONST);

            append(instructions, Instruction::RELCONSTI64);
            append(instructions, toBytes(offset));

            append(instructions, node->bytecode);
        } break;
        case NodeType::FLOAT_LITERAL: {
            append(instructions, Instruction::STORECONST);

            append(instructions, Instruction::RELCONSTI64);
            append(instructions, toBytes(offset));

            append(instructions, node->bytecode);
        } break;
        case NodeType::FN_DECL: {
            append(instructions, Instruction::STORECONST);

            append(instructions, Instruction::RELCONSTI64);
            append(instructions, toBytes(offset));

            append(instructions, Instruction::CONSTI32);
            append(instructions, toBytes32(node->fnDeclData.tableIndex));
        } break;
        case NodeType::ADDRESS_OF: {
            if (node->nodeData->type == NodeType::DOT && node->nodeData->dotData.pointerIsRelative) {
                makeStore(instructions, Instruction::RELCONSTI64, offset, Instruction::RELCONSTI64, node->nodeData->localOffset, 8);
            }
            else {
                append(instructions, Instruction::STORECONST);

                append(instructions, Instruction::RELCONSTI64);
                append(instructions, toBytes(offset));

                append(instructions, Instruction::RELI64);
                append(instructions, toBytes(node->nodeData->localOffset));
            }
        } break;
        case NodeType::DEREF: {
            auto resolvedDeref = bytecodeResolve(node->nodeData);

            makeStore(instructions, Instruction::RELCONSTI64, offset, Instruction::I64, resolvedDeref->localOffset, typeSize(node->typeInfo));
        } break;
        case NodeType::CAST:
        case NodeType::DECL_PARAM:
        case NodeType::FN_CALL:
        case NodeType::BINOP:
        case NodeType::UNARY_NOT:
        case NodeType::UNARY_BITNOT:
        case NodeType::DECL: {
            auto resolved = resolve(node);

            if (resolved != node) {
                storeValue(node->staticValue, offset);
            }
            else {
                makeStore(instructions, Instruction::RELCONSTI64, offset, Instruction::RELCONSTI64, node->localOffset, typeSize(node->typeInfo));
            }
        } break;
        case NodeType::DOT: {
            gen(node);

            if (node->dotData.pointerIsRelative) {
                makeStore(instructions, Instruction::RELCONSTI64, offset, Instruction::I64, node->localOffset, typeSize(node->typeInfo));
            } else {
                makeStore(instructions, Instruction::RELCONSTI64, offset, Instruction::RELCONSTI64, node->localOffset, typeSize(node->typeInfo));
            }
        } break;
        case NodeType::STRUCT_LITERAL: {
            if (node->typeInfo->typeData.structTypeData.coercedType != nullptr
                && node->typeInfo->typeData.structTypeData.coercedType->typeData.structTypeData.isSecretlyUnion) {

                // we need to store the tag and the value.
                auto tagIndex = vector_at(node->typeInfo->typeData.structTypeData.params, 0)->paramData.index;
                auto value = vector_at(node->structLiteralData.params, 0);

                auto tagValue = new Node(node->region);
                tagValue->type = NodeType::INT_LITERAL;
                if (node->typeInfo->typeData.structTypeData.coercedType->typeData.structTypeData.unionTagType != nullptr) {
                    tagValue->typeInfo = node->typeInfo->typeData.structTypeData.coercedType->typeData.structTypeData.unionTagType;
                }
                else {
                    tagValue->typeInfo = new Node(NodeTypekind::I64);
                }
                tagValue->intLiteralData.value = tagIndex;

                cpi_assert(value->type == NodeType::VALUE_PARAM);

                gen(tagValue);
                gen(value->paramData.value);

                storeValue(tagValue, offset);
                storeValue(value->paramData.value, offset + 8);
            }
            else {
                auto sizeSoFar = 0;

                for (const auto &param : node->structLiteralData.params) {
                    gen(param->paramData.value);
                }

                for (const auto &param : node->structLiteralData.params) {
                    auto paramSize = typeSize(param->paramData.value->typeInfo);
                    auto paramAlign = typeAlign(param->paramData.value->typeInfo);

                    // alignment
                    if (sizeSoFar > 0 && paramAlign > 0) {
                        sizeSoFar += sizeSoFar % paramAlign;
                    }

                    storeValue(param->paramData.value, offset + sizeSoFar);

                    sizeSoFar += paramSize;
                }
            }
        } break;
        case NodeType::ARRAY_LITERAL: {
            storeValue(node->arrayLiteralData.structLiteralRepresentation, offset);
        } break;
        case NodeType::STRING_LITERAL: {
            auto savedForcing = this->forcing;
            this->forcing = true;

            storeValue(node->stringLiteralData.arrayLiteralRepresentation, offset);

            this->forcing = savedForcing;
        } break;
        case NodeType::UNARY_NEG: {
            storeValue(node->unaryNegData.rewritten, offset);
        } break;
        case NodeType::ALIAS: {
            storeValue(node->declData.initialValue, offset);
        } break;
        case NodeType::ENUM_LITERAL: {
            storeValue(node->enumLiteralData.value, offset);
        } break;
        default:
            cpi_assert(false);
    }
}

BytecodeGen::BytecodeGen() {
    fnTable = hash_init<uint32_t, uint64_t>(100);
    fixups = vector_init<Fixup>(512);
}
