#include <vector>

#include "node.h"

class Semantic {
public:
    bool encounteredErrors;
    FnDeclData *currentFnDecl = nullptr;
    Node *targetType = nullptr;

    void reportError(vector<Node *> affectedNodes, Error error);

    void resolveTypes(Node *node);
};