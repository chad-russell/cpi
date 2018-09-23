#ifndef SEMANTIC_H
#define SEMANTIC_H

#include <vector>

#include "node.h"
#include "parser.h"

struct Polymorphed {
    Node *target;
    vector_t<Node *> givenParams;

    Node *result;
};

class Semantic {
public:
    bool encounteredErrors = false;
    bool lvalueAssignmentContext = false;
    Node *currentFnDecl = nullptr;

    vector_t<string *> linkLibs = vector_init<string *>(4);
    vector_t<Polymorphed> polymorphs = vector_init<Polymorphed>(32);

    vector_t<Node *> contexts = vector_init<Node *>(16);
    vector_t<Node *> contextInits = vector_init<Node *>(16);
    Node *contextType = nullptr;

    vector_t<StructTypeData> structsToSize = vector_init<StructTypeData>(256);

    Lexer *lexer = nullptr;
    Parser *parser = nullptr;

    void addStaticIfs(Scope *target, Scope *importInto = nullptr);
    void addImports(vector_t<Node *> imports);

    Node *deepCopyScopedStmt(Node *node, Scope *scope);
    Node *deepCopyRvalue(Node *node, Scope *scope);
    Node *makeContextType();

    void reportError(vector<Node *> affectedNodes, Error error);
    void resolveTypes(Node *node);
    void addLocal(Node *local);
    Node *findExistingPolymorph(Node *polymorph, vector_t<Node *> givenParams);
    void sizeStructs();
};

bool assignParams(Semantic *semantic, Node *errorReportTarget, const vector_t<Node *> &declParams, vector_t<Node *> &givenParams, bool reportError = true);
Node *constantize(Semantic *semantic, Node *node);
bool typesMatch(Node *desired, Node *actual, Semantic *semantic, bool reportError = true);
void maybeStructDefault(Semantic *semantic, Node *rhs, Node *lhsType);

#endif // SEMANTIC_H
