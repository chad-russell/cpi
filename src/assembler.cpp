#include <string>
#include <fstream>
#include <iostream>
#include <sstream>

#include "util.h"
#include "assembler.h"

using namespace std;

hash_t<string, TokenType> *AssemblyLexer::nameToTokenType = hash_init<string, TokenType>(200);
hash_t<string, Instruction> *AssemblyLexer::nameToInstruction = hash_init<string, Instruction>(200);

void AssemblyLexer::populateMaps() {
    for (unsigned char i = 0; i < AssemblyLexer::tokenTypeStrings.size(); i++) {
        hash_insert(nameToTokenType, AssemblyLexer::tokenTypeStrings[i], static_cast<TokenType>(i));
    }

    for (unsigned char i = 0; i < AssemblyLexer::instructionStrings.size(); i++) {
        hash_insert(nameToInstruction, AssemblyLexer::instructionStrings[i], static_cast<Instruction>(i));
    }
}

void AssemblyLexer::readFnTable() {
    auto firstLinePos = sourceMap.sourceInfo.source->find('\n');
    auto firstLine = sourceMap.sourceInfo.source->substr(0, firstLinePos);

    istringstream iss(firstLine);
    int32_t count;
    iss >> count;
    for (auto i = 0; i < count; i++) {
        uint32_t fnIndex, instIndex;
        iss >> fnIndex;
        iss >> instIndex;
        hash_insert(fnTable, fnIndex, (uint64_t) instIndex);
    }

    sourceMap.sourceInfo.source = new string(
            sourceMap.sourceInfo.source->substr(firstLine.length() + 1,
                                                sourceMap.sourceInfo.source->size() - firstLine.length()));
}

AssemblyLexer::AssemblyLexer(string fileName)  {
    ifstream t(fileName);
    string fileBytes;

    t.seekg(0, ios::end);   
    fileBytes.reserve(static_cast<unsigned long>(t.tellg()));
    t.seekg(0, ios::beg);

    fileBytes.assign((istreambuf_iterator<char>(t)),
                istreambuf_iterator<char>());

    auto lines = vector_init<unsigned long>(1);
    vector_append(lines, (unsigned long) 0);

    sourceMap.sourceInfo = {new string(fileName), new string(fileBytes), lines};

    lastLoc = {0, 0, 0};
    loc = {0, 0, 0};

    readFnTable();

    popFront();
    popFront();
}


bool AssemblyLexer::empty() {
    return front.type == TokenType::EOF_;
}

int AssemblyLexer::getArgCount(TokenType tt) {
    auto ttName = AssemblyLexer::tokenTypeStrings[static_cast<int>(tt)];

    if (ttName == "ADD_S_I64") {
        return 4;
    } else if (startsWith(&ttName, "ADD")
        || startsWith(&ttName, "SUB")
        || startsWith(&ttName, "MUL")
        || startsWith(&ttName, "DIV")
        || startsWith(&ttName, "UDIV")
        || startsWith(&ttName, "SDIV")
        || startsWith(&ttName, "REM")
        || startsWith(&ttName, "UREM")
        || startsWith(&ttName, "SREM")
        || startsWith(&ttName, "EQ")
        || startsWith(&ttName, "NEQ")
        || startsWith(&ttName, "GT")
        || startsWith(&ttName, "GE")
        || startsWith(&ttName, "UGT")
        || startsWith(&ttName, "SGT")
        || startsWith(&ttName, "UGE")
        || startsWith(&ttName, "SGE")
        || startsWith(&ttName, "LT")
        || startsWith(&ttName, "ULT")
        || startsWith(&ttName, "SLT")
        || startsWith(&ttName, "LE")
        || startsWith(&ttName, "ULE")
        || startsWith(&ttName, "SLE")
        || startsWith(&ttName, "AND8")
        || startsWith(&ttName, "AND16")
        || startsWith(&ttName, "AND32")
        || startsWith(&ttName, "AND64")
        || startsWith(&ttName, "OR8")
        || startsWith(&ttName, "OR16")
        || startsWith(&ttName, "OR32")
        || startsWith(&ttName, "OR64")
        || startsWith(&ttName, "XOR8")
        || startsWith(&ttName, "XOR16")
        || startsWith(&ttName, "XOR32")
        || startsWith(&ttName, "XOR64")
        || tt == TokenType::STORE
        || tt == TokenType::JUMPIF) {
        return 3;
    } else if (startsWith(&ttName, "CONST")
               || startsWith(&ttName, "REL")
               || startsWith(&ttName, "I")
               || startsWith(&ttName, "F")
               || tt == TokenType::BUMPSP
               || tt == TokenType::JUMP
               || tt == TokenType::CALL
               || tt == TokenType::CALLI
               || tt == TokenType::CALLE
               || tt == TokenType::STORECONST
               || tt == TokenType::PUTS) {
        return 1;
    } else if (tt == TokenType::RET
               || tt == TokenType::EXIT
               || tt == TokenType::COMMENT
               || tt == TokenType::PANIC) {
        return 0;
    }

    cout << "unknown TokenType for argCount: '" << ttName << "'";
    cpi_assert(false);
}

