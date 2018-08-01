#include <vector>

#include "node.h"
#include "parser.h"

class Semantic {
public:
    bool encounteredErrors;
    bool lvalueAssignmentContext = false;
    Node *currentFnDecl = nullptr;

    Lexer *lexer;
    Parser *parser;

    void addImports();

    Node *deepCopyScopedStmt(Node *node, Scope *scope);
    Node *deepCopyRvalue(Node *node, Scope *scope);

    void reportError(vector<Node *> affectedNodes, Error error);
    void resolveTypes(Node *node);
    void addLocal(Node *local);
};

bool assignParams(Semantic *semantic,
                  Node *errorReportTarget,
                  const vector_t<Node *> &declParams,
                  vector_t<Node *> &givenParams,
                  bool isCt = false);
