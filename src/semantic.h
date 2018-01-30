#include <vector>

#include "node.h"

class Semantic {
public:
    bool encounteredErrors;
    fnDeclData currentFnDecl;
    Node *targetType;

    void resolveTypes(Node *node);
};