void AssemblyLexer::popFront() {
    front = next;
    Token newNext;

    // ignore whitespace
    while (loc.byteIndex < sourceMap.sourceInfo.source->length() && isspace(sourceMap.sourceInfo.source->at(loc.byteIndex))) {
        eat();
    }
    lastLoc = loc;

    // Comment
    if (startsWith(sourceMap.sourceInfo.source, loc.byteIndex, "--")) {
        // eat until newline
        while (loc.byteIndex < sourceMap.sourceInfo.source->length() && sourceMap.sourceInfo.source->at(loc.byteIndex) != '\n') {
            eat();
        }

        newNext.type = TokenType::COMMENT;
        argCount = 0;
        popFrontFinalize(newNext, {});

        return;
    }

    // ignore whitespace (again)
    while (loc.byteIndex < sourceMap.sourceInfo.source->length() && isspace(sourceMap.sourceInfo.source->at(loc.byteIndex))) {
        eat();
    }
    lastLoc = loc;

    // look for EOF
    if (loc.byteIndex >= sourceMap.sourceInfo.source->length()) {
        newNext.type = TokenType::EOF_;
        argCount = 0;
        popFrontFinalize(newNext, {});

        return;
    }

    // try parsing all the tokens
    for (const auto &memberName : tokenTypeStrings) {
        auto member = *hash_get(nameToTokenType, memberName);

        if (startsWith(sourceMap.sourceInfo.source, loc.byteIndex, memberName)) {
            auto startIndex = loc.byteIndex;

            newNext.type = member;
            argCount += getArgCount(member);
            for (auto i = 0; i < memberName.length(); i++) {
                eat();
            }

            auto newInst = *hash_get(nameToInstruction,
                                     sourceMap.sourceInfo.source->substr(startIndex, memberName.length()));
            popFrontFinalize(newNext, {static_cast<unsigned char>(newInst)});

            return;
        }
    }

    // maybe it's an integer or floating point literal
    auto startIndex = loc.byteIndex;
    while (sourceMap.sourceInfo.source->at(loc.byteIndex) == '-'
        || isdigit(sourceMap.sourceInfo.source->at(loc.byteIndex))
        || sourceMap.sourceInfo.source->at(loc.byteIndex) == '.') {
        eat();
    }

    auto toParse = sourceMap.sourceInfo.source->substr(startIndex, loc.byteIndex - startIndex);

    if (toParse.empty()) {
        newNext.type = TokenType::EOF_;
        argCount = 0;
        popFrontFinalize(newNext, {});

        return;
    }

    if (toParse.find('.') != string::npos) {
        auto parsed = stod(toParse);

        auto inst = static_cast<Instruction>(next.type);
        vector<unsigned char> newInst;
        switch (inst) {
            case Instruction::CONSTF32: {
                newInst = toBytes(static_cast<float>(parsed));
            } break;
            case Instruction::CONSTF64: {
                newInst = toBytes(parsed);
            } break;
            default: cpi_assert(false);
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
                    newInst = toBytes32(parsed);
                } break;
                case Instruction::CONSTI64: {
                    newInst = toBytes(static_cast<int64_t>(parsed));
                } break;
                default: {
                    newInst = toBytes(parsed);
                }
            }

        newNext.type = TokenType::CONSTINT;
        popFrontFinalize(newNext, newInst);

        return;
    }
}

