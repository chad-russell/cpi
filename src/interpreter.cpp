#include "interpreter.h"
#include "node.h"

#include <iostream>
#include <string.h>
#include <algorithm>
#include <sstream>

using namespace std;

void printStmt(Interpreter *interp, int32_t pcStmtStart, bool withLineInfo = false) {
    for (auto stmt : interp->sourceMap.statements) {
        if (stmt.instIndex == (unsigned long) pcStmtStart) {
            cout << interp->sourceMap.sourceInfo.source->substr(stmt.startByte, stmt.endByte - stmt.startByte);

            if (withLineInfo) {
                cout << "[" << stmt.startLine << "] ";
            }

            cout << endl;
        }
    }
}

void printCurrentStmt(Interpreter *interp, bool withLineInfo = false) {
    printStmt(interp, interp->pc, withLineInfo);
}

void debugPrintVar(ostream &target, Interpreter *interp, int32_t bp, TypeData td, int32_t offset, vector<string> &extraLines);

void debugPrintVar(Interpreter *interp, int32_t bp, TypeData td, int32_t offset, vector<string> &extraLines) {
    debugPrintVar(cout, interp, bp, td, offset, extraLines);
}

void debugPrintVar(ostream &target, Interpreter *interp, int32_t bp, TypeData td, int32_t offset, vector<string> &extraLines) {
    switch (td.kind) {
        case NodeTypekind::NONE: {
            target << "{}";
        };
        case NodeTypekind::INT_LITERAL: {
            target << td.intTypeData;
        } break;
        case NodeTypekind::I8: {
            target << interp->readFromStack<char>(offset);
        } break;
        case NodeTypekind::I32: {
            target << interp->readFromStack<int32_t>(offset);
        } break;
        case NodeTypekind::I64: {
            target << interp->readFromStack<int64_t>(offset);
        } break;
        case NodeTypekind::FLOAT_LITERAL: {
            target << td.floatTypeData;
        } break;
        case NodeTypekind::BOOLEAN_LITERAL: {
            target << td.boolTypeData;
        } break;
        case NodeTypekind::BOOLEAN: {
            target << (interp->readFromStack<int32_t>(offset) == 1 ? "true" : "false");
        } break;
        case NodeTypekind::F32: {
            target << interp->readFromStack<float>(offset);
        } break;
        case NodeTypekind::F64: {
            target << interp->readFromStack<double>(offset);
        } break;
        case NodeTypekind::POINTER: {
            target << "0x" << hex << (int64_t) (interp->stack.data()) + interp->readFromStack<int32_t>(offset) << dec;
        } break;
        case NodeTypekind::FN: {
            target << interp->readFromStack<int32_t>(offset) << " (todo(chad): look up the fn name)";
        } break;
        case NodeTypekind::STRUCT: {
            if (td.structTypeData.isSecretlyEnum) {
                auto tag = interp->readFromStack<int64_t>(offset);

                auto param = vector_at(td.structTypeData.params, (unsigned long) tag);
                assert(param->type == NodeType::DECL_PARAM);

                target << "{" << atomTable->backwardAtoms[param->paramData.name->symbolData.atomId] << ": ";
                debugPrintVar(interp, bp, resolve(param->typeInfo)->typeData, offset + 8, extraLines);
                target << "}";
            }
            else if (td.structTypeData.isSecretlyArray) {
                auto resolvedElementType = resolve(td.structTypeData.secretArrayElementType);

                auto size = interp->readFromStack<int32_t>(offset + 8);
                auto ts = typeSize(td.structTypeData.secretArrayElementType);
                auto array_offset = bp + interp->readFromStack<int64_t>(offset);

                if (resolvedElementType->typeData.kind == NodeTypekind::I8) {
                    target << "\"";
                    for (int32_t i = 0; i < size; i++) {
                        target << interp->readFromStack<char>((int32_t) array_offset);
                        array_offset += ts;
                    }
                    target << "\"";
                }
                else {
                    target << "[]{";
                    for (int32_t i = 0; i < size; i++) {
                        debugPrintVar(interp, bp, resolve(td.structTypeData.secretArrayElementType)->typeData, (int32_t) array_offset, extraLines);
                        array_offset += ts;

                        if (i < size - 1) {
                            target << " ";
                        }
                    }
                    target << "}";
                }
            }
            else {
                auto nvr = interp->nextVarReference;
                interp->nextVarReference += 1;

                target << "#" << nvr;

                // plain old struct
                ostringstream extra("");
                extra << "#" << nvr << ": ";
                extra << "{";
                auto sizeSoFar = 0;
                unsigned long idx = 0;

                for (const auto &param : td.structTypeData.params) {
                    auto name = "_" + to_string(idx);
                    if (param->paramData.name != nullptr) {
                        name = atomTable->backwardAtoms[param->paramData.name->symbolData.atomId];
                    }
                    extra << name << ":";

                    debugPrintVar(extra, interp, bp, param->typeInfo->typeData, offset + sizeSoFar, extraLines);
                    if (idx < td.structTypeData.params.length - 1) {
                        extra << " ";
                    }

                    sizeSoFar += typeSize(param->typeInfo);
                    idx += 1;
                }
                extra << "}";

                extraLines.push_back(extra.str());
            }
        } break;
        default: assert(false);
    }
}

