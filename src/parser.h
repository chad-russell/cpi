#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include <stack>

#include "util.h"
#include "lexer.h"
#include "node.h"

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
    void scopeInsert(Region region, Node *node);

    void parseRoot();
    Node *parseTopLevel();
    Node *parseFnDecl();
    Node *parseSymbol();
    Node *parseScopedStmt();
    Node *parseRet();
    Node *parseLvalue();
    Node *parseType();
    Node *parseLvalueOrLiteral();
    Node *parseRvalue();
    Node *parseIntLiteral();
    Node *parseFloatLiteral();
};

#endif // PARSER_H