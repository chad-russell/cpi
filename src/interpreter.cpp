#include "interpreter.h"

#include <iostream>
#include <string.h>

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
    auto mp = new MnemonicPrinter();
    mp->instructions = this->instructions;

    while (pc < instructions.size() && !terminated) {
        auto stmtStopIf = find_if(sourceMap.statements.begin(), sourceMap.statements.end(),
                                  [this](const SourceMapStatement &s) { return s.instIndex == pc; });
        auto stmtStop = stmtStopIf != sourceMap.statements.end();
        if (stmtStop) {
            lastStmtPc = pc;
        }

        auto breakStopIf = find(breakpoints.begin(), breakpoints.end(), pc);
        auto breakStop = breakStopIf != breakpoints.end();

        auto shouldStop = (stmtStop && !continuing) || breakStop;

        auto srcInfo = sourceMap.sourceInfo;

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
    unsigned long inst = instructions[pc];
    pc += 1;
    table.at(inst)(this);
}

void interpretExit(Interpreter *interp) {
    interp->terminated = true;
}

void interpretAssert(Interpreter *interp) {
    auto condition = interp->read<int32_t>();
    if (condition == 0) {
        cout << "ASSERTION FAILURE!!!!" << endl << "> ";
        printStmt(interp, interp->lastStmtPc, true);
        exit(0);
    }
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
    bool cond;

    auto constCond = interp->tryRead<int32_t>();
    if (constCond.isPresent) {
        cond = constCond.value == 1;
    } else {
        cond = interp->read<int32_t>() == 1;
    }

    auto trueInst = interp->consume<int32_t>();
        auto falseInst = interp->consume<int32_t>();

        if (cond) {
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
    auto maybeStoreOffset = interp->tryRead<int32_t>();
    assert(maybeStoreOffset.isPresent);
    auto storeOffset = maybeStoreOffset.value;

    if (storeOffset >= interp->stack.size()) {
        cout << "STACK OVERFLOW!!!!" << endl;
        exit(1);
    }

    auto maybeReadOffset = interp->tryRead<int32_t>();
    assert(maybeReadOffset.isPresent);
    auto readOffset = maybeReadOffset.value;

    auto size = interp->consume<int32_t>();

    memcpy(&interp->stack[storeOffset], &interp->stack[readOffset], static_cast<size_t>(size));
}

void interpretStoreConst(Interpreter *interp) {
    auto storeOffset = interp->read<int32_t>();

    if (storeOffset >= interp->stack.size()) {
        cout << "STACK OVERFLOW!!!!" << endl;
        exit(1);
    }

    auto inst = AssemblyLexer::instructionStrings[interp->instructions[interp->pc]];

    interp->pc += 1;
    if (endsWith(inst, "CONSTI8")) {
        int8_t value = interp->consume<int8_t>();
        memcpy(&interp->stack[storeOffset], &value, sizeof(int8_t));
    } else if (endsWith(inst, "CONSTI16")) {
        int16_t value = interp->consume<int16_t>();
        memcpy(&interp->stack[storeOffset], &value, sizeof(int16_t));
    } else if (endsWith(inst, "CONSTI32")) {
        int32_t value = interp->consume<int32_t>();
        if (startsWith(inst, "REL")) {
            value += interp->bp;
        }
        memcpy(&interp->stack[storeOffset], &value, sizeof(int32_t));
    } else if (endsWith(inst, "CONSTI64")) {
        int64_t value = interp->consume<int64_t>();
        memcpy(&interp->stack[storeOffset], &value, sizeof(int64_t));
    } else {
        assert(false);
    }
}
