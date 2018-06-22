#ifndef UTIL_H
#define UTIL_H

#include <string>
#include <cassert>
#include <vector>
#include <iostream>
#include <unordered_map>
#include <memory.h>

#include "container.h"

using namespace std;

extern unsigned long nodeId;
extern unsigned long fnTableId;
extern int debugFlag;

const char *readFile(const char *fileName);

bool startsWith(string *s, unsigned long startPos, string pre);
bool startsWith(string *s, string pre);

bool endsWith(string *s, string suf);

int bytesInCodepoint(char firstByte);

class Node;
class Scope;

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
    BOOLEAN,
    UNARY_NEG,
    RET,
    IF,
    WHILE,
    DECL_PARAM,
    VALUE_PARAM,
    TYPE,
    DEREF,
    ADDRESS_OF,
    STRUCT_LITERAL,
    UNARY_NOT,
    MODULE,
    IMPORT,
    CAST,
    PANIC,
    RUN,
    TYPEOF,
    RETTYPEOF,
    SIZEOF,
    FIELDSOF,
    ARRAY_INDEX,
    MALLOC,
    FREE,
    PUTS,
    TAGCHECK,
    ARRAY_LITERAL,
    FOR,
    HEAPIFY,
    ISKIND,
};

enum class NodeTypekind {
    NONE,
    INT_LITERAL,
    I8,
    I32,
    I64,
    FLOAT_LITERAL,
    F32,
    F64,
    FN,
    BOOLEAN_LITERAL,
    BOOLEAN,
    STRUCT,
    SYMBOL,
    POINTER,
    EXPOSED_AST,
};

enum class LexerTokenType : int32_t {
    EOF_,
    COMMENT,
    LCURLY,
    RCURLY,
    LPAREN,
    RPAREN,
    LSQUARE,
    RSQUARE,
    SUB,
    ADD,
    MUL,
    DIV,
    DEREF,
    EQ_EQ,
    NE,
    LE,
    GE,
    LT,
    GT,
    AMP,
    DOT,
    COLON_EQ,
    COLON,
    EQ,
    COMMA,
    SINGLE_QUOTE,
    DOUBLE_QUOTE,
    BACK_TICK,
    VERTICAL_BAR,
    AT,
    FN,
    TYPE,
    STRUCT,
    ENUM,
    SYMBOL,
    INT_LITERAL,
    FLOAT_LITERAL,
    RET,
    STRING,
    BOOLEAN,
    I8,
    I32,
    I64,
    F32,
    F64,
    IF,
    WHILE,
    ELSE,
    TRUE_,
    FALSE_,
    AND,
    OR,
    NOT,
    NIL,
    MODULE,
    IMPORT,
    CAST,
    SEMICOLON,
    RUN,
    EXPOSED_AST,
    TYPEOF,
    RETTYPEOF,
    SIZEOF,
    FIELDSOF,
    PANIC,
    NONE,
    MALLOC,
    FREE,
    PUTS,
    TAGCHECK,
    FOR,
    STATIC_FOR,
    HEAP,
    ISKIND,
};

struct SourceInfo {
    string *fileName = nullptr;
    string *source = nullptr;

    vector_t<unsigned long> lines = vector_init<unsigned long>(100);
};

struct Location {
    unsigned long byteIndex;
    unsigned long line = 1;
    unsigned long col = 1;
};

struct Region {
    SourceInfo srcInfo = {};
    Location start = {};
    Location end = {};
};

struct FnTypeData {
    vector_t<Node *> params;
    Node *returnType;
};

struct StructTypeData {
    bool isLiteral;
    vector_t<Node *> params;

    bool isSecretlyArray;
    Node *secretArrayElementType;

    bool isSecretlyEnum;

    Node *coercedType;
};

struct PointerTypeData {
    Node *underlyingType;
};

struct SymbolTypeData {
    int64_t atomId;
};

struct TypeData {
    NodeTypekind kind;

    union {
        FnTypeData fnTypeData;
        StructTypeData structTypeData;
        PointerTypeData pointerTypeData;
        SymbolTypeData symbolTypeData;
        double floatTypeData;
        int64_t intTypeData;
        bool boolTypeData;
    };
};

struct ParamData {
    Node *name;
    Node *type;
    Node *value;

    Node *polyLink;

    int64_t index;
};

struct FnDeclData {
    Node *name;
    vector_t<Node *> ctParams;
    vector_t<Node *> params;
    Node * returnType;
    vector_t<Node *> body;
    vector_t<Node *> locals;
    vector_t<Node *> returns;

