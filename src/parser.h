#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include <stack>

#include "util.h"
#include "lexer.h"
#include "node.h"

struct Parser;

extern vector_t<Node *> importedFileModules;

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
    // todo(chad): does this need to be a stack?? or can we keep track of it in the moment?
    stack<Scope *> scopes;
    Node *currentFnDecl = nullptr;
    bool isCopying = false;
    int64_t mainAtom;

    vector_t<Node *> *imports;

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

    Node *createInitContextCall(Scope *scope);
    void initContext(Node *decl);
    void addBasicImport();
    Node *addImport(string importName);
    void addContextParameterForDecl(vector_t<Node *> &currentParams, Scope *scope);

    void parseRoot();
    Node *parseTopLevel();
    vector_t<Node *> parseDeclParams();
    vector_t<Node *> parseValueParams();
    Node *parseFnDecl();
    Node *parseImport();
    Node *parseTypeDecl();
    Node *parseModuleDecl();
    Node *parseScopeDecl();
    Node *parseSymbol();
    Node *parseScopedStmt();
    Node *parseIf();
    Node *parseWhile();
    Node *parseFor();
    Node *parseReturn();
    Node *parseLvalue();
    Node *parsePanic();
    Node *parseType();
    Node *parseStructLiteral();
    Node *parseArrayLiteral();
    Node *parseRun();
    Node *parseTypeof();
    Node *parseReturnTypeof();
    Node *parseSizeof();
    Node *parseIsKind();
    Node *parseFieldsof();
    Node *parsePuts();
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
    Node *parseLink();
};

#endif // PARSER_H