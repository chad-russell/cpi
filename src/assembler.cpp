#include <string>
#include <fstream>
#include <streambuf>
#include <iostream>
#include <cassert>

#include "util.h"
#include "assembler.h"

using namespace std;

AssemblyLexer::AssemblyLexer(string fileName)  {
    ifstream t(fileName);
    string fileBytes;

    t.seekg(0, ios::end);   
    fileBytes.reserve(t.tellg());
    t.seekg(0, ios::beg);

    fileBytes.assign((istreambuf_iterator<char>(t)),
                istreambuf_iterator<char>());

    srcInfo = {fileName, fileBytes, {0}};
    lastLoc = {0, 0, 0};
    loc = {0, 0, 0};

    nameToTokenType = {};
    for (unsigned char i = 0; i < AssemblyLexer::tokenTypeStrings.size(); i++) {
        nameToTokenType.insert({AssemblyLexer::tokenTypeStrings[i],
                                static_cast<TokenType>(i)});
    }

    nameToInstruction = {};
    for (unsigned char i = 0; i < AssemblyLexer::instructionStrings.size(); i++) {
        nameToInstruction.insert({AssemblyLexer::instructionStrings[i],
                                  static_cast<Instruction>(i)});
    }

    popFront();
    popFront();
}


bool AssemblyLexer::empty() {
    return front.type == TokenType::EOF_;
}

int AssemblyLexer::getArgCount(TokenType tt) {
    auto ttName = AssemblyLexer::tokenTypeStrings[static_cast<int>(tt)];

    if (startsWith(ttName, "ADD")
        || startsWith(ttName, "SUB")
        || startsWith(ttName, "MUL")
        || startsWith(ttName, "DIV")
        || startsWith(ttName, "UDIV")
        || startsWith(ttName, "SDIV")
        || startsWith(ttName, "REM")
        || startsWith(ttName, "UREM")
        || startsWith(ttName, "SREM")
        || startsWith(ttName, "EQ")
        || startsWith(ttName, "NEQ")
        || startsWith(ttName, "GT")
        || startsWith(ttName, "GE")
        || startsWith(ttName, "UGT")
        || startsWith(ttName, "SGT")
        || startsWith(ttName, "UGE")
        || startsWith(ttName, "SGE")
        || startsWith(ttName, "LT")
        || startsWith(ttName, "ULT")
        || startsWith(ttName, "SLT")
        || startsWith(ttName, "LE")
        || startsWith(ttName, "ULE")
        || startsWith(ttName, "SLE")
        || startsWith(ttName, "AND8")
        || startsWith(ttName, "AND16")
        || startsWith(ttName, "AND32")
        || startsWith(ttName, "AND64")
        || startsWith(ttName, "OR8")
        || startsWith(ttName, "OR16")
        || startsWith(ttName, "OR32")
        || startsWith(ttName, "OR64")
        || startsWith(ttName, "XOR8")
        || startsWith(ttName, "XOR16")
        || startsWith(ttName, "XOR32")
        || startsWith(ttName, "XOR64")
        || tt == TokenType::JUMPIF) {
        return 3;
    } else if (tt == TokenType::STORE) {
        return 2;
    } else if (startsWith(ttName, "CONST")
        || startsWith(ttName, "I")
        || startsWith(ttName, "F")
        || tt == TokenType::BUMPSP
        || tt == TokenType::JUMP
        || tt == TokenType::CALL
        || tt == TokenType::CALLI) {
        return 1;
    } else if (tt == TokenType::RET 
        || tt == TokenType::EXIT
        || tt == TokenType::COMMENT) {
        return 0;
    }

    cout << "unknown TokenType for argCount: '" << ttName << "'";
    assert(false);
}

