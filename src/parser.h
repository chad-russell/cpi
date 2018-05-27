#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include <stack>

#include "util.h"
#include "lexer.h"
#include "node.h"

enum class ShuntingYardType {
    NODE,
    OP
};

struct ShuntingYardData {
    Node *node = nullptr;
    LexerTokenType type = {};
};

struct ShuntingYard {
    ShuntingYardType type = {};
    ShuntingYardData data = {};
};

struct Parser {
    Lexer *lexer = nullptr;
    LexerToken last;
    Node *mainFn = nullptr;
    stack<Scope *> scopes = {};
    Node *currentFnDecl = nullptr;

    Parser(Lexer *lexer_);

    void popFront();
    void reportError(string error);
    LexerToken expect(LexerTokenType type, string expectation);
    void expectSemicolon();
    void scopeInsert(int64_t atomId, Node *node);

    int8_t operatorPrecedence(LexerTokenType type);
    Node *unwindPolish(stack<ShuntingYard> *values);
    void addLocal(Node *local);

    void parseRoot();
    Node *parseTopLevel();
    vector<Node *> parseDeclParams();
    vector<Node *> parseValueParams();
    Node *parseFnDecl(bool polymorphCopy = false);
    Node *parseTypeDecl();
    Node *parseSymbol();
    Node *parseScopedStmt();
    Node *parseIf();
    Node *parseWhile();
    Node *parseFor();
    Node *parseRet();
    Node *parseLvalue();
    Node *parseType();
    Node *parseStructLiteral();
    Node *parseArrayLiteral();
    Node *parseRun();
    Node *parseTypeof();
    Node *parseSizeof();
    Node *parseMalloc();
    Node *parseTagCheck();
    Node *parseFree();
    Node *parseLvalueOrLiteral();
    Node *parseRvalueSimple();
    Node *parseRvalue();
    Node *parseIntLiteral();
    Node *parseFloatLiteral();
    Node *parseStringLiteral();
    Node *parseLvalueHelper(Node *symbol, Location saved);
    Node *parseFnCall();
    Node *parseArrayIndex();
    Node *buildDots(stack<Node *> rvalues);
};

#endif // PARSER_H