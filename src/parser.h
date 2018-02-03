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
    Node *node;
    LexerTokenType type;
};

struct ShuntingYard {
    ShuntingYardType type;
    ShuntingYardData data;
};

struct Parser {
    Lexer *lexer;
    LexerToken last;
    Node *mainFn;
    vector<Node *> allNodes;
    stack<Scope *> scopes;
    vector<Node *> fnDecls;
    Node *currentFnDecl;

    Parser(Lexer *lexer_);

    void popFront();
    void reportError(string error);
    LexerToken expect(LexerTokenType type, string expectation);
    void scopeInsert(int64_t atomId, Node *node);

    bool isBinop(LexerTokenType type);
    bool isBooleanBinop(LexerTokenType type);
    int8_t operatorPrecedence(LexerTokenType type);
    Node *unwindPolish(stack<ShuntingYard> *values);

    void parseRoot();
    Node *parseTopLevel();
    Node *parseFnDecl();
    Node *parseSymbol();
    Node *parseScopedStmt();
    Node *parseRet();
    Node *parseLvalue();
    Node *parseType();
    Node *parseLvalueOrLiteral();
    Node *parseRvalueSimple();
    Node *parseRvalue();
    Node *parseIntLiteral();
    Node *parseFloatLiteral();
};

#endif // PARSER_H