#include <vector>

#include "node.h"
#include "parser.h"

class Semantic {
public:
    bool encounteredErrors;
    bool lvalueAssignmentContext = false;
    FnDeclData *currentFnDecl = nullptr;

    Lexer *lexer;

    Node *deepCopy(Node *node, Scope *scope);
    void reportError(vector<Node *> affectedNodes, Error error);
    void resolveTypes(Node *node);
    void addLocal(Node *local);
};

bool assignParams(Semantic *semantic,
                  Node *errorReportTarget,
                  const vector<Node *> &declParams,
                  vector<Node *> &givenParams);
