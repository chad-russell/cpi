#ifndef NODE_H
#define NODE_H

#include <unordered_map>

#include "lexer.h"
#include "util.h"

int32_t typeSize(Node *type);

ostream &operator<<(ostream &os, NodeType type);

Node *makeArrayType(Node *elementType);

Node *wrapInValueParam(Node *value, string name);

Node *wrapInDeclParam(Node *value, string name, int index);

#endif // NODE_H