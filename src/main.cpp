#include <vector>
#include <iostream>
#include <getopt.h>
#include <fstream>
#include <sstream>

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

using namespace std;
using namespace llvm;

unsigned long nodeId;
unsigned long fnTableId;
int debugFlag;

static int printAsmFlag = 0;
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
    debugFlag = 0;

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

    Parser *parser = nullptr;

    if (inputType == InputType::CPI) {
        auto lexer = new Lexer(inputFile);
        parser = new Parser(lexer);
        parser->parseRoot();

        auto semantic = new Semantic();
        semantic->lexer = lexer;
        semantic->resolveTypes(parser->mainFn);
        if (semantic->encounteredErrors) { return -1; }

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
    }

    if (outputFileName != nullptr) {
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
            auto llvmGen = new LlvmGen();
            llvmGen->gen(parser->mainFn);
            llvmGen->module->print(llvm::errs(), nullptr);
        } else {
            // assume we are generating an executable
            auto llvmGen = new LlvmGen();
            llvmGen->gen(parser->mainFn);

            InitializeAllTargetInfos();
            InitializeAllTargets();
            InitializeAllTargetMCs();
            InitializeAllAsmParsers();
            InitializeAllAsmPrinters();

            auto TargetTriple = sys::getDefaultTargetTriple();
            llvmGen->module->setTargetTriple(TargetTriple);

            std::string Error;
            auto Target = TargetRegistry::lookupTarget(TargetTriple, Error);

            // Print an error and exit if we couldn't find the requested target.
            // This generally occurs if we've forgotten to initialise the
            // TargetRegistry or we have a bogus target triple.
            if (!Target) {
                errs() << Error;
                return 1;
            }

            auto CPU = "generic";
            auto Features = "";

            TargetOptions opt;
            auto RM = llvm::Optional<Reloc::Model>();
            auto TheTargetMachine = Target->createTargetMachine(TargetTriple, CPU, Features, opt, RM);

            llvmGen->module->setDataLayout(TheTargetMachine->createDataLayout());

            auto targetingObj = endsWith(outputFileName, ".o");
            auto outputObjName = targetingObj ? outputFileName : "output.o";

            std::error_code EC;
            raw_fd_ostream dest(outputObjName, EC, sys::fs::F_None);

            if (EC) {
                errs() << "Could not open file: " << EC.message();
                return 1;
            }

            legacy::PassManager pass;
            auto FileType = TargetMachine::CGFT_ObjectFile;

            if (TheTargetMachine->addPassesToEmitFile(pass, dest, FileType)) {
                errs() << "TheTargetMachine can't emit a file of this type";
                return 1;
            }

            pass.run(*llvmGen->module);
            dest.flush();

            if (!targetingObj) {
                ostringstream command;
                command << "clang -o " << outputFileName << " output.o";
                system(command.str().c_str());

                system("rm output.o");
            }
        }

        out.close();
    }

    return 0;
}