#ifndef NODE_H
#define NODE_H

#include <unordered_map>

#include "lexer.h"
#include "util.h"

int32_t typeSize(Node *type);
int32_t typeAlign(Node *type);

ostream &operator<<(ostream &os, NodeType type);

Node *makeArrayType(Node *elementType);

Node *wrapInValueParam(Node *value, Node *name);
Node *wrapInValueParam(Node *value, string name);
Node *wrapInValueParam(Node *value, int64_t atomId);

Node *wrapInDeclParam(Node *type, string name, int index);
Node *wrapInDeclParam(Node *type, Node *name, int index);

void initForData(Node *node);
void initArrayLiteralData(Node *node);
void initCastData(Node *node);
void initWhileData(Node *node);
void initIfData(Node *node);
void initStructLiteralData(Node *node);
void initDotData(Node *node);
void initBinopData(Node *node);
void initFnCallData(Node *node);
void initAssignData(Node *node);
void initDeclData(Node *node);
void initParamData(Node *node);
void initFnDeclData(Node *node);
void initStructTypeData(Node *node);
void initFnTypeData(Node *node);
void initModuleData(Node *node);
void initDeferData(Node *node);
void initEndScopeData(Node *node);
void initTypeData(Node *node);

#endif // NODE_H