void AssemblyLexer::popFront() {
    front = next;
    Token newNext;

    // ignore whitespace
    while (loc.byteIndex < srcInfo.source.length() && isspace(srcInfo.source[loc.byteIndex])) {
        eat();
    }
    lastLoc = loc;

    // Comment
    if (startsWith(srcInfo.source, loc.byteIndex, "--")) {
        // eat until newline
        while (loc.byteIndex < srcInfo.source.length() && srcInfo.source[loc.byteIndex] != '\n') {
            eat();
        }

        newNext.type = TokenType::COMMENT;
        argCount = 0;
        popFrontFinalize(newNext, {});

        return;
    }

    // ignore whitespace (again)
    while (loc.byteIndex < srcInfo.source.length() && isspace(srcInfo.source[loc.byteIndex])) {
        eat();
    }
    lastLoc = loc;

    // look for EOF
    if (loc.byteIndex >= srcInfo.source.length()) {
        newNext.type = TokenType::EOF_;
        argCount = 0;
        popFrontFinalize(newNext, {});

        return;
    }

    // try parsing all the tokens
    for (auto memberName : tokenTypeStrings) {
        auto member = nameToTokenType.at(memberName);

        if (startsWith(srcInfo.source, loc.byteIndex, memberName)) {
            auto startIndex = loc.byteIndex;

            newNext.type = member;
            argCount += getArgCount(member);
            for (auto i = 0; i < memberName.length(); i++) {
                eat();
            }

            auto newInst = nameToInstruction.at(srcInfo.source.substr(startIndex, memberName.length()));
            popFrontFinalize(newNext, {static_cast<unsigned char>(newInst)});

            return;
        }
    }

    // maybe it's an integer or floating point literal
    auto startIndex = loc.byteIndex;
    while (srcInfo.source[loc.byteIndex] == '-'
        || isnumber(srcInfo.source[loc.byteIndex]) 
        || srcInfo.source[loc.byteIndex] == '.') {
        eat();
    }
    auto toParse = srcInfo.source.substr(startIndex, loc.byteIndex - startIndex);
    if (toParse.find('.') != string::npos) {
        auto parsed = stod(toParse);

        auto inst = static_cast<Instruction>(next.type);
        vector<unsigned char> newInst;
        switch (inst) {
            case Instruction::CONSTF32: {
                newInst = toBytes(static_cast<float>(parsed));
            } break;
            case Instruction::CONSTF64: {
                newInst = toBytes(static_cast<double>(parsed));
            } break;
            default: assert(false);
        }

        newNext.type = TokenType::CONSTFLOAT;
        popFrontFinalize(newNext, newInst);

        return;
    } else {
        auto parsed = stoi(toParse);

        auto inst = static_cast<Instruction>(next.type);
        vector<unsigned char> newInst;
        switch (inst) {
                case Instruction::CONSTI8: {
                    newInst = toBytes(static_cast<int8_t>(parsed));
                } break;
                case Instruction::CONSTI16: {
                    newInst = toBytes(static_cast<int16_t>(parsed));
                } break;
                case Instruction::CONSTI32: {
                    newInst = toBytes(static_cast<int32_t>(parsed));
                } break;
                case Instruction::CONSTI64: {
                    newInst = toBytes(static_cast<int64_t>(parsed));
                } break;
                default: {
                    newInst = toBytes(static_cast<int32_t>(parsed));
                }
            }

        newNext.type = TokenType::CONSTINT;
        popFrontFinalize(newNext, newInst);

        return;
    }

    cout << "could not parse token at byteIndex "
         << loc.byteIndex
         << "("
         << srcInfo.source.substr(loc.byteIndex, 10)
         << ")...";

    assert(false);
}

void AssemblyLexer::popFrontFinalize(Token newNext, vector<unsigned char> newInst) {
    newNext.region = {srcInfo, lastLoc, loc};
    next = newNext;
    instructions.insert(instructions.end(), newInst.begin(), newInst.end());

    if (argCount == 0) {
        vector<unsigned long> moreSourceMap = {savedLoc.line, savedLoc.col,
                                               newNext.region.end.line, newNext.region.end.col,
                                               lastInstStart};
        sourceMap.insert(sourceMap.end(), moreSourceMap.begin(), moreSourceMap.end());

        // ignore whitespace
        while (loc.byteIndex < srcInfo.source.length() && isspace(srcInfo.source[loc.byteIndex])) {
            eat();
        }
        savedLoc = loc;

        lastInstStart = instructions.size();
    } else {
        argCount -= 1;
    }
}

void AssemblyLexer::eat() {
    auto frontChar = srcInfo.source[loc.byteIndex];

    if (frontChar == '\n') {
        loc.line += 1;
        loc.col = 0;

        srcInfo.lines.push_back(loc.byteIndex + 1);
    }
    else {
        loc.col += 1;
    }

    loc.byteIndex += bytesInCodepoint(frontChar);
}

