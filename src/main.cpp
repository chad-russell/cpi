#include <vector>
#include <cstring>
#include <iostream>
#include <getopt.h>
#include <fstream>

#include "assembler.h"
#include "interpreter.h"
#include "util.h"
#include "lexer.h"
#include "parser.h"
#include "semantic.h"
#include "bytecodegen.h"

using namespace std;

int64_t nodeId;

static int printAsmFlag = 0;
static int debugFlag = 0;
static int interpretFlag = 0;

void printHelp() {
    cout << "Usage: cpi [args] inputFile.[cpi,cas,cbc]"                         << endl << endl
         << "==========="                                                       << endl
         << "-- args --"                                                        << endl
         << "==========="                                                       << endl
         << "--print-asm   (-p):               Print assembly instructions"     << endl
         << "--debug       (-d):               Start the program in debug mode" << endl
         << "--output-file (-o) <filename>:    File to write to"                << endl
         << "--interpret   (-i):               Run the interpreter"             << endl
         << "--help        (-h):               Show help"                       << endl;
    exit(1);
}

enum class InputType {
    CPI,
    CAS,
    CBC
};

InputType inputTypeFromExtension(string fileName) {
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
            {nullptr,       0,                 nullptr,        0}
    };

    char *outputFileName = nullptr;

    while (true) {
        int optionIndex;

        auto c = getopt_long(argc, argv, "pdo:h", longOptions, &optionIndex);
        if (c == -1) { break; }
        switch (c) {
            case 0: {
                if (longOptions[optionIndex].flag != nullptr) { break; }
            } break;
            case 'o': {
                outputFileName = optarg;
            } break;
            case 'i': {

            }
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

    std::vector<unsigned char> instructions;

    if (inputType == InputType::CPI) {
        auto lexer = new Lexer(inputFile);
        auto parser = new Parser(lexer);
        parser->parseRoot();

        auto semantic = new Semantic();
        semantic->resolveTypes(parser->mainFn);

        auto gen = new BytecodeGen();
        gen->isMainFn = true;
        gen->sourceMap.sourceInfo = lexer->srcInfo;
        gen->gen(parser->mainFn);
        while (!gen->toProcess.empty()) {
            gen->isMainFn = false;
            gen->gen(gen->toProcess.front());
            gen->toProcess.pop();
        }
        for (auto fixup : gen->fixups) {
            // look, ma! I'm a linker!
            auto node = fixup.second;
            assert(node->type == NodeType::FN_DECL);

            memcpy(&gen->instructions[fixup.first], &node->fnDeclData.instOffset, sizeof(int32_t));
        }

        interp->instructions = gen->instructions;
        interp->sourceMap = gen->sourceMap;

        instructions = gen->instructions;
    }
    else if (inputType == InputType::CAS) {
        auto assemblyLexer = new AssemblyLexer(inputFile);
        while (!assemblyLexer->empty()) {
            assemblyLexer->popFront();
        }

        interp->instructions = assemblyLexer->instructions;
        interp->sourceMap = assemblyLexer->sourceMap;

        instructions = assemblyLexer->instructions;
    }
    else {
        ifstream t(inputFile);
        string fileBytes;

        t.seekg(0, ios::end);
        fileBytes.reserve(t.tellg());
        t.seekg(0, ios::beg);

        fileBytes.assign((istreambuf_iterator<char>(t)),
                         istreambuf_iterator<char>());

        instructions = vector<unsigned char>(fileBytes.length());
        copy(fileBytes.begin(), fileBytes.end(), instructions.begin());
    }

    if (printAsmFlag != 0) {
        auto printer = new MnemonicPrinter();
        printer->instructions = instructions;
        cout << printer->debugString() << endl;
    }

    if (interpretFlag != 0) {
        if (debugFlag == 0) {
            interp->continuing = true;
        }
        interp->instructions = instructions;
        interp->interpret();

//        cout << "STACK:" << endl;
//        for (auto i = 0; i < interp->stack.size(); i++) {
//            cout << static_cast<int32_t>(interp->stack[i]) << ", ";
//        }
//        cout << endl;
        cout << interp->readFromStack<int32_t>(0) << endl;
    }

    if (outputFileName != nullptr) {
        auto writeAssembly = endsWith(outputFileName, ".cas");
        if (!writeAssembly && !endsWith(outputFileName, ".cbc")) {
            cout << "output file name must be .cas or .cbc" << endl;
            exit(1);
        }

        std::ofstream out(outputFileName);

        if (writeAssembly) {
            auto printer = new MnemonicPrinter();
            printer->instructions = instructions;
            out << printer->debugString();
        } else {
            for (auto inst : instructions) {
                out << inst;
            }
        }

        out.close();
    }

    return 0;
}