    int32_t stackSize;
    uint64_t instOffset;
    bool isLiteral;
    uint32_t tableIndex;
    bool cameFromPolymorph;
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
    vector_t<Node *> ctParams;
    vector_t<Node *> params;

    bool hasRuntimeParams;
};

struct ArrayIndexData {
    Node *target;
    Node *indexValue;
};

struct DotData {
    Node *lhs;
    Node *rhs;
    Node *resolved;

    Node *autoDerefStorage;
    bool pointerIsRelative;
};

struct BinopData {
    LexerTokenType type;

    Node *lhs;
    Node *rhs;

    int rhsScale;

    Node *lhsTemporary;
    Node *rhsTemporary;
};

struct StructLiteralData {
    vector_t<Node *> params;
};

struct StringLiteralData {
    string value;
};

struct IfData {
    Node *condition;
    vector_t<Node *> stmts;
    vector_t<Node *> elseStmts;
};

struct WhileData {
    Node *condition;
    vector_t<Node *> stmts;
};

struct CastData {
    Node *type = nullptr;
    Node *value = nullptr;

    // @Hack??
    bool isCastFromArrayToDataPtr = false;
};

struct RetData {
    Node *value = nullptr;
};

struct SymbolData {
    int64_t atomId;
};

struct DerefData {
    Node *target = nullptr;
};

struct ArrayLiteralData {
    Node *elementType = nullptr;
    vector_t<Node *> elements = vector_init<Node *>(10);

    Node *structLiteralRepresentation;
};

struct ForData {
    Node *element_alias = nullptr;
    Node *iterator_alias = nullptr;
    Node *target = nullptr;

    vector_t<Node *> stmts = vector_init<Node *>(10);;

    vector_t<Node *> rewritten = vector_init<Node *>(10);;

    bool isStatic = false;
    vector_t<Node *> staticStmts = vector_init<Node *>(10);;
};

struct IsKindData {
    Node *type;
    LexerTokenType tokenType;
};

class Scope {
public:
    hash_t<int64_t, Node *> *symbols;
    Scope *parent = nullptr;

    explicit Scope(Scope *parent);

    Node *find(int64_t atomId);
};

union TDF {
    Node *nodeData;
    FnDeclData fnDeclData;
//    DeclData declData;
//    AssignData assignData;
//    IntLiteralData intLiteralData;
//    FloatLiteralData floatLiteralData;
//    BoolLiteralData boolLiteralData;
//    FnCallData fnCallData;
//    ArrayIndexData arrayIndexData;
//    DotData dotData;
//    BinopData binopData;
//    ParamData paramData;
//    TypeData typeData;
//    StructLiteralData structLiteralData;
//    StringLiteralData stringLiteralData;
//    IfData ifData;
//    WhileData whileData;
//    CastData castData;
//    RetData retData;
//    SymbolData symbolData;
//    DerefData derefData;
//    ArrayLiteralData arrayLiteralData;
//    ForData forData;
//    IsKindData isKindData;
};

class Node {
public:
    unsigned long id;

    Region region = {};
    Scope *scope = nullptr;
    Node *typeInfo = nullptr;
    Node *resolved = nullptr;
    NodeType type;

    vector_t<Node *> preStmts = vector_init<Node *>(10);;
    vector_t<Node *> postStmts = vector_init<Node *>(10);;

    // union {
    Node *nodeData;
    FnDeclData fnDeclData;
    DeclData declData;
    AssignData assignData;
    IntLiteralData intLiteralData;
    FloatLiteralData floatLiteralData;
    BoolLiteralData boolLiteralData;
    FnCallData fnCallData;
    ArrayIndexData arrayIndexData;
    DotData dotData;
    BinopData binopData;
    ParamData paramData;
    TypeData typeData;
    StructLiteralData structLiteralData;
    StringLiteralData stringLiteralData;
    IfData ifData;
    WhileData whileData;
    CastData castData;
    RetData retData;
    SymbolData symbolData;
    DerefData derefData;
    ArrayLiteralData arrayLiteralData;
    ForData forData;
    IsKindData isKindData;
    // };

    Node *staticValue = nullptr;

    bool isUsedInError = false;
    bool semantic = false;
    bool printed = false;
    bool tagCheck = false;
    bool gen = false;
    bool llvmGen = false;
    bool sourceMapStatement = false;

    // todo(chad): better way to store this?
    vector<unsigned char> bytecode;
    bool isLocal = false;

    bool isBytecodeLocal = false;
    bool skipAllButPostStmts = false;

    // the offset of the storage for this node from the current base pointer
    // todo(chad): make this 64 bits
    int32_t localOffset = 0;

    void *llvmLocal = nullptr;
    void *llvmData = nullptr;

