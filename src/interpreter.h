#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <vector>
#include <stack>
#include <dlfcn.h>

#include "assembler.h"

class Interpreter;

template <typename T>
void interpretMathAdd(Interpreter *interp);

void interpretMathAddSI64(Interpreter *interp);

template <typename T>
void interpretMathSub(Interpreter *interp);

template <typename T>
void interpretMathMul(Interpreter *interp);

template <typename T>
void interpretMathDiv(Interpreter *interp);

template <typename T>
void interpretMathMod(Interpreter *interp);

template <typename T>
void interpretCmpEq(Interpreter *interp);

template <typename T>
void interpretCmpNeq(Interpreter *interp);

template <typename T>
void interpretCmpGt(Interpreter *interp);

template <typename T>
void interpretCmpGte(Interpreter *interp);

template <typename T>
void interpretCmpLt(Interpreter *interp);

template <typename T>
void interpretCmpLte(Interpreter *interp);

void interpretCalli(Interpreter *interp);
void interpretCalle(Interpreter *interp);
void interpretCall(Interpreter *interp);
void interpretBumpSP(Interpreter *interp);
void interpretReturn(Interpreter *interp);
void interpretJumpIf(Interpreter *interp);
void interpretJump(Interpreter *interp);
void interpretStore(Interpreter *interp);
void interpretStoreConst(Interpreter *interp);
void interpretExit(Interpreter *interp);
void interpretPanic(Interpreter *interp);
void interpretPuts(Interpreter *interp);
void interpretNop(Interpreter *interp);
void interpretNot(Interpreter *interp);
void interpretBitNot(Interpreter *interp);
void interpretConvert(Interpreter *interp);
void interpretMathBitwiseAnd(Interpreter *interp);
void interpretMathBitwiseOr(Interpreter *interp);
void interpretMathBitwiseXor(Interpreter *interp);
void interpretMathBitwiseShl(Interpreter *interp);
void interpretMathBitwiseshr(Interpreter *interp);

class Interpreter {
public:
    vector<unsigned char> instructions = {};
    hash_t<uint32_t, uint64_t> *fnTable;

    vector<unsigned char> stack = {};
    int32_t stackSize = 1024;

    uint64_t stepCount = 0;

    uint32_t pc = 0;
    int32_t sp = 0;
    int32_t bp = 0;

    uint32_t nextVarReference = 1;
    hash_t<int64_t, string> *pointerRecursion;

    vector<uint32_t> pcs = {};
    uint32_t lastValidPc = 0;

    typedef void (*interpFuncType)(Interpreter *);
    vector<interpFuncType> table;

    bool terminated = false;

    SourceMap sourceMap;
    vector<unsigned long> breakpoints = {};
    bool continuing = false;
    SourceMapStatement stoppedOnStatement;

    vector_t<Node *> externalFnTable;
    vector_t<void *> libs;

    // used for stepping 'over' functions (as opposed to normal step which goes 'into')
    uint16_t depth = 0;
    int32_t overDepth = (2 << 15) + 1;

    Interpreter(vector_t<string *> externalLibs): Interpreter(2048 * 64, externalLibs) {}

