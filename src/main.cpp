#include <vector>
#include <cstring>
#include <iostream>

#include "assembler.h"
#include "interpreter.h"
#include "util.h"
#include "lexer.h"
#include "parser.h"
#include "semantic.h"
#include "bytecodegen.h"

using namespace std;

int64_t nodeId;

int main() {
    nodeId = 0;

    AtomTable();

    auto lexer = new Lexer("/Users/chadrussell/Projects/cpi/test/foo.cpi");
    auto parser = new Parser(lexer);
    parser->parseRoot();

    auto semantic = new Semantic();
    semantic->resolveTypes(parser->mainFn);

    BytecodeGen *gen = new BytecodeGen();
    gen->gen(parser->mainFn);

    // auto lexer = new AssemblyLexer("/Users/chadrussell/Projects/cpi/test/fact.cas");
    // while (!lexer->empty()) {
    //     lexer->popFront();
    // }

    // for (auto inst : lexer->instructions) {
    //     cout << AssemblyLexer::instructionStrings[inst] << endl;
    // }

//    auto printer = new MnemonicPrinter();
//    // printer->instructions = lexer->instructions;
//    printer->instructions = gen->instructions;
//    cout << printer->debugString() << endl;

    auto interp = new Interpreter();
//    interp->stepCount = -1;

//     interp->instructions = lexer->instructions;
    interp->instructions = gen->instructions;

//    interp->sourceMap = lexer->sourceMap;
    interp->sourceMap = gen->sourceMap;

    interp->srcInfo = lexer->srcInfo;
    interp->interpret();
    cout << interp->readFromStack<int32_t>(0) << endl;

    return 0;
}