    Node();
    explicit Node(NodeTypekind typekind);
    Node(SourceInfo srcInfo, NodeType type_, Scope *scope_);
};

template<typename T>
class Optional {
public:
    bool isPresent;
    T value;
};

template<typename T> 
vector<unsigned char> toBytes(T object) {
    vector<unsigned char> bytes;
    bytes.reserve(sizeof(T));

    const auto *beg = reinterpret_cast<const unsigned char *>(addressof(object));
    const auto *end = beg + sizeof(T);

    copy(beg, end, back_inserter(bytes));

    return bytes;
}

template<typename T>
vector<unsigned char> toBytes32(const T object) {
    return toBytes(static_cast<int32_t>(object));
}

template<typename T>
vector<unsigned char> toBytes64(const T object) {
    return toBytes(static_cast<int64_t>(object));
}

template<typename T>
T bytesTo(const vector<unsigned char> &bytes, int32_t start) {
    return *(reinterpret_cast<const T *>(&bytes[start]));
}

template<>
inline int64_t bytesTo(const vector<unsigned char> &bytes, int32_t start) {
    return *(reinterpret_cast<const int64_t *>(&bytes[start]));
}

template<>
inline int32_t bytesTo(const vector<unsigned char> &bytes, int32_t start) {
    return *(reinterpret_cast<const int32_t *>(&bytes[start]));
}

template<typename T>
string suffixForType() {
    if (is_same<T, int8_t>::value) { return "I8"; }
    if (is_same<T, uint8_t>::value) { return "I8"; }
    if (is_same<T, int16_t>::value) { return "I16"; }
    if (is_same<T, uint16_t>::value) { return "I16"; }
    if (is_same<T, int32_t>::value) { return "I32"; }
    if (is_same<T, uint32_t>::value) { return "I32"; }
    if (is_same<T, int64_t>::value) { return "I64"; }
    if (is_same<T, uint64_t>::value) { return "I64"; }
    if (is_same<T, float>::value) { return "F32"; }
    if (is_same<T, double>::value) { return "F64"; }

    assert(false);
}

struct SourceRegion {
    Region region = {};
};

struct HighlightedRegion {
    Region region = {};
};

struct SourceInfoRegion {
    Region region = {};
};

struct Note {
    Region region = {};
    string message = "";
};

struct Error {
    Region region = {};
    string message = "";
    vector<Note> notes = {};
};

enum class Color {
    FG_RED,        // "0;31"
    FG_GREEN,      // "0;32"
    FG_BLUE,       // "0;34"
    FG_LIGHT_GREY, // "0;37"
    FG_DARK_GREY,  // "0;90"
    FG_DEFAULT,    // "0;39"
    BG_RED,        // "0;41"
    BG_GREEN,      // "0;42"
    BG_MAGENTA,    // "0;45"
    BG_BLUE,       // "0;44"
    BG_DEFAULT,    // "0;49"
};

template <typename T>
struct Colored {
    T value;
    vector<Color> colors;
    bool bold = false;
};

ostream &operator<<(ostream &os, LexerTokenType tokenType);
ostream &operator<<(ostream &os, Node *node);
ostream &operator<<(ostream &os, Location location);
ostream &operator<<(ostream &os, Region location);
ostream &operator<<(ostream &os, SourceInfoRegion srcInfo);
ostream &operator<<(ostream &os, SourceRegion region);
ostream &operator<<(ostream &os, HighlightedRegion region);
ostream &operator<<(ostream &os, Color color);
ostream &operator<<(ostream &os, NodeTypekind kind);
ostream &operator<<(ostream &os, vector<unsigned char> v);
ostream &operator<<(ostream &os, TypeData td);

template<typename T>
ostream &operator<<(ostream &os, Colored<T> colored) {
    os << "\x1B[" << (colored.bold ? "1" : "0");
    for (auto color : colored.colors) {
        os << ";" << color;
    }
    return os << "m" << colored.value << "\x1B[0m";
}

struct SourceMapStatement {
    unsigned long instIndex;
    unsigned long instEndIndex;

    unsigned long startLine;
    unsigned long startByte;
    unsigned long endByte;
};

struct SourceMap {
    SourceInfo sourceInfo = {};
    vector<SourceMapStatement> statements = {};
};

/////////////
//  ATOMS  //
/////////////
class AtomTable {
public:
    static AtomTable *current;
    hash_t<string, int64_t> *atoms;
    vector<string> backwardAtoms;

    int64_t insert(Region &r);
    int64_t insertStr(string s);

    AtomTable();
};

Node *resolve(Node *n);

bool hasNoLocalByDefault(Node *node);

#endif
