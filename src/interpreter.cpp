#include "interpreter.h"

#include <iostream>

using namespace std;

void Interpreter::interpret() {
    while (pc < instructions.size() && !terminated) {
        stepCount -= 1;

        auto found = find(breakpoints.begin(), breakpoints.end(), pc);
        if (stepCount == 0 || found < breakpoints.end()) {
            stepCount = 1;

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
                            breakpoints.push_back(bNum);
                        } else if (line == "breakRemove") {
                            breakpoints = {};
                        } else if (line == "inst") {
                            if (sourceMap.size() > 0) {
                                auto index = 0;
                                while (index + 5 < sourceMap.size()) {
                                    if (sourceMap[index + 4] == pc) {

                                        // get byte corresponding to start
                                        auto startByte = srcInfo.lines[sourceMap[index]] +
                                                         sourceMap[index + 1];

                                        // get byte corresponding to end
                                        auto endByte = srcInfo.lines[sourceMap[index + 2]] +
                                                       sourceMap[index + 3];

                                        cout << srcInfo.source.substr(startByte, endByte - startByte) << endl;
                                    }

                                    index += 5;
                                }
                            } else {
                                cout << AssemblyLexer::instructionStrings[instructions[pc]] << endl;
                            }
                        } else if (line == "lines") {
                            auto index = 0;

                            vector<unsigned long> linesToPrint;

                            while (index + 5 < sourceMap.size()) {
                                if (sourceMap[index + 4] == pc) {
                                    linesToPrint.push_back(sourceMap[index]);
                                }
                                index += 5;
                            }

                            if (linesToPrint.size() == 0) {
                                break;
                            }

                            auto endLine = linesToPrint.at(linesToPrint.size() - 1) + 3;
                            if (endLine > srcInfo.lines.size() - 1) {
                                endLine = srcInfo.lines.size() - 1;
                            }

                            for (auto l = linesToPrint[0]; l < endLine; l++) {
                                // get byte corresponding to start
                                auto startByte = srcInfo.lines[l];

                                // get byte corresponding to end
                                unsigned long endByte;
                                if (srcInfo.lines.size() < l + 1) {
                                    endByte = srcInfo.source.size();
                                } else {
                                    endByte = srcInfo.lines[l + 1] - 1;
                                }

                                cout << srcInfo.source.substr(startByte, endByte - startByte) << endl;
                            }

                            index += 5;
                        } else if (line == "continue") {
                            stepCount = -1;
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
}

void Interpreter::step() {
    int32_t inst = instructions[pc];
    auto debugInst = static_cast<Instruction>(inst);
    pc += 1;

    table.at(inst)(this);
}

void interpretExit(Interpreter *interp) {
    interp->terminated = true;
}

// calli
void interpretCalli(Interpreter *interp) {
    interp->callIndex(interp->read<int32_t>());
}

// call
void interpretCall(Interpreter *interp) {
    interp->callIndex(interp->consume<int32_t>());
}

void Interpreter::callIndex(int index) {
    pushToStack(bp);
    pushToStack(pc);
    bp = sp;

    pc = index;
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
}

// jumpif
void interpretJumpIf(Interpreter *interp) {
    bool cond;

    auto constCond = interp->tryRead<int8_t>();
    if (constCond.isPresent) {
        cond = constCond.value == 1;
    } else {
        cond = interp->read<int8_t>() == 1;
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

// e.g. (store 0 i32 3)
void interpretStore(Interpreter *interp) {
    auto storeOffset = interp->consume<int32_t>();

    switch (static_cast<int>(interp->instructions.at(interp->pc))) {
    case static_cast<int>(Instruction::I8):
    case static_cast<int>(Instruction::CONSTI8): {
        auto value = interp->read<int8_t>();
        interp->copyToStack(value, interp->bp + storeOffset);
    } break;
    case static_cast<int>(Instruction::I16):
    case static_cast<int>(Instruction::CONSTI16): {
        auto value = interp->read<int16_t>();
        interp->copyToStack(value, interp->bp + storeOffset);
    } break;
    case static_cast<int>(Instruction::I32):
    case static_cast<int>(Instruction::CONSTI32): {
        auto value = interp->read<int32_t>();
        interp->copyToStack(value, interp->bp + storeOffset);
    } break;
    case static_cast<int>(Instruction::I64):
    case static_cast<int>(Instruction::CONSTI64): {
        auto value = interp->read<int64_t>();
        interp->copyToStack(value, interp->bp + storeOffset);
    } break;
    case static_cast<int>(Instruction::F32):
    case static_cast<int>(Instruction::CONSTF32): {
        auto value = interp->read<float>();
        interp->copyToStack(value, interp->bp + storeOffset);
    } break;
    case static_cast<int>(Instruction::F64):
    case static_cast<int>(Instruction::CONSTF64): {
        auto value = interp->read<double>();
        interp->copyToStack(value, interp->bp + storeOffset);
    } break;
    default:
        assert(false);
    }
}