const vector<string> AssemblyLexer::tokenTypeStrings = {
    // I8 math
    "ADDI8", "SUBI8", "MULI8", "UDIVI8", "SDIVI8", "UREMI8", "SREMI8", 
    "EQI8", "NEQI8", "UGTI8", "SGTI8", "UGEI8", "SGEI8", "ULTI8", "SLTI8", "ULEI8", "SLEI8",

    // I16 math
    "ADDI16", "SUBI16", "MULI16", "UDIVI16", "SDIVI16", "UREMI16", "SREMI16", 
    "EQI16", "NEQI16", "UGTI16", "SGTI16", "UGEI16", "SGEI16", "ULTI16", "SLTI16", "ULEI16", "SLEI16",

    // I32 math
    "ADDI32", "SUBI32", "MULI32", "UDIVI32", "SDIVI32", "UREMI32", "SREMI32",
    "EQI32", "NEQI32", "UGTI32", "SGTI32", "UGEI32", "SGEI32", "ULTI32", "SLTI32", "ULEI32", "SLEI32",

    // I64 math
    "ADDI64", "SUBI64", "MULI64", "UDIVI64", "SDIVI64", "UREMI64", "SREMI64",
    "EQI64", "NEQI64", "UGTI64", "SGTI64", "UGEI64", "SGEI64", "ULTI64", "SLTI64", "ULEI64", "SLEI64",

    // F32 math
    "ADDF32", "SUBF32", "MULF32", "DIVF32", "REMF32",
    "EQF32", "NEQF32", "LTF32", "LEF32", "GTF32", "GEF32",

    // F64 math
    "ADDF64", "SUBF64", "MULF64", "DIVF64", "REMF64",
    "EQF64", "NEQF64", "LTF64", "LEF64", "GTF64", "GEF64",

    // bitwise math
    "AND8", "AND16", "AND32", "AND64", "OR8", "OR16", "OR32", "OR64", "XOR8", "XOR16", "XOR32", "XOR64",

    // general instructions
    "STORE", 
    "BUMPSP", 
    "JUMPIF", 
    "JUMP", 
    "CALLI", 
    "CALL", 
    "RET", 
    "EXIT",

    // literals
    "CONSTI8", "CONSTI16", "CONSTI32", "CONSTI64", "CONSTF32", "CONSTF64",

    // types
    "I8", "I16", "I32", "I64", "F32", "F64",

    // end of instruction duplication, the rest are only tokens
    "COMMENT", 
    "EOF_", 
    "CONSTINT", 
    "CONSTFLOAT"
};

const vector<string> AssemblyLexer::instructionStrings = {
    // I8 math
    "ADDI8", "SUBI8", "MULI8", "UDIVI8", "SDIVI8", "UREMI8", "SREMI8", 
    "EQI8", "NEQI8", "UGTI8", "SGTI8", "UGEI8", "SGEI8", "ULTI8", "SLTI8", "ULEI8", "SLEI8",

    // I16 math
    "ADDI16", "SUBI16", "MULI16", "UDIVI16", "SDIVI16", "UREMI16", "SREMI16", 
    "EQI16", "NEQI16", "UGTI16", "SGTI16", "UGEI16", "SGEI16", "ULTI16", "SLTI16", "ULEI16", "SLEI16",

    // I32 math
    "ADDI32", "SUBI32", "MULI32", "UDIVI32", "SDIVI32", "UREMI32", "SREMI32",
    "EQI32", "NEQI32", "UGTI32", "SGTI32", "UGEI32", "SGEI32", "ULTI32", "SLTI32", "ULEI32", "SLEI32",

    // I64 math
    "ADDI64", "SUBI64", "MULI64", "UDIVI64", "SDIVI64", "UREMI64", "SREMI64",
    "EQI64", "NEQI64", "UGTI64", "SGTI64", "UGEI64", "SGEI64", "ULTI64", "SLTI64", "ULEI64", "SLEI64",

    // F32 math
    "ADDF32", "SUBF32", "MULF32", "DIVF32", "REMF32",
    "EQF32", "NEQF32", "LTF32", "LEF32", "GTF32", "GEF32",

    // F64 math
    "ADDF64", "SUBF64", "MULF64", "DIVF64", "REMF64",
    "EQF64", "NEQF64", "LTF64", "LEF64", "GTF64", "GEF64",

    // bitwise math
    "AND8", "AND16", "AND32", "AND64", "OR8", "OR16", "OR32", "OR64", "XOR8", "XOR16", "XOR32", "XOR64",

    // general instructions
    "STORE", 
    "BUMPSP", 
    "JUMPIF", 
    "JUMP", 
    "CALLI", 
    "CALL", 
    "RET", 
    "EXIT",

    // literals
    "CONSTI8", "CONSTI16", "CONSTI32", "CONSTI64", "CONSTF32", "CONSTF64",

    // types
    "I8", "I16", "I32", "I64", "F32", "F64",
};


string MnemonicPrinter::debugString() {
    instructionString = "";

    while (pc < instructions.size()) {
        step();
    }

    return instructionString;
}

