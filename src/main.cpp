#include <vector>
#include <iostream>
#include <getopt.h>
#include <fstream>
#include <sstream>
#include <chrono>
#include <stdlib.h>

#include "assembler.h"
#include "interpreter.h"
#include "lexer.h"
#include "parser.h"
#include "semantic.h"
#include "bytecodegen.h"
#include "llvmgen.h"
#include "container.h"

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
#include "llvm/Bitcode/BitcodeWriter.h"

using namespace std;
using namespace llvm;

unsigned long nodeId;
unsigned long fnTableId;
int debugFlag;
int noIppFlag;
AtomTable *atomTable;
vector_t<Node *> importedFileModules;

static int printAsmFlag = 0;
static int printAstFlag = 0;
static int interpretFlag = 0;

void printHelp() {
    cout << "Usage: cpi [args] inputFile.[cpi,cas,cbc]"                                      << endl << endl
         << "==========="                                                                    << endl
         << "-- args --"                                                                     << endl
         << "==========="                                                                    << endl
         << "--no-ipp                          Run without implicit context pointer"         << endl
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
    if (endsWith(const_cast<string *>(&fileName), ".cpi")) { return InputType::CPI; }
    if (endsWith(const_cast<string *>(&fileName), ".cas")) { return InputType::CAS; }
    if (endsWith(const_cast<string *>(&fileName), ".cbc")) { return InputType::CBC; }

    printHelp();

    cpi_assert(false);
    return {};
}

