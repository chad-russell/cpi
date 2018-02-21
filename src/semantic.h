#include <vector>

#include "node.h"
#include "parser.h"

class Semantic {
public:
    bool encounteredErrors;
    FnDeclData *currentFnDecl = nullptr;
    Node *targetType = nullptr;

    Lexer *lexer;

    Node *deepCopy(Node *node, Scope *scope);
    void reportError(vector<Node *> affectedNodes, Error error);
    void resolveTypes(Node *node);
};