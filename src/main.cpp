#include <vector>
#include <iostream>
#include <getopt.h>
#include <fstream>
#include <sstream>
#include <chrono>

#include "assembler.h"
#include "interpreter.h"
#include "lexer.h"
#include "parser.h"
#include "semantic.h"
#include "bytecodegen.h"
#include "llvmgen.h"

#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/Optional.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Host.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetOptions.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Scalar/GVN.h"
#include "llvm/Bitcode/BitcodeWriter.h"

using namespace std;
using namespace llvm;

unsigned long nodeId;
unsigned long fnTableId;
int debugFlag;

static int printAsmFlag = 0;
static int printAstFlag = 0;
static int interpretFlag = 0;

void printHelp() {
    cout << "Usage: cpi [args] inputFile.[cpi,cas,cbc]"                                      << endl << endl
         << "==========="                                                                    << endl
         << "-- args --"                                                                     << endl
         << "==========="                                                                    << endl
         << "--print-asm   (-p):               Print assembly instructions"                  << endl
         << "--print-ast   (-a):               Print lowered AST"                            << endl
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

enum class OutputType {
    NONE,
    CAS,
    CBC,
    LL,
    BINARY
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
    debugFlag = 0;

    AtomTable();
    AssemblyLexer::populateMaps();

    // ./cpi [-o outputFile] [--print-asm] [--debug] inputFile
    static struct option longOptions[] = {
            {"print-asm",   no_argument,       &printAsmFlag,  'p'},
            {"print-ast",   no_argument,       &printAstFlag,  'a'},
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
    Semantic *semantic;

    vector<unsigned char> instructions;
    unordered_map<uint32_t, uint32_t> fnTable;

    Parser *parser = nullptr;

    auto outputType = OutputType::NONE;
    if (outputFileName != nullptr) {
        std::ofstream out(outputFileName);

        if (endsWith(outputFileName, ".cas")) {
            outputType = OutputType::CAS;
        } else if (endsWith(outputFileName, ".cbc")) {
            outputType = OutputType::CBC;
        } else if (endsWith(outputFileName, ".ll")) {
            outputType = OutputType::LL;
        } else {
            outputType = OutputType::BINARY;
        }

        out.close();
    }

    if (inputType == InputType::CPI) {
        auto lexer = new Lexer(inputFile);
        parser = new Parser(lexer);
        parser->parseRoot();

        auto semantic = new Semantic();
        semantic->lexer = lexer;
        semantic->resolveTypes(parser->mainFn);
        if (semantic->encounteredErrors) { return -1; }

        if (interpretFlag != 0 || outputType == OutputType::CAS || outputType == OutputType::CBC || printAsmFlag != 0) {
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

    if (printAstFlag != 0) {
        cout << parser->mainFn << endl;
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

        cout << "ANSWER: ";

        // todo(chad): look at the return type of the 'main' fn and determine which of these to print!!
        auto mainReturnType = resolve(resolve(parser->mainFn)->fnDeclData.returnType);
        assert(mainReturnType->type == NodeType::TYPE);

        switch (mainReturnType->typeData.kind) {
            case NodeTypekind::I8: {
                cout << interp->readFromStack<int8_t>(0) << endl;
            } break;
            case NodeTypekind::INT_LITERAL:
            case NodeTypekind::I64: {
                cout << interp->readFromStack<int64_t>(0) << endl;
            } break;
            case NodeTypekind::F32:
            case NodeTypekind::FLOAT_LITERAL: {
                cout << interp->readFromStack<float>(0) << endl;
            } break;
            case NodeTypekind::F64: {
                cout << interp->readFromStack<double>(0) << endl;
            } break;
            default: {
                cout << interp->readFromStack<int32_t>(0) << endl;
            }
        }
    }

    if (outputFileName != nullptr && semantic != nullptr && !semantic->encounteredErrors) {
        std::ofstream out(outputFileName);

        if (endsWith(outputFileName, ".cas")) {
            auto printer = new MnemonicPrinter(instructions);
            printer->fnTable = fnTable;
            out << printer->debugString();
        } else if (endsWith(outputFileName, ".cbc")) {
            // .cbc

            // fn table
            out << toBytes(static_cast<uint32_t>(fnTable.size()));
            for (auto t : fnTable) {
                out << toBytes(t.first) << toBytes(t.second);
            }

            // instructions
            out << instructions;
        } else if (endsWith(outputFileName, ".ll")) {
            // .ll
            auto llvmGen = new LlvmGen(inputFile);

            llvmGen->gen(parser->mainFn);
            llvmGen->finalize();

            string outString;
            llvm::raw_string_ostream outStream(outString);
            llvmGen->module->print(outStream, nullptr);

            std::ofstream outFile(outputFileName);
            out << outString;
            out.close();
        } else {
            // assume we are generating an executable
            auto llvmGen = new LlvmGen(inputFile);

            std::error_code EC;
            raw_fd_ostream dest("output.bc", EC, sys::fs::F_None);
            if (EC) {
                errs() << "Could not open file: " << EC.message();
                return 1;
            }

            llvmGen->gen(parser->mainFn);
            llvmGen->finalize();

            WriteBitcodeToFile(llvmGen->module.get(), dest);

            legacy::PassManager pass;
            auto FileType = TargetMachine::CGFT_ObjectFile;

            if (llvmGen->targetMachine->addPassesToEmitFile(pass, dest, FileType)) {
                errs() << "TheTargetMachine can't emit a file of this type";
                return 1;
            }

            pass.run(*llvmGen->module);
            dest.flush();

            system("/usr/local/Cellar/llvm/5.0.1/bin/llc --filetype=obj ./output.bc");

            ostringstream command;
            command << "clang -O0 -o" << outputFileName << " output.o";
            system(command.str().c_str());

            // need to keep the .o file because apparently that's where the debug info is stored
            system("rm output.bc");
        }

        out.close();
    }

    return 0;
}