#include "interpreter.h"
#include "node.h"

#include <iostream>
#include <string.h>
#include <algorithm>

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

void debugPrintVar(Interpreter *interp, TypeData td, int32_t offset) {
    switch (td.kind) {
        case NodeTypekind::NONE: {
            cout << "{}";
        };
        case NodeTypekind::INT_LITERAL: {
            cout << td.intTypeData;
        } break;
        case NodeTypekind::I8: {
            cout << interp->readFromStack<char>(offset);
        } break;
        case NodeTypekind::I32: {
            cout << interp->readFromStack<int32_t>(offset);
        } break;
        case NodeTypekind::I64: {
            cout << interp->readFromStack<int64_t>(offset);
        } break;
        case NodeTypekind::FLOAT_LITERAL: {
            cout << td.floatTypeData;
        } break;
        case NodeTypekind::BOOLEAN_LITERAL: {
            cout << td.boolTypeData;
        } break;
        case NodeTypekind::BOOLEAN: {
            cout << (interp->readFromStack<int32_t>(offset) == 1 ? "true" : "false");
        } break;
        case NodeTypekind::F32: {
            cout << interp->readFromStack<float>(offset);
        } break;
        case NodeTypekind::F64: {
            cout << interp->readFromStack<double>(offset);
        } break;
        case NodeTypekind::POINTER: {
            cout << interp->readFromStack<int32_t>(offset);
        } break;
        case NodeTypekind::FN: {
            cout << interp->readFromStack<int32_t>(offset) << " (todo(chad): look up the fn name)";
        } break;
        case NodeTypekind::STRUCT: {
            if (td.structTypeData.isSecretlyEnum) {
                cout << "todo(chad): enums";
            }
            else if (td.structTypeData.isSecretlyArray) {
                cout << "todo(chad): arrays";
            }
            else {
                // plain old struct
                cout << "{";
                auto sizeSoFar = 0;
                auto idx = 0;

                for (const auto &param : td.structTypeData.params) {
                    debugPrintVar(interp, param->typeInfo->typeData, offset + sizeSoFar);
                    if (idx < td.structTypeData.params.length - 1) {
                        cout << ", ";
                    }

                    sizeSoFar += typeSize(param->typeInfo);
                    idx += 1;
                }
                cout << "}";
            }
        } break;
        default: {
            cout << "unsupported typekind " << td.kind << endl;
        } break;
    }
}

void debugPrintVar(Interpreter *interp, Node *n) {
    if (!n->isLocal || n->isBytecodeLocal) {
        cout << "no info";
        return;
    }

    auto resolvedTypeinfo = resolve(resolve(n)->typeInfo);
    debugPrintVar(interp, resolvedTypeinfo->typeData, n->localOffset);
}

void printCurrentVars(Interpreter *interp) {
    for (auto stmt : interp->sourceMap.statements) {
        if (stmt.instIndex == (unsigned long) interp->pc) {
            auto node = stmt.node;
            if (node == nullptr) {
                cout << "ERROR: no debug info";
                return;
            }

            for (auto i = 0; i < node->scope->symbols->bucket_count; i++) {
                auto bucket = node->scope->symbols->buckets[i];
                if (bucket != nullptr) {
                    cout << atomTable->backwardAtoms[bucket->key] << ": ";
                    debugPrintVar(interp, bucket->value);
                    cout << endl;

                    while (bucket->next != nullptr) {
                        bucket = bucket->next;
                        cout << atomTable->backwardAtoms[bucket->key] << ": ";
                        debugPrintVar(interp, bucket->value);
                        cout << endl;
                    }
                }
            }
        }
    }
}

void Interpreter::interpret() {
    auto mp = new MnemonicPrinter(this->instructions);

    while ((unsigned long) pc < instructions.size() && !terminated) {

        bool shouldStop = false;
        if (debugFlag) {
            auto stmtStop = false;
            for (auto&& s : sourceMap.statements) {
                if (s.instIndex == (unsigned long) pc) { stmtStop = true; }
            }

            if (stmtStop) {
            }

            auto breakStopIf = find(breakpoints.begin(), breakpoints.end(), pc);
            auto breakStop = breakStopIf != breakpoints.end();

            shouldStop = (stmtStop && !continuing) || breakStop;
        }

        while (shouldStop && !terminated && depth < overDepth) {
            continuing = false;
            overDepth = (2 << 15) + 1;

            printCurrentStmt(this);
            cout << "> ";

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
                } else if (line == "stmt" || line == "line") {
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
                    printCurrentVars(this);
                } else if (line == "step") {
                    shouldStop = false;
                    break;
                } else if (line == "over") {
                    overDepth = depth + 1;
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

                cout << "> ";
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
    interp->callIndex(interp->consume<int32_t>());
}

void Interpreter::callIndex(int index) {
    depth += 1;

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
void interpretRet(Interpreter *interp) {
    interp->sp = interp->bp - 8;
    interp->pc = interp->readFromStack<int32_t>(interp->bp - 4);
    interp->bp = interp->readFromStack<int32_t>(interp->bp - 8);
    interp->depth -= 1;
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
    auto index = interp->consume<int32_t>();
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
