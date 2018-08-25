#include <vector>

#include "node.h"
#include "parser.h"

class Semantic {
public:
    bool encounteredErrors = false;
    bool lvalueAssignmentContext = false;
    Node *currentFnDecl = nullptr;

    vector_t<string *> linkLibs;

    Lexer *lexer = nullptr;
    Parser *parser = nullptr;

    void addStaticIfs(Scope *target, Scope *importInto = nullptr);
    void addImports(vector_t<Node *> imports);

    Node *deepCopyScopedStmt(Node *node, Scope *scope);
    Node *deepCopyRvalue(Node *node, Scope *scope);

    void reportError(vector<Node *> affectedNodes, Error error);
    void resolveTypes(Node *node);
    void addLocal(Node *local);
};

bool assignParams(Semantic *semantic, Node *errorReportTarget, const vector_t<Node *> &declParams, vector_t<Node *> &givenParams);
Node *constantize(Semantic *semantic, Node *node);
