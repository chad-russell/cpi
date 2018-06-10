#include "interpreter.h"

#include <iostream>
#include <string.h>
#include <algorithm>

using namespace std;

void printStmt(Interpreter *interp, int32_t pcStmtStart, bool withLineInfo = false) {
    for (auto stmt : interp->sourceMap.statements) {
        if (stmt.instIndex == pcStmtStart) {
            // get byte corresponding to start
            auto startByte = stmt.startByte;

            // get byte corresponding to end
            auto endByte = stmt.endByte;

            if (withLineInfo) {
                cout << "[" << stmt.startLine << "] ";
            }
            cout << interp->sourceMap.sourceInfo.source.substr(startByte, endByte - startByte) << endl;
        }
    }
}

void printCurrentStmt(Interpreter *interp, bool withLineInfo = false) {
    printStmt(interp, interp->pc, withLineInfo);
}

void Interpreter::interpret() {
    auto mp = new MnemonicPrinter(this->instructions);

    while (pc < instructions.size() && !terminated) {

        bool shouldStop = false;
        if (debugFlag) {
            auto stmtStop = false;
            for (auto&& s : sourceMap.statements) {
                if (s.instIndex == pc) { stmtStop = true; }
            }

            if (stmtStop) {
                lastStmtPc = pc;
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
                } else if (startsWith(line, "break ")) {
                    int32_t bNum;
                    sscanf(line.c_str(), "break %d", &bNum);

                    // find the statement which is on this line
                    for (auto stmt : sourceMap.statements) {
                        if (stmt.startLine == bNum) {
                            breakpoints.push_back(stmt.instIndex);
                        }
                    }
                } else if (line == "breakRemove") {
                    breakpoints = {};
                } else if (line == "location") {
                    for (auto stmt : sourceMap.statements) {
                        if (stmt.instIndex == pc) {
                            cout << stmt.startLine << endl;
                        }
                    }
                } else if (line == "stmt" || line == "line") {
                    printCurrentStmt(this);
                } else if (line == "asm") {
                    // print all insts between this stmt and the next one
                    auto firstIndex = (uint32_t) pc;
                    auto lastIndex = (uint32_t) pc;
                    for (auto statement : sourceMap.statements) {
                        auto stmt = statement;
                        if (stmt.instIndex == pc) {
                            lastIndex = (int32_t) statement.instEndIndex;
                            break;
                        }
                    }

                    cout << mp->debugString(firstIndex, lastIndex);
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
            step();
        }
    }

    delete mp;
}

void Interpreter::step() {
    stepCount++;

    unsigned long inst = instructions[pc];
    pc += 1;
    table.at(inst)(this);
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

// calli
void interpretCalli(Interpreter *interp) {
    auto fnTableIndex = interp->read<int32_t>();
    auto callIndex = interp->fnTable[fnTableIndex];
    interp->callIndex(callIndex);
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

    auto maybeReadOffset = interp->tryRead<int32_t>();
    assert(maybeReadOffset.isPresent);
    auto readOffset = maybeReadOffset.value;

//    if (readOffset > 5000 || readOffset < 0) {
//        cout << "storing from read offset -- " << readOffset << endl;
//    }

    auto size = interp->consume<int32_t>();

    memcpy(&interp->stack[storeOffset], &interp->stack[readOffset], static_cast<size_t>(size));

//    auto debugValue = *((int64_t *) (&interp->stack[storeOffset]));
//    cout << "stored " << debugValue << " to " << storeOffset << " (from " << readOffset << ")" << endl;
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
