#ifndef NODE_H
#define NODE_H

#include <unordered_map>

#include "lexer.h"
#include "util.h"

int32_t typeSize(Node *type);

ostream &operator<<(ostream &os, NodeType type);

#endif // NODE_H