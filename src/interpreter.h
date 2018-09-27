#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <utility>
#include <vector>
#include <stack>
#include <dlfcn.h>
#include <zmq.h>

#include "assembler.h"
#include "semantic.h"
#include "bytecodegen.h"

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

struct Breakpoint {
    unsigned long instIndex;

    bool conditional = false;
    string condition = "";
};

class Interpreter {
public:
    vector<unsigned char> instructions = {};
    hash_t<uint32_t, uint64_t> *fnTable;
    hash_t<string, void *> *externalSymbols;

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
    vector<Breakpoint> breakpoints = {};
    bool continuing = false;
    SourceMapStatement stoppedOnStatement;
    bool debugging = false;

    vector_t<Node *> externalFnTable;
    vector_t<void *> libs;

    // used for stepping 'over' functions (as opposed to normal step which goes 'into')
    uint16_t depth = 0;
    int32_t overDepth = (2 << 15) + 1;

    void *zmq_ctx;
    void *zmq_sock;

    Interpreter(vector_t<string *> externalLibs): Interpreter(2048 * 64, externalLibs) {}

    Interpreter(int32_t stackSize_, vector_t<string *> externalLibs) {
        this->stackSize = stackSize_;

        stack.reserve((unsigned long) stackSize);
        for (auto i = 0; i < stackSize; i++) {
            stack.push_back(0);
        }

        this->fnTable = hash_init<uint32_t, uint64_t>(64);
        this->externalSymbols = hash_init<string, void *>(64);

        if (debugFlag) {
            zmq_ctx = zmq_ctx_new();
            zmq_sock = zmq_socket(zmq_ctx, ZMQ_REP);
        }

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
            auto home = strdup(getenv("HOME"));
            auto path = realpath(string("/usr/local/lib/cpi/" + *lib + ".dylib").c_str(), nullptr);

            if (path == nullptr) {
                path = realpath(string("/usr/local/lib/" + *lib + ".dylib").c_str(), nullptr);
            }
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
    void zsend(string s);

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

void interp_destroy(Interpreter *interp);

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

template<typename T>
int64_t evaluate(Interpreter *fromInterp, SourceInfo srcInfo, Scope *scope, string code) {
    auto evalFnDecl = new Node(srcInfo, NodeType::FN_DECL, scope);
    evalFnDecl->fnDeclData.debugLocalOffset = fromInterp->sp - fromInterp->bp;

    auto evalLexer = new Lexer(std::move(code), false);

    auto evalParser = new Parser(evalLexer);
    evalParser->isCopying = true;
    evalParser->scopes.pop();
    evalParser->scopes.push(scope);
    evalParser->currentFnDecl = evalFnDecl;
    auto parsed = evalParser->parseRvalue();

    // set the srcInfo to the original srcInfo in case there's polymorphs
    evalLexer->srcInfo = srcInfo;

    auto semantic = new Semantic();
    semantic->currentFnDecl = evalFnDecl;
    semantic->lexer = evalLexer;
    semantic->parser = evalParser;
    semantic->addStaticIfs(evalParser->scopes.top());
    semantic->addImports(*evalParser->imports, *evalParser->impls, *evalParser->contexts, *evalParser->contextInits);

    auto wrappedRet = new Node(parsed->region.srcInfo, NodeType::RETURN, parsed->scope);
    wrappedRet->nodeData = parsed;

    vector_append(evalFnDecl->fnDeclData.body, wrappedRet);
    vector_append(evalFnDecl->fnDeclData.returns, wrappedRet);
    semantic->resolveTypes(evalFnDecl);

    // gen
    auto gen = new BytecodeGen();
    gen->isMainFn = true;
    gen->sourceMap.sourceInfo = evalFnDecl->region.srcInfo;
    gen->processFnDecls = true;
    gen->debugLocalOffset = fromInterp->bp - fromInterp->sp;

    gen->gen(evalFnDecl);
    gen->debugLocalOffset = 0;

    while (!gen->toProcess.empty()) {
        gen->isMainFn = false;
        gen->processFnDecls = true;
        gen->genId = 2;
        gen->gen(gen->toProcess.front());
        gen->toProcess.pop();
    }
    gen->fixup();

    for (auto g : gen->generatedNodes) {
        g->genId = 0;
        g->bytecode = {};

        if (g->debugBytecodeAdjusted) {
            g->debugBytecodeAdjusted = false;
            g->localOffset -= (fromInterp->bp - fromInterp->sp);
        }
    }

//    auto m = new MnemonicPrinter(gen->instructions);
//    m->fnTable = gen->fnTable;
//    cout << m->debugString() << endl;

    auto interp = new Interpreter(semantic->linkLibs);

    interp->bp = fromInterp->bp;
    interp->sp = fromInterp->bp;

    interp->externalFnTable = fromInterp->externalFnTable;
    for (auto &entry : gen->externalFnTable) {
        vector_append(interp->externalFnTable, entry);
    }
    interp->libs = fromInterp->libs;
    interp->instructions = gen->instructions;
    interp->fnTable = gen->fnTable;
    interp->stack = fromInterp->stack; // todo(chad): this copies a lot of stuff. Make stack a vector_t? (or maybe we actually want to copy...)
    interp->sourceMap = gen->sourceMap;
    interp->continuing = true;
    interp->instructions = gen->instructions;
    interp->fnTable = gen->fnTable;
    interp->interpret();

    // todo(chad): present the type of the answer appropriately -- don't just assume it's i64
    auto answer = interp->readFromStack<int64_t>(interp->bp);

    interp_destroy(interp);

    return answer;
}

#endif // INTERPRETER_H