void debugPrintVar(Interpreter *interp, int32_t bp, Node *n) {
    assert(n->isLocal || n->isBytecodeLocal);

    auto resolvedTypeinfo = resolve(resolve(n)->typeInfo);

    vector<string> extra;
    debugPrintVar(interp, bp, resolvedTypeinfo->typeData, bp + n->localOffset, extra);
    cout << endl;
    for (const auto &e : extra) {
        cout << e << endl;
    }
}

void printCurrentVars(Interpreter *interp, int32_t bp, uint32_t pc) {
    for (auto stmt : interp->sourceMap.statements) {
        if (stmt.instIndex == (unsigned long) pc) {
            auto node = stmt.node;
            if (node == nullptr) {
                continue;
            }

            // hack
            if (node->type == NodeType::FN_DECL && node->fnDeclData.body.length > 0) {
                node = vector_at(node->fnDeclData.body, 0);
            }

            for (auto i = 0; i < node->scope->symbols->bucket_count; i++) {
                auto bucket = node->scope->symbols->buckets[i];
                if (bucket != nullptr) {
                    if (bucket->value->isLocal || bucket->value->isBytecodeLocal) {
                        cout << atomTable->backwardAtoms[bucket->key] << ": ";
                        debugPrintVar(interp, bp, bucket->value);
                    }

                    while (bucket->next != nullptr) {
                        bucket = bucket->next;

                        if (bucket->value->isLocal || bucket->value->isBytecodeLocal) {
                            cout << atomTable->backwardAtoms[bucket->key] << ": ";
                            debugPrintVar(interp, bp, bucket->value);
                        }
                    }
                }
            }
        }
    }
}

void printInfo(Interpreter *interp, int32_t bp, uint32_t pc) {
    // next: one line per vairable
    printCurrentVars(interp, bp, pc);
}