    Interpreter(int32_t stackSize_, vector_t<string *> externalLibs) {
        this->stackSize = stackSize_;

        stack.reserve((unsigned long) stackSize);
        for (auto i = 0; i < stackSize; i++) {
            stack.push_back(0);
        }

        this->fnTable = hash_init<uint32_t, uint64_t>(100);

        table = {
                interpretMathAdd<int8_t>,
                interpretMathSub<int8_t>,
                interpretMathMul<int8_t>,
                interpretMathDiv<uint8_t>,
                interpretMathDiv<int8_t>,
                interpretMathMod<uint8_t>,
                interpretMathMod<int8_t>,
                interpretCmpEq<int8_t>,
                interpretCmpNeq<int8_t>,
                interpretCmpGt<uint8_t>,
                interpretCmpGt<int8_t>,
                interpretCmpGte<uint8_t>,
                interpretCmpGte<int8_t>,
                interpretCmpLt<uint8_t>,
                interpretCmpLt<int8_t>,
                interpretCmpLte<uint8_t>,
                interpretCmpLte<int8_t>,
                interpretMathAdd<int16_t>,
                interpretMathSub<int16_t>,
                interpretMathMul<int16_t>,
                interpretMathDiv<uint16_t>,
                interpretMathDiv<int16_t>,
                interpretMathMod<uint16_t>,
                interpretMathMod<int16_t>,
                interpretCmpEq<int16_t>,
                interpretCmpNeq<int16_t>,
                interpretCmpGt<uint16_t>,
                interpretCmpGt<int16_t>,
                interpretCmpGte<uint16_t>,
                interpretCmpGte<int16_t>,
                interpretCmpLt<uint16_t>,
                interpretCmpLt<int16_t>,
                interpretCmpLte<uint16_t>,
                interpretCmpLte<int16_t>,
                interpretMathAdd<int32_t>,
                interpretMathSub<int32_t>,
                interpretMathMul<int32_t>,
                interpretMathDiv<uint32_t>,
                interpretMathDiv<int32_t>,
                interpretMathMod<uint32_t>,
                interpretMathMod<int32_t>,
                interpretCmpEq<int32_t>,
                interpretCmpNeq<int32_t>,
                interpretCmpGt<uint32_t>,
                interpretCmpGt<int32_t>,
                interpretCmpGte<uint32_t>,
                interpretCmpGte<int32_t>,
                interpretCmpLt<uint32_t>,
                interpretCmpLt<int32_t>,
                interpretCmpLte<uint32_t>,
                interpretCmpLte<int32_t>,
                interpretMathAdd<int64_t>,
                interpretMathAddSI64,
                interpretMathSub<int64_t>,
                interpretMathMul<int64_t>,
                interpretMathDiv<uint64_t>,
                interpretMathDiv<int64_t>,
                interpretMathMod<uint64_t>,
                interpretMathMod<int64_t>,
                interpretCmpEq<int64_t>,
                interpretCmpNeq<int64_t>,
                interpretCmpGt<uint64_t>,
                interpretCmpGt<int64_t>,
                interpretCmpGte<uint64_t>,
                interpretCmpGte<int64_t>,
                interpretCmpLt<uint64_t>,
                interpretCmpLt<int64_t>,
                interpretCmpLte<uint64_t>,
                interpretCmpLte<int64_t>,
                interpretMathAdd<float>,
                interpretMathSub<float>,
                interpretMathMul<float>,
                interpretMathDiv<float>,
                interpretCmpEq<float>,
                interpretCmpNeq<float>,
                interpretCmpLt<float>,
                interpretCmpLte<float>,
                interpretCmpGt<float>,
                interpretCmpGte<float>,
                interpretMathAdd<double>,
                interpretMathSub<double>,
                interpretMathMul<double>,
                interpretMathDiv<double>,
                interpretCmpEq<double>,
                interpretCmpNeq<double>,
                interpretCmpLt<double>,
                interpretCmpLte<double>,
                interpretCmpGt<double>,
                interpretCmpGte<double>,
                interpretMathBitwiseAnd,
                interpretMathBitwiseOr,
                interpretMathBitwiseXor,
                interpretMathBitwiseShl,
                interpretMathBitwiseshr,
                interpretStoreConst,
                interpretStore,
                interpretBumpSP,
                interpretJumpIf,
                interpretJump,
                interpretCalli,
                interpretCalle,
                interpretCall,
                interpretReturn,
                interpretExit,
                interpretPanic,
                interpretPuts,
                interpretNop,
                interpretNot,
                interpretBitNot,
                interpretConvert};

        libs = vector_init<void *>(10);
        for (auto lib : externalLibs) {
            auto path = realpath(string("/usr/local/lib/" + *lib + ".dylib").c_str(), nullptr);
            if (path == nullptr) {
                path = realpath(string("/usr/lib/" + *lib + ".dylib").c_str(), nullptr);
            }
            if (path == nullptr) {
                path = realpath(string("./" + *lib + ".dylib").c_str(), nullptr);
            }
            if (path == nullptr) {
                path = realpath(string(*lib + ".dylib").c_str(), nullptr);
            }

            void *libhandle = dlopen(path, RTLD_LAZY);
            if (!libhandle) {
                fprintf(stderr, "dlopen error: %s\n", dlerror());
                exit(1);
            }

            vector_append(libs, libhandle);
        }
    }

