#ifndef NODE_H
#define NODE_H

#include <unordered_map>

#include "lexer.h"
#include "util.h"

int32_t typeSize(Node *type);

ostream &operator<<(ostream &os, NodeType type);

Node *makeArrayType(Node *elementType);

Node *wrapInValueParam(Node *value, Node *name);
Node *wrapInValueParam(Node *value, string name);
Node *wrapInValueParam(Node *value, int64_t atomId);

Node *wrapInDeclParam(Node *value, string name, int index);
Node *wrapInDeclParam(Node *value, Node *name, int index);

void initFnDeclData(Node *node);
void initStructTypeData(Node *node);
void initFnTypeData(Node *node);

#endif // NODE_H