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

    Lexer *lexer = nullptr;
    Parser *parser = nullptr;

    void addStaticIfs(Scope *target, Scope *importInto = nullptr);
    void addImports(vector_t<Node *> imports, vector_t<Node *> impls);

    Node *deepCopyScopedStmt(Node *node, Scope *scope);
    Node *deepCopyRvalue(Node *node, Scope *scope);

    void reportError(vector<Node *> affectedNodes, Error error);
    void resolveTypes(Node *node);
    void addLocal(Node *local);
    Node *findExistingPolymorph(Node *polymorph, vector_t<Node *> givenParams);
};

bool assignParams(Semantic *semantic, Node *errorReportTarget, const vector_t<Node *> &declParams, vector_t<Node *> &givenParams, bool reportError = true);
Node *constantize(Semantic *semantic, Node *node);
bool typesMatch(Node *desired, Node *actual, Semantic *semantic, bool reportError = true);
void maybeStructDefault(Semantic *semantic, Node *rhs, Node *lhsType);