    void interpret();
    void callIndex(int64_t index);

    void dumpStack();

    template <typename T>
    void copyToStack(T t, int64_t offset) {
        memcpy(&stack[offset], &t, sizeof(T));
    }

    template <typename T>
    void pushToStack(T t) {
        copyToStack(t, sp);
        sp += sizeof(T);
    }

    template <typename T>
    inline T readFromStack(int64_t offset) {
        return bytesTo<T>(stack, offset);
    }

    template <typename T>
    inline T consume() {
        auto value = bytesTo<T>(instructions, pc);
        pc += sizeof(T);
        return value;
    }

    template <typename T>
    T read() {
        auto inst = static_cast<Instruction>(instructions[pc]);

        // RELCONST
        if (inst == Instruction::RELCONSTI32 || inst == Instruction::RELCONSTI64) {
            pc += 1;
            auto consumed = consume<T>();
            consumed += static_cast<T>(bp);
            return consumed;
        }

        // REL
        if (inst == Instruction::RELI8 || inst == Instruction::RELI16
            || inst == Instruction::RELI32 || inst == Instruction::RELI64
            || inst == Instruction::RELF32 || inst == Instruction::RELF64) {
            pc += 1;
            auto consumed = consume<int64_t>();
            return readFromStack<T>(consumed + bp);
        }

        // CONST
        if (inst == Instruction::CONSTI8 || inst == Instruction::CONSTI16
            || inst == Instruction::CONSTI32 || inst == Instruction::CONSTI64
            || inst == Instruction::CONSTF32 || inst == Instruction::CONSTF64) {
            pc += 1;
            return consume<T>();
        }

        if (inst == Instruction::I64) {
            pc += 1;
            auto consumed = consume<int64_t>();
            auto read = readFromStack<int64_t>(consumed + bp);
            auto stackData = (int64_t) stack.data();
            auto offsetRead = read - stackData;
            return offsetRead;
        }

        cpi_assert(false && "unrecognized inst for read<T>");
        return {};
    }

    auto readBits8() {
        return read<int8_t>();
    }

    auto readBits16() {
        return read<int16_t>();
    }

    auto readBits32() {
        return read<int32_t>();
    }

    auto readBits64() {
        return read<int64_t>();
    }

    template<int32_t bits, typename T>
    T readBits() {
        if (bits == 8) {
            return static_cast<T>(readBits8());
        }
        if (bits == 16) {
            return static_cast<T>(readBits16());
        }
        if (bits == 32) {
            return static_cast<T>(readBits32());
        }
        if (bits == 64) {
            return static_cast<T>(readBits64());
        }
    }
};

template<typename FROM, typename TO>
void interpretConvertFromTo(void *fromAddr, void *toAddr) {
    FROM from = *((FROM *) fromAddr);

    auto to = (TO) from;

    auto *castedToAddr = (TO *) toAddr;
    *castedToAddr = to;
}

