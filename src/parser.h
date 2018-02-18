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
    vector<Node *> allNodes = {};
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
    Node *parseFnDecl();
    Node *parseTypeDecl();
    Node *parseSymbol();
    Node *parseScopedStmt();
    Node *parseIf();
    Node *parseRet();
    Node *parseLvalue();
    Node *parseType();
    Node *parseStructLiteral();
    Node *parseRun();
    Node *parseLvalueOrLiteral();
    Node *parseRvalueSimple();
    Node *parseRvalue();
    Node *parseIntLiteral();
    Node *parseFloatLiteral();
    Node *parseLvalueHelper(Node *symbol, Location saved);
    Node *parseFnCall();
    Node *buildDots(stack<Node *> rvalues);
};

#endif // PARSER_H