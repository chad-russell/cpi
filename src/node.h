#ifndef NODE_H
#define NODE_H

#include <unordered_map>

#include "lexer.h"
#include "util.h"

enum class NodeType {
    FN_DECL,
    SYMBOL,
    DECL,
    ASSIGN,
    BINOP,
    PIPE,
    DOT,
    FN_CALL,
    INT_LITERAL,
    FLOAT_LITERAL,
    STRING_LITERAL,
    NIL_LITERAL,
    BOOLEAN_LITERAL,
    UNARY_NEG,
    RET,
    IF,
    WHILE,
    DECL_PARAM,
    PARAM,
    TYPE,
    DEREF,
    ADDRESS_OF,
    STRUCT_LITERAL,
    UNARY_NOT,
    MODULE,
    IMPORT,
    CAST,
    ASSERT,
};

enum class NodeTypekind {
    NONE,
    INT_LITERAL,
    I32,
    I64,
    FLOAT_LITERAL,
    STRING,
    BOOLEAN,
    F32,
    F64,
    FN,
    STRUCT,
    SYMBOL,
    POINTER,
};

class Node;

struct fnTypeData {
    vector<Node *> params;
    Node *returnType = nullptr;
};

struct structTypeData {
    bool isLiteral;
    Region name;
    vector<Node *> params;
};

struct pointerTypeData {
    bool isNilLiteral;
    Node *underlyingType;
};

struct symbolTypeData {
    Region region;
};

struct typeData {
    NodeTypekind kind;
    // union {
        fnTypeData fnTypeData;
        structTypeData structTypeData;
        pointerTypeData pointerTypeData;
        symbolTypeData symbolTypeData; 
    // };
};

struct declParamData {
    Region name;
    Node *type;
    Node *initialValue;
};

struct fnParamData {
    Node *name;
    Node *value;
};

struct moduleData {
    Region name;
    vector<Node *> decls;
};

struct importData {
    Node *target;
};

struct fnDeclData {
    Region name;
    vector<Node *> params;
    Node * returnType;
    vector<Node *> body;
    vector<Node *> locals;
    vector<Node *> returns;
    bool isLiteral;
};

struct declData {
    Node *lvalue;
    Node *type;
    Node *initialValue;
};

struct assignData {
    Node *lhs;
    Node *rhs;
};

struct intLiteralData {
    int64_t value;
};

struct floatLiteralData {
    double value;
};

struct boolLiteralData {
    bool value;
};

struct fnCallData {
    Node *fn;
    vector<Node *> params;
};

struct dotData {
    Node *lhs;
    Node *rhs;
};

struct binopData {
    LexerTokenType type;
    Node *lhs;
    Node *rhs;
};

struct pipeData {
    Node *lhs;
    Node *rhs;
};

struct structLiteralData {
    vector<Node *> params;
};

struct ifData {
    Node *condition;
    vector<Node *> stmts;
    vector<Node *> elseStmts;
};

struct whileData {
    Node *condition;
    vector<Node *> stmts;
};

struct castData {
    Node *type;
    Node *value;
};

struct retData {
    Node *value;
};

class Scope {
public:
    unordered_map<string, Node *> symbols;
    Scope *parent;

    Scope(Scope *parent);

    Node *find(Region r);
};

class Node {
public:
    int64_t id;

    Region region;
    Scope *scope;
    Node *typeInfo;
    Node *resolved;
    NodeType type;
    // union {
        Node *nodeData;
        moduleData moduleData;
        fnDeclData fnDeclData;
        declData delcData;
        assignData assignData;
        intLiteralData intLiteralData;
        floatLiteralData floatLiteralData;
        boolLiteralData boolLiteralData;
        fnCallData fnCallData;
        dotData dotData;
        binopData binopData;
        pipeData pipeData;
        declParamData declParamData;
        fnParamData fnParamData;
        typeData typeData;
        structLiteralData structLiteralData;
        ifData ifData;
        whileData whileData;
        importData importData;
        castData castData;
        retData retData;
    // };

    Node(NodeTypekind typekind);
    Node(SourceInfo srcInfo, vector<Node *> *allNodes, NodeType type_, Scope *scope_);
};

Node *resolve(Node *n);

ostream &operator<<(ostream &os, NodeType type);

#endif // NODE_H