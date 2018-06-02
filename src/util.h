#ifndef UTIL_H
#define UTIL_H

#include <string>
#include <cassert>
#include <vector>
#include <iostream>
#include <unordered_map>
#include <memory.h>

using namespace std;

extern unsigned long nodeId;
extern unsigned long fnTableId;
extern int debugFlag;

const char *readFile(const char *fileName);

bool startsWith(string s, unsigned long startPos, string pre);
bool startsWith(string s, string pre);

bool endsWith(string s, string suf);

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
    DOT_QUESTION,
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
    SIZEOF,
    ARRAY_INDEX,
    MALLOC,
    FREE,
    TAGCHECK,
    ARRAY_LITERAL,
    FOR,
    HEAPIFY,
    ANYOF,
};

enum class NodeTypekind {
    NONE,
    INT_LITERAL,
    I8,
    I32,
    I64,
    FLOAT_LITERAL,
    BOOLEAN,
    F32,
    F64,
    FN,
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
    UNION,
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
    SIZEOF,
    PANIC,
    NONE,
    MALLOC,
    FREE,
    TAGCHECK,
    FOR,
    HEAP,
    ANYOF,
};

struct SourceInfo {
    string fileName = "";
    string source = "";
    vector<unsigned long> lines = {};
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
    vector<Node *> params = {};
    Node *returnType = nullptr;
};

struct StructTypeData {
    bool isLiteral = false;
    vector<Node *> params = {};

    bool isSecretlyArray = false;
    Node *secretArrayElementType = nullptr;

    bool isSecretlyUnion = false;

    // todo(chad): this should maybe just be rolled up into 'resolved'?
    Node *coercedType = nullptr;
};

struct PointerTypeData {
    Node *underlyingType = nullptr;
};

struct SymbolTypeData {
    int64_t atomId;
};

struct TypeData {
    NodeTypekind kind;

//     union {
    FnTypeData fnTypeData;
    StructTypeData structTypeData;
    PointerTypeData pointerTypeData;
    SymbolTypeData symbolTypeData;
    double floatTypeData;
    int64_t intTypeData;
    bool boolTypeData;
//     };
};

struct DeclParamData {
    Node *name = nullptr;
    Node *type = nullptr;
    Node *initialValue = nullptr;

    Node *polyLink = nullptr;
    int64_t index;
};

struct ValueParamData {
    Node *name = nullptr;
    Node *value = nullptr;

    int32_t index;
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
    vector<Node *> ctParams = {};
    vector<Node *> params = {};
    Node * returnType = nullptr;
    vector<Node *> body = {};
    vector<Node *> locals = {};
    vector<Node *> returns = {};

    Scope *paramScope = nullptr;
    Scope *bodyScope = nullptr;

    int32_t stackSize = 0;
    unsigned long instOffset;
    bool isLiteral;
    unsigned long tableIndex;
    bool cameFromPolymorph = false;
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
    vector<Node *> ctParams = {};
    vector<Node *> params = {};

    bool hasRuntimeParams = true;
};

struct ArrayIndexData {
    Node *target;
    Node *indexValue;
};

struct DotData {
    Node *lhs = nullptr;
    Node *rhs = nullptr;
    Node *resolved = nullptr;

    Node *autoDerefStorage = nullptr;
    bool pointerIsRelative = false;
};

struct BinopData {
    LexerTokenType type;

    Node *lhs = nullptr;
    Node *rhs = nullptr;

    int rhsScale = 1;

    Node *lhsTemporary;
    Node *rhsTemporary;
};

struct PipeData {
    Node *lhs = nullptr;
    Node *rhs = nullptr;
};

struct StructLiteralData {
    vector<Node *> params = {};
};

struct StringLiteralData {
    string value;
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
    vector<Node *> elements;

    Node *structLiteralRepresentation;
};

struct ForData {
    Node *element_alias = nullptr;
    Node *iterator_alias = nullptr;
    Node *target = nullptr;

    vector<Node *> stmts;

    vector<Node *> rewritten;
};

class Scope {
public:
    unordered_map<int64_t, Node *> symbols = {};
    Scope *parent = {};

    Scope(Scope *parent);

    Node *find(int64_t atomId);
};

class Node {
public:
    unsigned long id;

    Region region = {};
    Scope *scope = nullptr;
    Node *typeInfo = nullptr;
    Node *resolved = nullptr;
    NodeType type;

    vector<Node *> preStmts;

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
    ArrayIndexData arrayIndexData;
    DotData dotData;
    BinopData binopData;
    PipeData pipeData;
    DeclParamData declParamData;
    ValueParamData valueParamData;
    TypeData typeData;
    StructLiteralData structLiteralData;
    StringLiteralData stringLiteralData;
    IfData ifData;
    WhileData whileData;
    ImportData importData;
    CastData castData;
    RetData retData;
    SymbolData symbolData;
    DerefData derefData;
    ArrayLiteralData arrayLiteralData;
    ForData forData;
    // };

    Node *staticValue = nullptr;

    bool isUsedInError = false;
    bool semantic = false;
    bool tagCheck = false;
    bool gen = false;
    bool llvmGen = false;
    bool sourceMapStatement = false;

    // todo(chad): better way to store this?
    vector<unsigned char> bytecode;
    bool isLocal = false;

    bool isBytecodeLocal = false;

    // todo(chad): figure out a way to get rid of this
//    bool isAutoDerefStorage = false;

    // the offset of the storage for this node from the current pointer
    int32_t localOffset = 0;

    void *llvmLocal = nullptr;
    void *llvmData = nullptr;

    Node();
    Node(NodeTypekind typekind);
    Node(SourceInfo srcInfo, NodeType type_, Scope *scope_);
};

template<typename T> 
vector<unsigned char> toBytes(const T object) {
    vector<unsigned char> bytes;
    bytes.reserve(sizeof(T));

    const unsigned char *beg = reinterpret_cast<const unsigned char *>(addressof(object));
    const unsigned char *end = beg + sizeof(T);

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
    T object;
    unsigned char *begin_object = reinterpret_cast<unsigned char *>(&object);
    memcpy(begin_object, &bytes[start], sizeof(T));
    return object;
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
    os << "\e[" << (colored.bold ? "1" : "0");
    for (auto color : colored.colors) {
        os << ";" << color;
    }
    return os << "m" << colored.value << "\e[0m";
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
    unordered_map<string, int64_t> atoms = {};
    vector<string> backwardAtoms = {};

    int64_t insert(Region r);
    int64_t insertStr(string s);

    AtomTable();
};

Node *resolve(Node *n);

bool hasNoLocalByDefault(Node *node);

#endif