template<typename T>
void interpretConvertFrom(void *fromAddr, void *toAddr, NodeTypekind toType) {
    switch (toType) {
        case NodeTypekind::I8: {
            interpretConvertFromTo<T, int8_t>(fromAddr, toAddr);
        } break;
        case NodeTypekind::I16: {
            interpretConvertFromTo<T, int16_t>(fromAddr, toAddr);
        } break;
        case NodeTypekind::I32: {
            interpretConvertFromTo<T, int32_t>(fromAddr, toAddr);
        } break;
        case NodeTypekind::I64: {
            interpretConvertFromTo<T, int64_t>(fromAddr, toAddr);
        } break;
        case NodeTypekind::F32: {
            interpretConvertFromTo<T, float>(fromAddr, toAddr);
        } break;
        case NodeTypekind::F64: {
            interpretConvertFromTo<T, double>(fromAddr, toAddr);
        } break;
        default: cpi_assert(false);
    }
}

template <typename T>
void interpretMathAdd(Interpreter *interp) {
    auto a = interp->read<T>();
    auto b = interp->read<T>();
    auto result = a + b;
    auto storeOffset = interp->consume<int64_t>();
    interp->copyToStack(result, interp->bp + storeOffset);
}

template <typename T>
void interpretMathSub(Interpreter *interp) {
    auto a = interp->read<T>();
    auto b = interp->read<T>();
    auto result = a - b;
    auto storeOffset = interp->consume<int64_t>();
    interp->copyToStack(result, interp->bp + storeOffset);
}

template <typename T>
void interpretMathMul(Interpreter *interp) {
    auto a = interp->read<T>();
    auto b = interp->read<T>();
    auto result = a * b;
    auto storeOffset = interp->consume<int64_t>();
    interp->copyToStack(result, interp->bp + storeOffset);
}

template <typename T>
void interpretMathDiv(Interpreter *interp) {
    auto a = interp->read<T>();
    auto b = interp->read<T>();
    auto result = a / b;
    auto storeOffset = interp->consume<int64_t>();
    interp->copyToStack(result, interp->bp + storeOffset);
}

template <typename T>
void interpretMathMod(Interpreter *interp) {
    auto a = interp->read<T>();
    auto b = interp->read<T>();
    auto result = a % b;
    auto storeOffset = interp->consume<int64_t>();
    interp->copyToStack(result, interp->bp + storeOffset);
}

// comparison
template <typename T>
void interpretCmpEq(Interpreter *interp) {
    auto a = interp->read<T>();
    auto b = interp->read<T>();
    int32_t result = a == b ? 1 : 0;
    auto storeOffset = interp->consume<int64_t>();
    interp->copyToStack(result, interp->bp + storeOffset);
}

template <typename T>
void interpretCmpNeq(Interpreter *interp) {
    auto a = interp->read<T>();
    auto b = interp->read<T>();
    int32_t result = a != b ? 1 : 0;
    auto storeOffset = interp->consume<int64_t>();
    interp->copyToStack(result, interp->bp + storeOffset);
}

template <typename T>
void interpretCmpGt(Interpreter *interp) {
    auto a = interp->read<T>();
    auto b = interp->read<T>();
    int32_t result = a > b ? 1 : 0;
    auto storeOffset = interp->consume<int64_t>();
    interp->copyToStack(result, interp->bp + storeOffset);
}

template <typename T>
void interpretCmpGte(Interpreter *interp) {
    auto a = interp->read<T>();
    auto b = interp->read<T>();
    int32_t result = a >= b ? 1 : 0;
    auto storeOffset = interp->consume<int64_t>();
    interp->copyToStack(result, interp->bp + storeOffset);
}

template <typename T>
void interpretCmpLt(Interpreter *interp) {
    auto a = interp->read<T>();
    auto b = interp->read<T>();
    int32_t result = a < b ? 1 : 0;
    auto storeOffset = interp->consume<int64_t>();
    interp->copyToStack(result, interp->bp + storeOffset);
}

template <typename T>
void interpretCmpLte(Interpreter *interp) {
    auto a = interp->read<T>();
    auto b = interp->read<T>();
    int32_t result = a <= b ? 1 : 0;
    auto storeOffset = interp->consume<int64_t>();
    interp->copyToStack(result, interp->bp + storeOffset);
}

#endif // INTERPRETER_H