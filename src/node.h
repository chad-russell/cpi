#ifndef NODE_H
#define NODE_H

#include <unordered_map>

#include "lexer.h"
#include "util.h"

class Node;

struct FnTypeData {
    vector<Node *> params;
    Node *returnType = nullptr;
};

struct StructTypeData {
    bool isLiteral;
    int64_t atomId;
    vector<Node *> params;
};

struct PointerTypeData {
    bool isNilLiteral;
    Node *underlyingType;
};

struct SymbolTypeData {
    int64_t atomId;
};

struct TypeData {
    // todo(chad): size?

    NodeTypekind kind;
//     union {
        FnTypeData fnTypeData;
        StructTypeData structTypeData;
        PointerTypeData pointerTypeData;
        SymbolTypeData symbolTypeData;
//     };
};

struct DeclParamData {
    int64_t atomId;
    Node *type;
    Node *initialValue;
};

struct FnParamData {
    Node *name;
    Node *value;
};

struct ModuleData {
    int64_t atomId;
    vector<Node *> decls;
};

struct ImportData {
    Node *target;
};

struct FnDeclData {
    int64_t atomId;
    vector<Node *> params;
    Node * returnType;
    vector<Node *> body;
    vector<Node *> locals;
    vector<Node *> returns;

    int64_t stackSize;
    unsigned long instOffset;
    bool isLiteral;
    unsigned long tableIndex;
};

struct DeclData {
    Node *lvalue;
    Node *type;
    Node *initialValue;
};

struct AssignData {
    Node *lhs;
    Node *rhs;
};

struct IntLiteralData {
    int64_t value;
};

struct FloatLiteralData {
    double value;
};

struct BoolLiteralData {
    bool value;
};

struct FnCallData {
    Node *fn;
    vector<Node *> params;
};

struct DotData {
    Node *lhs;
    Node *rhs;
};

struct BinopData {
    LexerTokenType type;
    Node *lhs;
    Node *rhs;
};

struct PipeData {
    Node *lhs;
    Node *rhs;
};

struct StructLiteralData {
    vector<Node *> params;
};

struct IfData {
    Node *condition;
    vector<Node *> stmts;
    vector<Node *> elseStmts;
};

struct WhileData {
    Node *condition;
    vector<Node *> stmts;
};

struct CastData {
    Node *type;
    Node *value;
};

struct RetData {
    Node *value;
};

struct SymbolData {
    int64_t atomId;
};

class Scope {
public:
    unordered_map<int64_t, Node *> symbols;
    Scope *parent;

    Scope(Scope *parent);

    Node *find(int64_t atomId);
};

class Node {
public:
    unsigned long id;

    Region region;
    Scope *scope;
    Node *typeInfo;
    Node *resolved;
    NodeType type;

    // union {
        Node *nodeData;
        ModuleData moduleData;
        FnDeclData fnDeclData;
        DeclData declData;
        AssignData assignData;
        IntLiteralData intLiteralData;
        FloatLiteralData floatLiteralData;
        BoolLiteralData boolLiteralData;
        FnCallData fnCallData;
        DotData dotData;
        BinopData binopData;
        PipeData pipeData;
        DeclParamData declParamData;
        FnParamData fnParamData;
        TypeData typeData;
        StructLiteralData structLiteralData;
        IfData ifData;
        WhileData whileData;
        ImportData importData;
        CastData castData;
        RetData retData;
        SymbolData symbolData;
    // };

    bool isUsedInError = false;
    bool semantic = false;
    bool gen = false;
    bool sourceMapStatement = false;

    // todo(chad): better way to store this?
    vector<unsigned char> bytecode;
    int64_t localOffset;

    Node(NodeTypekind typekind);
    Node(SourceInfo srcInfo, vector<Node *> *allNodes, NodeType type_, Scope *scope_);
};

Node *resolve(Node *n);

ostream &operator<<(ostream &os, NodeType type);

#endif // NODE_H