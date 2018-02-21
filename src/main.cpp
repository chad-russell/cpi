#include <vector>
#include <cstring>
#include <iostream>
#include <getopt.h>
#include <fstream>
#include <sstream>

#include "assembler.h"
#include "interpreter.h"
#include "util.h"
#include "lexer.h"
#include "parser.h"
#include "semantic.h"
#include "bytecodegen.h"

using namespace std;

unsigned long nodeId;
unsigned long fnTableId;

static int printAsmFlag = 0;
static int debugFlag = 0;
static int interpretFlag = 0;

void printHelp() {
    cout << "Usage: cpi [args] inputFile.[cpi,cas,cbc]"                                      << endl << endl
         << "==========="                                                                    << endl
         << "-- args --"                                                                     << endl
         << "==========="                                                                    << endl
         << "--print-asm   (-p):               Print assembly instructions"                  << endl
         << "--debug       (-d):               Start the program in debug mode"              << endl
         << "--output-file (-o) <filename>:    File to write to"                             << endl
         << "--interpret   (-i):               Run the interpreter"                          << endl
         << "--n-times     (-n):               Run interpreter n times (for benchmarking)"   << endl
         << "--help        (-h):               Show help"                                    << endl;
    exit(1);
}

enum class InputType {
    CPI,
    CAS,
    CBC
};

InputType inputTypeFromExtension(const string &fileName) {
    if (endsWith(fileName, ".cpi")) { return InputType::CPI; }
    if (endsWith(fileName, ".cas")) { return InputType::CAS; }
    if (endsWith(fileName, ".cbc")) { return InputType::CBC; }

    printHelp();
    assert(false);
}

int main(int argc, char **argv) {
    nodeId = 0;

    AtomTable();
    AssemblyLexer::populateMaps();

    // ./cpi [-o outputFile] [--print-asm] [--debug] inputFile
    static struct option longOptions[] = {
            {"print-asm",   no_argument,       &printAsmFlag,  'p'},
            {"debug",       no_argument,       &debugFlag,     'd'},
            {"output-file", required_argument, nullptr,        'o'},
            {"interpret",   no_argument,       &interpretFlag, 'i'},
            {"help",        no_argument,       nullptr,        'h'},
            {"n-times",     required_argument, nullptr,        'n'},
            {nullptr,       0,                 nullptr,        0}
    };

    char *outputFileName = nullptr;
    int nTimes = 1;

    while (true) {
        int optionIndex;

        auto c = getopt_long(argc, argv, "pdo:n:ih", longOptions, &optionIndex);
        if (c == -1) { break; }
        switch (c) {
            case 0: {
                if (longOptions[optionIndex].flag != nullptr) { break; }
            } break;
            case 'o': {
                outputFileName = optarg;
            } break;
            case 'n': {
                nTimes = atoi(optarg);
            } break;
            case 'i':
            case 'h':
            case '?':
            default: {
                printHelp();
                abort();
            }
        }
    }

    if (argc != optind + 1) {
        printHelp();
    }

    auto inputFile = argv[optind];
    auto inputType = inputTypeFromExtension(inputFile);

    auto interp = new Interpreter();

    vector<unsigned char> instructions;
    unordered_map<uint32_t, uint32_t> fnTable;

    if (inputType == InputType::CPI) {
        auto lexer = new Lexer(inputFile);
        auto parser = new Parser(lexer);
        parser->parseRoot();

        auto semantic = new Semantic();
        semantic->lexer = lexer;
        semantic->resolveTypes(parser->mainFn);

        auto gen = new BytecodeGen();
        gen->isMainFn = true;
        gen->sourceMap.sourceInfo = lexer->srcInfo;
        gen->processFnDecls = true;

        gen->gen(parser->mainFn);
        while (!gen->toProcess.empty()) {
            gen->isMainFn = false;
            gen->processFnDecls = true;
            gen->gen(gen->toProcess.front());
            gen->toProcess.pop();
        }
        gen->fixup();

        interp->instructions = gen->instructions;
        interp->fnTable = gen->fnTable;
        interp->sourceMap = gen->sourceMap;

        instructions = gen->instructions;
        fnTable = gen->fnTable;
    }
    else if (inputType == InputType::CAS) {
        auto assemblyLexer = new AssemblyLexer(inputFile);
        while (!assemblyLexer->empty()) {
            assemblyLexer->popFront();
        }

        interp->instructions = assemblyLexer->instructions;
        interp->fnTable = assemblyLexer->fnTable;
        interp->sourceMap = assemblyLexer->sourceMap;

        instructions = assemblyLexer->instructions;
        fnTable = assemblyLexer->fnTable;
    }
    else {
        ifstream t(inputFile);
        string fileBytes;

        t.seekg(0, ios::end);
        fileBytes.reserve(static_cast<unsigned long>(t.tellg()));
        t.seekg(0, ios::beg);

        fileBytes.assign((istreambuf_iterator<char>(t)),
                         istreambuf_iterator<char>());

        auto bytes = vector<unsigned char>(fileBytes.length());
        copy(fileBytes.begin(), fileBytes.end(), bytes.begin());

        auto offset = 0;
        auto count = bytesTo<uint32_t>(bytes, 0);
        offset += sizeof(uint32_t);
        for (auto i = 0; i < count; i++) {
            auto fnIndex = bytesTo<uint32_t>(bytes, offset);
            offset += sizeof(uint32_t);

            auto instIndex = bytesTo<uint32_t>(bytes, offset);
            offset += sizeof(uint32_t);

            fnTable.insert({fnIndex, instIndex});
        }

        instructions = vector<unsigned char>(bytes.size() - offset);
        memcpy(&instructions[0], &bytes[offset], bytes.size() - offset);
    }

    if (printAsmFlag != 0) {
        auto printer = new MnemonicPrinter(instructions);
        printer->fnTable = fnTable;
        cout << printer->debugString() << endl;
    }

    if (interpretFlag != 0) {
        if (debugFlag == 0) {
            interp->continuing = true;
        }
        interp->instructions = instructions;
        interp->fnTable = fnTable;

        if (nTimes > 1) {
            cout << "running interpreter " << nTimes << " times..." << endl;
        }

        for (int i = 0; i < nTimes; i++) {
            interp->terminated = false;
            interp->pc = 0;
            interp->sp = 0;
            interp->bp = 0;

            interp->interpret();
        }

        cout << interp->readFromStack<int32_t>(0) << endl;
//        cout << "step count: " << interp->stepCount << endl;
    }

    if (outputFileName != nullptr) {
        auto writeAssembly = endsWith(outputFileName, ".cas");
        if (!writeAssembly && !endsWith(outputFileName, ".cbc")) {
            cout << "output file name must be .cas or .cbc" << endl;
            exit(1);
        }

        std::ofstream out(outputFileName);

        if (writeAssembly) {
            auto printer = new MnemonicPrinter(instructions);
            printer->fnTable = fnTable;
            out << printer->debugString();
        } else {
            // .cbc

            // fn table
            out << toBytes(static_cast<uint32_t>(fnTable.size()));
            for (auto t : fnTable) {
                out << toBytes(t.first) << toBytes(t.second);
            }

            // instructions
            out << instructions;
        }

        out.close();
    }

    return 0;
}