void AssemblyLexer::popFrontFinalize(Token newNext, vector<unsigned char> newInst) {
    newNext.region = {sourceMap.sourceInfo, lastLoc, loc};
    next = newNext;
    instructions.insert(instructions.end(), newInst.begin(), newInst.end());

    if (argCount == 0) {
//        sourceMap.statements.push_back(SourceMapStatement{
//                lastInstStart,
//                instructions.size(),
//
//                savedLoc.line,
//                savedLoc.col,
//
//                savedLoc.byteIndex,
//                newNext.region.end.byteIndex,
//
//                nullptr
//        });

        // ignore whitespace
        while (loc.byteIndex < sourceMap.sourceInfo.source->length() && isspace(sourceMap.sourceInfo.source->at(loc.byteIndex))) {
            eat();
        }
        savedLoc = loc;

        lastInstStart = instructions.size();
    } else {
        argCount -= 1;
    }
}

void AssemblyLexer::eat() {
    auto frontChar = sourceMap.sourceInfo.source->at(loc.byteIndex);

    if (frontChar == '\n') {
        loc.line += 1;
        loc.col = 1;

        vector_append(sourceMap.sourceInfo.lines, loc.byteIndex + 1);
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
    "ADDF32", "SUBF32", "MULF32", "DIVF32",
    "EQF32", "NEQF32", "LTF32", "LEF32", "GTF32", "GEF32",

    // F64 math
    "ADDF64", "SUBF64", "MULF64", "DIVF64",
    "EQF64", "NEQF64", "LTF64", "LEF64", "GTF64", "GEF64",

    // bitwise math
    "AND8", "AND16", "AND32", "AND64", "OR8", "OR16", "OR32", "OR64", "XOR8", "XOR16", "XOR32", "XOR64",

    // general instructions
    "STORECONST",
    "STORE",
    "BUMPSP",
    "JUMPIF",
    "JUMP",
    "CALLI",
    "CALLE",
    "CALL", 
    "RET", 
    "EXIT",
    "PANIC",
    "NONE",
    "PUTS",
    "TAGCHECK",
    "NOP",

    // literals
    "CONSTI8", "CONSTI16", "CONSTI32", "CONSTI64", "CONSTF32", "CONSTF64",

    // types
    "I8", "I16", "I32", "I64", "F32", "F64",

    "RELCONSTI32", "RELI32", "RELCONSTI64", "RELI64", "RELI8", "RELI16", "RELF32", "RELF64",

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
    "ADDI64", "ADD_S_I64", "SUBI64", "MULI64", "UDIVI64", "SDIVI64", "UREMI64", "SREMI64",
    "EQI64", "NEQI64", "UGTI64", "SGTI64", "UGEI64", "SGEI64", "ULTI64", "SLTI64", "ULEI64", "SLEI64",

    // F32 math
    "ADDF32", "SUBF32", "MULF32", "DIVF32",
    "EQF32", "NEQF32", "LTF32", "LEF32", "GTF32", "GEF32",

    // F64 math
    "ADDF64", "SUBF64", "MULF64", "DIVF64",
    "EQF64", "NEQF64", "LTF64", "LEF64", "GTF64", "GEF64",

    // bitwise math
    "BITAND", "BITOR", "BITXOR", "BITSHL", "BITSHR",

    // general instructions
    "STORECONST",
    "STORE",
    "BUMPSP",
    "JUMPIF",
    "JUMP",
    "CALLI",
    "CALLE",
    "CALL", 
    "RET", 
    "EXIT",
    "PANIC",
    "PUTS",
    "NOP",
    "NOT",
    "BITNOT",
    "CONVERT",

    // literals
    "CONSTI8", "CONSTI16", "CONSTI32", "CONSTI64", "CONSTF32", "CONSTF64",

    // types
    "I8", "I16", "I32", "I64", "F32", "F64",

    "RELCONSTI32", "RELI32", "RELCONSTI64", "RELI64", "RELI8", "RELI16", "RELF32", "RELF64",
};


string MnemonicPrinter::debugString() {
    instructionString = "";

    instructionString.append(to_string(fnTable->size));
    instructionString.append(" ");

    for (auto i = 0; i < fnTable->bucket_count; i++) {
        auto bucket = fnTable->buckets[i];
        if (bucket != nullptr) {
            instructionString.append(to_string(bucket->key));
            instructionString.append(" ");
            instructionString.append(to_string(bucket->value));
            instructionString.append(" ");

            while (bucket->next != nullptr) {
                bucket = bucket->next;

                instructionString.append(to_string(bucket->key));
                instructionString.append(" ");
                instructionString.append(to_string(bucket->value));
                instructionString.append(" ");
            }
        }
    }
    instructionString.append("\n");

    while (pc < instructions.size()) {
        step();
    }

    return instructionString;
}

string MnemonicPrinter::debugString(uint32_t startPc, uint32_t endPc) {
    instructionString = "";

    pc = startPc;
    while (pc < endPc) {
        step();
    }

    return instructionString;
}

void MnemonicPrinter::step() {
    auto instAtPc = static_cast<Instruction>(instructions[pc]);
    auto inst = AssemblyLexer::instructionStrings[instructions[pc]];
    pc += 1;

    if (startsWith(&inst, "ADDI")
        || startsWith(&inst, "SUBI")
        || startsWith(&inst, "MULI")
        || startsWith(&inst, "UDIVI")
        || startsWith(&inst, "SDIVI")
        || startsWith(&inst, "UREMI")
        || startsWith(&inst, "SREMI")
        || startsWith(&inst, "EQI")
        || startsWith(&inst, "NEQI")
        || startsWith(&inst, "UGTI")
        || startsWith(&inst, "SGTI")
        || startsWith(&inst, "UGEI")
        || startsWith(&inst, "SGEI")
        || startsWith(&inst, "ULTI")
        || startsWith(&inst, "SLTI")
        || startsWith(&inst, "ULEI")
        || startsWith(&inst, "SLEI")) {
            instructionString.append(inst);
            instructionString.append(" ");
            readTypeAndInt();
            instructionString.append(" ");
            readTypeAndInt();
            instructionString.append(" ");
            instructionString.append(to_string(consume<int64_t>()));
    } else if (inst == "ADD_S_I64") {
        instructionString.append(inst);

        instructionString.append(" ");
        readTypeAndInt();

        instructionString.append(" ");
        readTypeAndInt();

        instructionString.append(" ");
        instructionString.append(to_string(consume<int32_t>()));

        instructionString.append(" ");
        instructionString.append(to_string(consume<int64_t>()));
    } else if (startsWith(&inst, "ADDF")
        || startsWith(&inst, "SUBF")
        || startsWith(&inst, "MULF")
        || startsWith(&inst, "DIVF")
        || startsWith(&inst, "REMF")
        || startsWith(&inst, "EQF")
        || startsWith(&inst, "NEQF")
        || startsWith(&inst, "LTF")
        || startsWith(&inst, "LEF")
        || startsWith(&inst, "GTF")
        || startsWith(&inst, "GEF")) {
            instructionString.append(inst);
            instructionString.append(" ");
            readTypeAndFloat();
            instructionString.append(" ");
            readTypeAndFloat();
            instructionString.append(" ");
            instructionString.append(to_string(consume<int32_t>()));
    } else if (startsWith(&inst, "AND") || startsWith(&inst, "OR") || startsWith(&inst, "XOR")) {
        instructionString.append(" ");
        readTypeAndIntOrFloat();
        instructionString.append(" ");
        readTypeAndIntOrFloat();
    } else if (startsWith(&inst, "STORECONST")) {
        instructionString.append(inst);
        instructionString.append(" ");
        readTypeAndInt();
        instructionString.append(" ");
        readTypeAndIntOrFloat();
        instructionString.append(" ");
    } else if (startsWith(&inst, "STORE")) {
        instructionString.append(inst);
        instructionString.append(" ");

        readTypeAndInt();
        instructionString.append(" ");

        readTypeAndIntOrFloat();
        instructionString.append(" ");

        instructionString.append(to_string(consume<int32_t>()));
    } else if (startsWith(&inst, "JUMPIF")) {
        instructionString.append(inst);
        instructionString.append(" ");
        readTypeAndInt();
        instructionString.append(" ");
        readTypeAndInt();
        instructionString.append(" ");
        readTypeAndInt();
    } else if (startsWith(&inst, "CALLI")) {
        instructionString.append(inst);
        instructionString.append(" ");
        readTypeAndInt();
    } else if (startsWith(&inst, "CALLE")) {
        instructionString.append(inst);
        instructionString.append(" ");
        instructionString.append(to_string(consume<int32_t>()));
    } else if (startsWith(&inst, "BUMPSP") || startsWith(&inst, "JUMP") || startsWith(&inst, "CALL")) {
        instructionString.append(inst);
        instructionString.append(" ");
        auto callPc = consume<int32_t>();
        instructionString.append(to_string(callPc));
    } else if (startsWith(&inst, "NOT")) {
        instructionString.append(inst);
        instructionString.append(" ");
        auto callPc = consume<int64_t>();
        instructionString.append(to_string(callPc));
    } else if (startsWith(&inst, "CONVERT")) {
        instructionString.append(inst);
        instructionString.append(" ");
        auto fromType = consume<int32_t>();
        instructionString.append(to_string(fromType));
        readTypeAndInt();
        auto toType = consume<int32_t>();
        instructionString.append(to_string(toType));
        readTypeAndInt();
    } else if (startsWith(&inst, "PUTS")) {
        instructionString.append(inst);
        instructionString.append(" ");
        readTypeAndInt();
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
        instructionString.append("<<error readTypeAndIntOrFloat>>");
//        cpi_assert(false);
    }
}

void MnemonicPrinter::readTypeAndInt()
{
    auto inst = instructions[pc];
    auto debugInst = static_cast<Instruction>(instructions[pc]);
    auto instStr = AssemblyLexer::instructionStrings[inst];

    instructionString.append(instStr);
    instructionString.append(" ");
    pc += 1;

    if (endsWith(&instStr, "CONSTI8")) {
        instructionString.append(to_string(consume<int8_t>()));
    } else if (endsWith(&instStr, "CONSTI16")) {
        instructionString.append(to_string(consume<int16_t>()));
    } else if (endsWith(&instStr, "RELCONSTI32")) {
        instructionString.append(to_string(consume<int64_t>()));
    } else if (endsWith(&instStr, "CONSTI32")) {
        instructionString.append(to_string(consume<int32_t>()));
    } else if (endsWith(&instStr, "CONSTI64")) {
        instructionString.append(to_string(consume<int64_t>()));
    } else if (endsWith(&instStr, "I8")
        || endsWith(&instStr, "I16")
        || endsWith(&instStr, "I32")
        || endsWith(&instStr, "I64")) {
        instructionString.append(to_string(consume<int64_t>()));
    } else {
        instructionString.append("<<<ERROR>>>");
    }
}

void MnemonicPrinter::readTypeAndFloat()
{
    auto inst = instructions[pc];
    auto debugInst = static_cast<Instruction>(instructions[pc]);
    auto instStr = AssemblyLexer::instructionStrings[inst];

    instructionString.append(instStr);
    instructionString.append(" ");
    pc += 1;

    if (endsWith(&instStr, "CONSTF32")) {
        instructionString.append(to_string(consume<float>()));
    }
    else if (endsWith(&instStr, "CONSTF64")) {
        instructionString.append(to_string(consume<double>()));
    }
    else if (endsWith(&instStr, "32")) {
        instructionString.append(to_string(consume<int32_t>()));
    } else if (endsWith(&instStr, "64")) {
        instructionString.append(to_string(consume<int64_t>()));
    } else {
        instructionString.append("<<<error>>>");
    }
}
