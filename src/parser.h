#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include <stack>

#include "util.h"
#include "lexer.h"
#include "node.h"

struct Parser;

extern vector_t<Node *> toSemantic;

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
    vector_t<Node *> allTopLevel;
    stack<Scope *> scopes;
    Node *currentFnDecl = nullptr;
    bool isCopying = false;
    int64_t mainAtom;

    vector_t<Node *> imports = vector_init<Node *>(16);

    vector_t<Node *> staticIfStmts = vector_init<Node *>(16);
    Scope *staticIfScope = nullptr;

    explicit Parser(Lexer *lexer_);

    void popFront();
    void reportError(string error);
    LexerToken expect(LexerTokenType type, string expectation);
    void expectSemicolon();
    void scopeInsert(int64_t atomId, Node *node);
    void appendEndScope(vector_t<Node *> &stmts);

    void possiblyInsertDeferreds(Node *node, bool isReturn);

    int8_t operatorPrecedence(LexerTokenType type);
    Node *unwindPolish(stack<ShuntingYard> *values);
    void addLocal(Node *local);

    void parseRoot();
    Node *parseTopLevel();
    vector_t<Node *> parseDeclParams();
    vector_t<Node *> parseValueParams();
    Node *parseFnDecl();
    Node *parseImport();
    Node *parseTypeDecl();
    Node *parseModuleDecl();
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
    Node *parseRetTypeof();
    Node *parseSizeof();
    Node *parseIsKind();
    Node *parseFieldsof();
    Node *parsePuts();
    Node *parseHeapify();
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
    Node *parseDefer();
};

#endif // PARSER_H