void MnemonicPrinter::step() {
    auto inst = AssemblyLexer::instructionStrings[instructions[pc]];
    pc += 1;

    if (startsWith(inst, "ADDI")
        || startsWith(inst, "SUBI")
        || startsWith(inst, "MULI")
        || startsWith(inst, "UDIVI")
        || startsWith(inst, "SDIVI")
        || startsWith(inst, "UREMI")
        || startsWith(inst, "SREMI")
        || startsWith(inst, "EQI")
        || startsWith(inst, "NEQI")
        || startsWith(inst, "UGTI")
        || startsWith(inst, "SGTI")
        || startsWith(inst, "UGEI")
        || startsWith(inst, "SGEI")
        || startsWith(inst, "ULTI")
        || startsWith(inst, "SLTI")
        || startsWith(inst, "ULEI")
        || startsWith(inst, "SLEI")) {
            instructionString.append(inst);
            instructionString.append(" ");
            readTypeAndInt();
            instructionString.append(" ");
            readTypeAndInt();
            instructionString.append(" ");
            instructionString.append(to_string(consume<int32_t>()));
    } else if (startsWith(inst, "ADDF")
        || startsWith(inst, "SUBF")
        || startsWith(inst, "MULF")
        || startsWith(inst, "DIVF")
        || startsWith(inst, "REMF")
        || startsWith(inst, "EQF")
        || startsWith(inst, "NEQF")
        || startsWith(inst, "LTF")
        || startsWith(inst, "LEF")
        || startsWith(inst, "GTF")
        || startsWith(inst, "GEF")) {
            instructionString.append(inst);
            instructionString.append(" ");
            readTypeAndFloat();
            instructionString.append(" ");
            readTypeAndFloat();
            instructionString.append(" ");
            instructionString.append(to_string(consume<int32_t>()));
    } else if (startsWith(inst, "AND") || startsWith(inst, "OR") || startsWith(inst, "XOR")) {
        instructionString.append(" ");
        readTypeAndIntOrFloat();
        instructionString.append(" ");
        readTypeAndIntOrFloat();
    } else if (startsWith(inst, "STORE")) {
        instructionString.append("STORE ");
        instructionString.append(to_string(consume<int32_t>()));
        instructionString.append(" ");
        readTypeAndIntOrFloat();
    } else if (startsWith(inst, "JUMPIF")) {
        instructionString.append(inst);
        instructionString.append(" ");
        readTypeAndInt();
        instructionString.append(" ");
        instructionString.append(to_string(consume<int32_t>()));
        instructionString.append(" ");
        instructionString.append(to_string(consume<int32_t>()));
    } else if (startsWith(inst, "CALLI")) {
        instructionString.append(inst);
        instructionString.append(" ");
        readTypeAndInt();
    } else if (startsWith(inst, "BUMPSP") || startsWith(inst, "JUMP") || startsWith(inst, "CALL")) {
        instructionString.append(inst);
        instructionString.append(" ");
        instructionString.append(to_string(consume<int32_t>()));
    } else {
        instructionString.append(inst);
    }

    instructionString.append("\n");
}

void MnemonicPrinter::readTypeAndIntOrFloat()
{
    auto inst = AssemblyLexer::instructionStrings[instructions[pc]];
    if (inst.find('I') != string::npos) {
        readTypeAndInt();
    } else if (inst.find('F') != string::npos) {
        readTypeAndFloat();
    } else {
        assert(false);
    }
}

void MnemonicPrinter::readTypeAndInt()
{
    auto inst = AssemblyLexer::instructionStrings[instructions[pc]];

    instructionString.append(inst);
    instructionString.append(" ");
    pc += 1;

    if (endsWith(inst, "CONSTI8")) {
        instructionString.append(to_string(consume<int8_t>()));
    } else if (endsWith(inst, "CONSTI16")) {
        instructionString.append(to_string(consume<int16_t>()));
    } else if (endsWith(inst, "CONSTI32")) {
        instructionString.append(to_string(consume<int32_t>()));
    } else if (endsWith(inst, "CONSTI64")) {
        instructionString.append(to_string(consume<int64_t>()));
    } else if (endsWith(inst, "I8") 
        || endsWith(inst, "I16") 
        || endsWith(inst, "I32") 
        || endsWith(inst, "I64")) {
        instructionString.append(to_string(consume<int32_t>()));
    } else {
        assert(false);
    }
}

void MnemonicPrinter::readTypeAndFloat()
{
    auto inst = AssemblyLexer::instructionStrings[instructions[pc]];

    inst.append(" ");
    pc += 1;

    if (endsWith(inst, "F32")) {
        instructionString.append(to_string(consume<float>()));
    } else if (endsWith(inst, "F64")) {
        instructionString.append(to_string(consume<double>()));
    } else {
        assert(false);
    }
}
