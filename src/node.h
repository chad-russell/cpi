#ifndef NODE_H
#define NODE_H

#include <unordered_map>

#include "lexer.h"
#include "util.h"

class Node;

struct FnTypeData {
    vector<Node *> params = {};
    Node *returnType = nullptr;
};

struct StructTypeData {
    bool isLiteral;
    Node *name = nullptr;
    vector<Node *> params = {};
};

struct PointerTypeData {
    bool isNilLiteral = false;
    Node *underlyingType = nullptr;
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
    Node *name = nullptr;
    Node *type = nullptr;
    Node *initialValue = nullptr;
};

struct ParamData {
    Node *name = nullptr;
    Node *value = nullptr;
};

struct ModuleData {
    Node *name = nullptr;
    vector<Node *> decls = {};
};

struct ImportData {
    Node *target = nullptr;
};

struct FnDeclData {
    Node *name = nullptr;
    vector<Node *> params = {};
    Node * returnType = nullptr;
    vector<Node *> body = {};
    vector<Node *> locals = {};
    vector<Node *> returns = {};

    int32_t stackSize = 0;
    unsigned long instOffset;
    bool isLiteral;
    unsigned long tableIndex;
};

struct DeclData {
    Node *lvalue = nullptr;
    Node *type = nullptr;
    Node *initialValue = nullptr;
};

struct AssignData {
    Node *lhs = nullptr;
    Node *rhs = nullptr;
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
    Node *fn = nullptr;
    vector<Node *> params = {};
};

struct DotData {
    Node *lhs = nullptr;
    Node *rhs = nullptr;
    Node *resolved = nullptr;

    bool autoDeref = false;
    Node *autoDerefStorage = nullptr;

    Node *nodeLocalStorage = nullptr;
};

struct BinopData {
    LexerTokenType type;
    Node *lhs = nullptr;
    Node *rhs = nullptr;
};

struct PipeData {
    Node *lhs = nullptr;
    Node *rhs = nullptr;
};

struct StructLiteralData {
    vector<Node *> params = {};
};

struct IfData {
    Node *condition = nullptr;
    vector<Node *> stmts = {};
    vector<Node *> elseStmts = {};
};

struct WhileData {
    Node *condition = nullptr;
    vector<Node *> stmts = {};
};

struct CastData {
    Node *type = nullptr;
    Node *value = nullptr;
};

struct RetData {
    Node *value = nullptr;
};

struct SymbolData {
    int64_t atomId;
};

class Scope {
public:
    unordered_map<int64_t, Node *> symbols = {};
    Scope *parent = {};

    Scope(Scope *parent);

    Node *find(int64_t atomId);
};

int32_t typeSize(Node *type);

class Node {
public:
    unsigned long id;

    Region region = {};
    Scope *scope = nullptr;
    Node *typeInfo = nullptr;
    Node *resolved = nullptr;
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
        ParamData paramData;
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
    bool isLocal = false;

    // the offset of the base type from the base pointer
    int32_t localOffset = 0;

    // when we finally do a storage, what is the offset?
    // (mostly for storing structs)
    int32_t localStorageOffset = 0;

    int32_t fullOffset();

    Node();
    Node(NodeTypekind typekind);
    Node(SourceInfo srcInfo, vector<Node *> *allNodes, NodeType type_, Scope *scope_);
};

Node *resolve(Node *n);

ostream &operator<<(ostream &os, NodeType type);

#endif // NODE_H