void Interpreter::interpret() {
    auto mp = new MnemonicPrinter(this->instructions);

    while ((unsigned long) pc < instructions.size() && !terminated) {

        bool shouldStop = false;
        if (debugFlag) {
            auto stmtStop = false;
            for (auto&& s : sourceMap.statements) {
                if (s.instIndex == (unsigned long) pc) {
                    stmtStop = true;
                }
            }

            auto breakStopIf = find(breakpoints.begin(), breakpoints.end(), pc);
            auto breakStop = breakStopIf != breakpoints.end();

            shouldStop = (stmtStop && !continuing) || breakStop;
        }

        while (shouldStop && !terminated && depth < overDepth) {
            continuing = false;
            overDepth = (2 << 15) + 1;

//            printCurrentStmt(this);
//            cout << "> ";

            lastValidPc = pc;

            string line;
            getline(cin, line);
            while (line != "" && !terminated) {
                if (line == "stack") {
                    cout << "[";
                    for (auto i = 0; i < sp; i++) {
                        cout << static_cast<int32_t>(stack[i]);
                        if (i < sp - 1) { cout << ", "; }
                    }
                    cout << "]";
                    cout << endl;
                } else if (line == "frame") {
                    cout << "[";
                    for (auto i = bp; i < sp; i++) {
                        cout << static_cast<int32_t>(stack[i]);
                        if (i < sp - 1) { cout << ", "; }
                    }
                    cout << "]";
                    cout << endl;
                } else if (startsWith(&line, "break ")) {
                    int32_t bNum;
                    sscanf(line.c_str(), "break %d", &bNum);

                    // find the statement which is on this line
                    for (auto stmt : sourceMap.statements) {
                        if (stmt.startLine == (unsigned long) bNum) {
                            breakpoints.push_back(stmt.instIndex);
                        }
                    }
                } else if (line == "breakRemove") {
                    breakpoints = {};
                } else if (line == "location") {
                    for (auto stmt : sourceMap.statements) {
                        if (stmt.instIndex == (unsigned long) pc) {
                            cout << stmt.startLine << endl;
                        }
                    }
                } else if (line == "info") {
                    this->nextVarReference = 1;

                    cout << this->depth + 1 << endl;

                    auto bp = this->bp;
                    auto pc = this->pc;

                    for (uint16_t i = 0; i < this->depth + 1; i++) {
                        // line 1: location
                        for (auto stmt : this->sourceMap.statements) {
                            if (stmt.instIndex == (unsigned long) pc) {
                                cout << stmt.startLine << endl;
                            }
                        }

                        printInfo(this, bp, pc);
                        bp = this->readFromStack<int32_t>(bp - 8);

                        if (i < this->depth) {
                            pc = (uint32_t) this->pcs.at(this->pcs.size() - 1 - i);
                            cout << "---" << endl;
                        }
                    }
                } else if (line == "stmt") {
                    printCurrentStmt(this);
                } else if (line == "asm") {
                    // print all insts between this stmt and the next one
                    auto firstIndex = pc;
                    auto lastIndex = pc;
                    for (auto statement : sourceMap.statements) {
                        auto stmt = statement;
                        if (stmt.instIndex == (unsigned long) pc) {
                            lastIndex = (int32_t) statement.instEndIndex;
                            break;
                        }
                    }

                    cout << mp->debugString(firstIndex, lastIndex);
                } else if (line == "vars") {
                    printCurrentVars(this, this->bp, this->pc);
                } else if (line == "step") {
                    shouldStop = false;
                    break;
                } else if (line == "over") {
                    auto instAt = table.at(instructions[pc]);
                    if (instAt == interpretCall || instAt == interpretCalli) {
                        overDepth = depth + 1;
                    }
                    shouldStop = false;
                    break;
                } else if (line == "out") {
                    // prevent accidentally stepping 'out' of main fn
                    if (depth > 0) {
                        overDepth = depth;
                    }
                    shouldStop = false;
                    break;
                } else if (line == "continue") {
                    shouldStop = false;
                    continuing = true;
                    break;
                } else if (line == "terminate" || line == "q" || line == "quit") {
                    terminated = true;
                    break;
                } else {
                    cout << "unrecognized command" << endl;
                }

//                cout << "> ";
                getline(cin, line);
            }
        }

        if (!terminated) {
            stepCount++;
            table.at(instructions[pc++])(this);
        }
    }

    delete mp;
}

void interpretExit(Interpreter *interp) {
    interp->terminated = true;
}

void interpretPanic(Interpreter *interp) {
    cout << "assertion failed!!!" << endl;
//    printStmt(interp, interp->lastStmtPc, true);
    exit(0);
}

void interpretMalloc(Interpreter *interp) {
    auto numBytes = interp->read<int64_t>();
    auto storeOffset = interp->consume<int32_t>();

//    cout << "allocating " << numBytes << " bytes...";

    auto allocated = malloc(static_cast<size_t>(numBytes));
    auto offset_from_stack = static_cast<int64_t>((int8_t *) allocated - (int8_t *) interp->stack.data());
//    cout << " offset -- " << offset_from_stack << " ...";
    interp->copyToStack(offset_from_stack, interp->bp + storeOffset);

//    cout << " done!" << endl;
}

void interpretFree(Interpreter *interp) {
    auto offset_from_stack = interp->read<int64_t>();
    auto ptr_to_free = (int8_t *) interp->stack.data() + offset_from_stack;
    free(ptr_to_free);
}

// puts
void interpretPuts(Interpreter *interp) {
    auto offset_from_stack = interp->read<int32_t>();
    auto ptr_to_offset = (int32_t *) ((int8_t *) interp->stack.data() + offset_from_stack);
    auto followed_ptr = (char *) ((int8_t *) interp->stack.data() + *ptr_to_offset);

    auto ptr_to_count = (int32_t *) ((int8_t *) interp->stack.data() + offset_from_stack + 8);
    auto count = static_cast<size_t>(*ptr_to_count);

    fwrite(followed_ptr, sizeof(char), count, stdout);
}

// calli
void interpretCalli(Interpreter *interp) {
    auto fnTableIndex = interp->read<int32_t>();
    auto callIndex = *hash_get(interp->fnTable, (uint32_t) fnTableIndex);
    interp->callIndex((int32_t) callIndex);
}