int main(int argc, char **argv) {
    nodeId = 0;
    debugFlag = 0;
    noIppFlag = 0;

    atomTable = new AtomTable();
    atomTable->atoms = hash_init<string, int64_t>(1000);

    importedFileModules = vector_init<Node *>(4);

    AssemblyLexer::populateMaps();

    // ./cpi [-o outputFile] [--print-asm] [--debug] inputFile
    static struct option longOptions[] = {
            {"no-ipp",      no_argument,       nullptr,        'c'},
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

        auto c = getopt_long(argc, argv, "pdo:c:n:ih", longOptions, &optionIndex);
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
            case 'c': {
                noIppFlag = 1;
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

    auto inputFile = string(realpath(argv[optind], nullptr));
    auto inputType = inputTypeFromExtension(inputFile);

    unsigned long lastSlash = 0;
    for (unsigned long i = 0; i < inputFile.length(); i++) {
        if (inputFile[i] == '/') {
            lastSlash =  i;
        }
    }
    auto compilerCurrentDir = realpath(inputFile.substr(0, lastSlash).c_str(), nullptr);
    chdir(compilerCurrentDir);

    Semantic *semantic;

    vector<unsigned char> instructions;
    hash_t<uint32_t, uint64_t> *fnTable = nullptr;

    Parser *parser = nullptr;

    auto outputType = OutputType::NONE;
    if (outputFileName != nullptr) {
        std::ofstream out(outputFileName);

        auto outputNameString = new string(outputFileName);

        if (endsWith(outputNameString, ".cas")) {
            outputType = OutputType::CAS;
        } else if (endsWith(outputNameString, ".cbc")) {
            outputType = OutputType::CBC;
        } else if (endsWith(outputNameString, ".ll")) {
            outputType = OutputType::LL;
        } else {
            outputType = OutputType::BINARY;
        }

        out.close();
    }

    Interpreter *interp;

    if (inputType == InputType::CPI) {
        auto lexer = new Lexer(inputFile, true);
        parser = new Parser(lexer);

        auto fileModule = new Node(lexer->srcInfo, NodeType::MODULE, parser->scopes.top());
        vector_append(importedFileModules, fileModule);
        fileModule->moduleData.name = new Node(lexer->srcInfo, NodeType::SYMBOL, parser->scopes.top());
        fileModule->moduleData.fullImportAtomId = atomTable->insertStr(inputFile);
        auto f = inputFile.substr(0, inputFile.length() - 4);
        fileModule->moduleData.name->symbolData.atomId = atomTable->insertStr(f);
        fileModule->moduleData.stmts = parser->allTopLevel;

        parser->parseRoot();

        semantic = new Semantic();
        semantic->lexer = lexer;
        semantic->parser = parser;
        semantic->addStaticIfs(parser->scopes.top());
        semantic->addImports(*parser->imports, *parser->impls);

        // todo(chad): only need to loop through all top level if this is imported. Otherwise just do the main fn
        for (auto tl : parser->allTopLevel) {
            semantic->resolveTypes(tl);
        }
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

            interp = new Interpreter(semantic->linkLibs);
            interp->instructions = gen->instructions;
            interp->fnTable = gen->fnTable;
            interp->sourceMap = gen->sourceMap;
            interp->externalFnTable = gen->externalFnTable;

            instructions = gen->instructions;
            fnTable = gen->fnTable;
        }
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
        for (uint32_t i = 0; i < count; i++) {
            auto fnIndex = bytesTo<uint32_t>(bytes, offset);
            offset += sizeof(uint32_t);

            auto instIndex = bytesTo<uint32_t>(bytes, offset);
            offset += sizeof(uint32_t);

            hash_insert(fnTable, fnIndex, (uint64_t) instIndex);
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
//            cout << "executed " << interp->stepCount << " instructions" << endl;
        }

        cout << "RETURN VALUE: ";

        // todo(chad): look at the return type of the 'main' fn and determine which of these to print!!
        auto mainReturnType = resolve(resolve(parser->mainFn)->fnDeclData.returnType);
        cpi_assert(mainReturnType->type == NodeType::TYPE);

        switch (mainReturnType->typeData.kind) {
            case NodeTypekind::BOOLEAN_LITERAL:
            case NodeTypekind::BOOLEAN: {
                cout << "(bool) " << (interp->readFromStack<int32_t>(0) ? "true" : "false") << endl;
            } break;
            case NodeTypekind::U8: {
                cout << "(u8) " << interp->readFromStack<uint8_t>(0) << endl;
            } break;
            case NodeTypekind::I8: {
                cout << "(i8) " << interp->readFromStack<int8_t>(0) << endl;
            } break;
            case NodeTypekind::U16: {
                cout << "(u16) " << interp->readFromStack<uint16_t>(0) << endl;
            } break;
            case NodeTypekind::I16: {
                cout << "(i16) " << interp->readFromStack<int16_t>(0) << endl;
            } break;
            case NodeTypekind::U32: {
                cout << "(u32) " << interp->readFromStack<uint32_t>(0) << endl;
            } break;
            case NodeTypekind::I32: {
                cout << "(i32) " << interp->readFromStack<int32_t>(0) << endl;
            } break;
            case NodeTypekind::U64: {
                cout << "(u64) " << interp->readFromStack<uint64_t>(0) << endl;
            } break;
            case NodeTypekind::INT_LITERAL:
            case NodeTypekind::I64: {
                cout << "(i64) " << interp->readFromStack<int64_t>(0) << endl;
            } break;
            case NodeTypekind::F32:
            case NodeTypekind::FLOAT_LITERAL: {
                cout << "(f32) " << interp->readFromStack<float>(0) << endl;
            } break;
            case NodeTypekind::F64: {
                cout << "(f64) " << interp->readFromStack<double>(0) << endl;
            } break;
            default: {
                cout << "(other) " << interp->readFromStack<int64_t>(0) << endl;
            }
        }
    }

    if (outputFileName != nullptr && semantic != nullptr && !semantic->encounteredErrors) {
        std::ofstream out(outputFileName);

        auto outputFileNameString = new string(outputFileName);

        if (endsWith(outputFileNameString, ".cas")) {
            auto printer = new MnemonicPrinter(instructions);
            printer->fnTable = fnTable;
            out << printer->debugString();
        } else if (endsWith(outputFileNameString, ".cbc")) {
            // .cbc

            // fn table
            out << toBytes(static_cast<uint32_t>(fnTable->size));
            for (auto i = 0; i < fnTable->bucket_count; i++) {
                auto bucket = fnTable->buckets[i];
                if (bucket != nullptr) {
                    out << toBytes(bucket->key) << toBytes(bucket->value);

                    while (bucket->next != nullptr) {
                        bucket = bucket->next;

                        out << toBytes(bucket->key) << toBytes(bucket->value);
                    }
                }
            }

            // instructions
            out << instructions;
        } else if (endsWith(outputFileNameString, ".ll")) {
            // .ll
            auto llvmGen = new LlvmGen(inputFile.c_str());

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
            auto llvmGen = new LlvmGen(inputFile.c_str());

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
            command << "clang -O0 -L . -o " << outputFileName;
            for (auto link : semantic->linkLibs) {
                // substr(3) to strip "lib" from the prefix
                command << " -l" << link->substr(3);
            }
            command << " output.o";
            system(command.str().c_str());

            // need to keep the .o file because apparently that's where the debug info is stored
            system("rm output.bc");
        }

        out.close();
    }

    return 0;
}