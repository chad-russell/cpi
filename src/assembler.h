#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include <string>
#include <vector>
#include <unordered_map>

#include "util.h"

using namespace std;

enum class TokenType : unsigned char {
    // I8 math
    ADDI8, SUBI8, MULI8, UDIVI8, SDIVI8, UREMI8, SREMI8, 
    EQI8, NEQI8, UGTI8, SGTI8, UGEI8, SGEI8, ULTI8, SLTI8, ULEI8, SLEI8,

    // I16 math
    ADDI16, SUBI16, MULI16, UDIVI16, SDIVI16, UREMI16, SREMI16, 
    EQI16, NEQI16, UGTI16, SGTI16, UGEI16, SGEI16, ULTI16, SLTI16, ULEI16, SLEI16,

    // I32 math
    ADDI32, SUBI32, MULI32, UDIVI32, SDIVI32, UREMI32, SREMI32,
    EQI32, NEQI32, UGTI32, SGTI32, UGEI32, SGEI32, ULTI32, SLTI32, ULEI32, SLEI32,

    // I64 math
    ADDI64, ADD_S_I64, SUBI64, SUB_S_I64, MULI64, UDIVI64, SDIVI64, UREMI64, SREMI64,
    EQI64, NEQI64, UGTI64, SGTI64, UGEI64, SGEI64, ULTI64, SLTI64, ULEI64, SLEI64,

    // F32 math
    ADDF32, SUBF32, MULF32, DIVF32,
    EQF32, NEQF32, LTF32, LEF32, GTF32, GEF32,

    // F64 math
    ADDF64, SUBF64, MULF64, DIVF64,
    EQF64, NEQF64, LTF64, LEF64, GTF64, GEF64,

    // bitwise math
    BITAND, BITOR, BITXOR, BITSHL, BITSHR,

    // general instructions
    STORECONST,
    STORE,
    BUMPSP,
    JUMPIF,
    JUMP,
    CALLI,
    CALLE,
    CALL, 
    RET, 
    EXIT,
    PANIC,
    PUTS,
    NOP,

    // literals
    CONSTI8, CONSTI16, CONSTI32, CONSTI64, CONSTF32, CONSTF64,

    // types
    I8, I16, I32, I64, F32, F64,

    RELCONSTI32, RELI32, RELCONSTI64, RELI64, RELI8, RELI16, RELF32, RELF64,

    // end of instruction duplication, the rest are only tokens
    COMMENT, 
    EOF_, 
    CONSTINT, 
    CONSTFLOAT
};

enum class Instruction : unsigned char {
    // I8 math
    ADDI8, SUBI8, MULI8, UDIVI8, SDIVI8, UREMI8, SREMI8,
    EQI8, NEQI8, UGTI8, SGTI8, UGEI8, SGEI8, ULTI8, SLTI8, ULEI8, SLEI8,

    // I16 math
    ADDI16, SUBI16, MULI16, UDIVI16, SDIVI16, UREMI16, SREMI16, 
    EQI16, NEQI16, UGTI16, SGTI16, UGEI16, SGEI16, ULTI16, SLTI16, ULEI16, SLEI16,

    // I32 math
    ADDI32, SUBI32, MULI32, UDIVI32, SDIVI32, UREMI32, SREMI32,
    EQI32, NEQI32, UGTI32, SGTI32, UGEI32, SGEI32, ULTI32, SLTI32, ULEI32, SLEI32,

    // I64 math
    ADDI64, ADD_S_I64, SUBI64, SUB_S_I64, MULI64, UDIVI64, SDIVI64, UREMI64, SREMI64,
    EQI64, NEQI64, UGTI64, SGTI64, UGEI64, SGEI64, ULTI64, SLTI64, ULEI64, SLEI64,

    // F32 math
    ADDF32, SUBF32, MULF32, DIVF32,
    EQF32, NEQF32, LTF32, LEF32, GTF32, GEF32,

    // F64 math
    ADDF64, SUBF64, MULF64, DIVF64,
    EQF64, NEQF64, LTF64, LEF64, GTF64, GEF64,

    // bitwise math
    BITAND, BITOR, BITXOR, BITSHL, BITSHR,

    // general instructions
    STORECONST,
    STORE,
    STORE_RELCONST_RELCONST,
    BUMPSP,
    JUMPIF,
    JUMP,
    CALLI,
    CALLE,
    CALL, 
    RET, 
    EXIT,
    PANIC,
    PUTS,
    NOP,
    NOT,
    BITNOT,
    CONVERT,

    // literals
    CONSTI8, CONSTI16, CONSTI32, CONSTI64, CONSTF32, CONSTF64,

    // types
    I8, I16, I32, I64, F32, F64,

    // relative literals
    RELCONSTI32, RELI32, RELCONSTI64, RELI64, RELI8, RELI16, RELF32, RELF64,
};

struct Token {
    TokenType type = {};
    Region region = {};
};

class AssemblyLexer {
public:
    Token front = {};
    Token next = {};

    Location lastLoc = {};
    Location loc = {};

    vector<unsigned char> instructions = {};
    hash_t<uint32_t, uint64_t> *fnTable;

    Location savedLoc = {};
    unsigned long lastInstStart;
    int argCount;
    SourceMap sourceMap = {};

    const static vector<string> tokenTypeStrings;
    static hash_t<string, TokenType> *nameToTokenType;

    const static vector<string> instructionStrings;
    static hash_t<string, Instruction> *nameToInstruction;

    AssemblyLexer(string fileName);
    static void populateMaps();
    void readFnTable();

    bool empty();

    void popFront();

    void popFrontFinalize(Token newNext, vector<unsigned char> newInst);

    void eat();
private:
    int getArgCount(TokenType tt);
};

class MnemonicPrinter {
public:
    const vector<unsigned char> &instructions;
    hash_t<uint32_t, uint64_t> *fnTable;

    uint32_t pc = 0;
    string instructionString = "";

    string debugString();
    string debugString(uint32_t startPc, uint32_t endPc);
    void step();
    void readTypeAndIntOrFloat();
    void readTypeAndInt();
    void readTypeAndFloat();

    MnemonicPrinter(const vector<unsigned char> &instructions) : instructions(instructions) {}

    template <typename T>
    T consume() {
        auto value = bytesTo<T>(instructions, pc);
        pc += sizeof(T);
        return value;
    }
};

#endif // ASSEMBLER_H