// call
void interpretCall(Interpreter *interp) {
    interp->callIndex((uint32_t) interp->consume<int32_t>());
}

void Interpreter::callIndex(uint32_t index) {
    depth += 1;

    pcs.push_back(lastValidPc);

    pushToStack(bp);
    pushToStack(pc);
    bp = sp;

    pc = index;
}

void Interpreter::dumpStack() {
    cout << "INTERP STACK:" << endl;
    for (auto i = 0; i < 64; i++) {
        cout << static_cast<int32_t>(stack[i]) << ", ";
    }
    cout << endl << endl;
}

void interpretMathAddSI64(Interpreter *interp) {
    auto a = interp->read<int64_t>();
    auto b = interp->read<int64_t>();
    auto c = interp->consume<int32_t>();
    auto result = a + b * c;
    auto storeOffset = interp->consume<int32_t>();
    interp->copyToStack(result, interp->bp + storeOffset);
}

// bumpsp numBits
void interpretBumpSP(Interpreter *interp) {
    auto numBytes = interp->consume<int32_t>();
    interp->sp += numBytes;
}

// ret
void interpretReturn(Interpreter *interp) {
    interp->sp = interp->bp - 8;
    interp->pc = interp->readFromStack<int32_t>(interp->bp - 4);
    interp->bp = interp->readFromStack<int32_t>(interp->bp - 8);
    interp->depth -= 1;

    interp->pcs.pop_back();
}

// jumpif
void interpretJumpIf(Interpreter *interp) {
    auto constCond = interp->read<int32_t>();

    auto trueInst = interp->read<int32_t>();
    auto falseInst = interp->read<int32_t>();

    if (constCond == 1) {
        interp->pc = trueInst;
    } else {
        interp->pc = falseInst;
    }
}

// jump
void interpretJump(Interpreter *interp) {
    auto index = (uint32_t)interp->consume<int32_t>();

    interp->pc = index;
}

void interpretStore(Interpreter *interp) {
    auto storeOffset = interp->read<int32_t>();

    auto maybeReadOffset = interp->read<int32_t>();
    auto readOffset = maybeReadOffset;

    auto size = interp->consume<int32_t>();

    memcpy(&interp->stack[storeOffset], &interp->stack[readOffset], static_cast<size_t>(size));

//    if (size == 4) {
//        auto debugValue = *((int32_t *) (&interp->stack[storeOffset]));
//        cout << "stored " << debugValue << " to " << storeOffset << " (from " << readOffset << ")" << endl;
//    }
//    else if (size == 8) {
//        auto debugValue = *((int64_t *) (&interp->stack[storeOffset]));
//        cout << "stored " << debugValue << " to " << storeOffset << " (from " << readOffset << ")" << endl;
//    }
//    else {
//        cout << "stored something";
//    }
}

void interpretStoreConst(Interpreter *interp) {
    auto storeOffset = interp->read<int32_t>();

    if (storeOffset >= interp->stackSize) {
        cout << "STACK OVERFLOW!!!!" << endl;
        exit(1);
    }

    auto inst = static_cast<Instruction>(interp->instructions[interp->pc]);
    auto instStr = AssemblyLexer::instructionStrings[interp->instructions[interp->pc]];

    interp->pc += 1;
    if (inst == Instruction::CONSTI8) {
        int8_t value = interp->consume<int8_t>();
        memcpy(&interp->stack[storeOffset], &value, sizeof(int8_t));
    } else if (inst == Instruction::CONSTI16) {
        int16_t value = interp->consume<int16_t>();
        memcpy(&interp->stack[storeOffset], &value, sizeof(int16_t));
    } else if (inst == Instruction::CONSTI32 || inst == Instruction::RELCONSTI32) {
        int32_t value = interp->consume<int32_t>();
        if (inst == Instruction::RELCONSTI32) {
            value += interp->bp;
        }
        memcpy(&interp->stack[storeOffset], &value, sizeof(int32_t));
    } else if (inst == Instruction::CONSTI64) {
        int64_t value = interp->consume<int64_t>();
        memcpy(&interp->stack[storeOffset], &value, sizeof(int64_t));
    } else if (inst == Instruction::CONSTF32) {
        auto value = interp->consume<float>();
        memcpy(&interp->stack[storeOffset], &value, sizeof(float));
    } else if (inst == Instruction::CONSTF64) {
        auto value = interp->consume<double>();
        memcpy(&interp->stack[storeOffset], &value, sizeof(double));
    } else {
        assert(false);
    }
}
