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
extern int noIppFlag;

extern int reusedPolymorphs;
extern int newPolymorphs;
extern long long int totalFnDeclDuration;

const char *readFile(const char *fileName);

bool startsWith(string *s, unsigned long startPos, string pre);
bool startsWith(string *s, string pre);

bool endsWith(string *s, string suf);

int bytesInCodepoint(char firstByte);

void cpi_assert(bool check);

class Node;
class Scope;

enum class NodeType {
    FN_DECL = 0,
    SYMBOL = 1,
    DECL = 2,
    ASSIGN = 3,
    BINOP = 4,
    PIPE = 5,
    DOT = 6,
    FN_CALL = 7,
    INT_LITERAL = 8,
    FLOAT_LITERAL = 9,
    STRING_LITERAL = 10,
    NIL_LITERAL = 11,
    BOOLEAN_LITERAL = 12,
    BOOLEAN = 13,
    UNARY_NEG = 14,
    RETURN = 15,
    IF = 16,
    WHILE = 17,
    DECL_PARAM = 18,
    VALUE_PARAM = 19,
    TYPE = 20,
    DEREF = 21,
    ADDRESS_OF = 22,
    STRUCT_LITERAL = 23,
    UNARY_NOT = 24,
    MODULE = 25,
    IMPORT = 26,
    CAST = 27,
    PANIC = 28,
    RUN = 29,
    TYPEOF = 30,
    RETURNTYPEOF = 31,
    SIZEOF = 32,
    FIELDSOF = 33,
    ARRAY_INDEX = 34,
    PUTS = 37,
    TAGCHECK = 38,
    ARRAY_LITERAL = 39,
    FOR = 40,
    ISKIND = 41,
    DEFER = 42,
    END_SCOPE = 43,
    ALIAS = 44,
    LINK = 45,
    UNARY_BITNOT = 46,
    PARAMETERIZED_TYPE = 47,
    ENUM_LITERAL = 48,
    ATTR = 49,
    ATTROF = 50,
    HASATTR = 51,
    CONTEXT = 52,
    CREATE_CONTEXT = 53,
};

enum class NodeTypekind: int32_t {
    NONE = 0,
    INT_LITERAL = 1,
    U8 = 2,
    I8 = 3,
    U16 = 4,
    I16 = 5,
    U32 = 6,
    I32 = 7,
    U64 = 8,
    I64 = 9,
    FLOAT_LITERAL = 10,
    F32 = 11,
    F64 = 12,
    FN = 13,
    BOOLEAN_LITERAL = 14,
    BOOLEAN = 15,
    STRUCT = 16,
    SYMBOL = 17,
    POINTER = 18,
    EXPOSED_AST = 19,
    DOT = 20,
    AUTOCAST = 21,
    PARAMETERIZED = 22,
    ENUM = 23,
    CONTEXT_TYPE = 24,
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
    SUBEQ,
    ADDEQ,
    MULEQ,
    DIVEQ,
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
    VERTICAL_BAR,
    AT,
    FN,
    TYPE,
    STRUCT,
    UNION,
    ENUM,
    SYMBOL,
    INT_LITERAL,
    FLOAT_LITERAL,
    RETURN,
    BOOLEAN,
    U8,
    I8,
    U16,
    I16,
    U32,
    I32,
    U64,
    I64,
    F32,
    F64,
    IF,
    STATIC_IF,
    WHILE,
    ELSE,
    TRUE_,
    FALSE_,
    AND,
    BITAND,
    OR,
    BITOR,
    BITXOR,
    BITSHL,
    BITSHR,
    NOT,
    BITNOT,
    NIL,
    MODULE,
    IMPORT,
    CAST,
    SEMICOLON,
    RUN,
    EXPOSED_AST,
    TYPEOF,
    RETURNTYPEOF,
    SIZEOF,
    FIELDSOF,
    PANIC,
    NONE,
    PUTS,
    TAGCHECK,
    FOR,
    STATIC_FOR,
    ISKIND,
    DEFER,
    LINK,
    MOD,
    ATTR,
    ATTROF,
    HASATTR,
    CONTEXT,
    CONTEXT_INIT,
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

    bool isSecretlyUnion;
    Node *unionTagType;

    int32_t alignment;

    Node *coercedType;
};

struct EnumTypeData {
    Node *type;
    vector_t<Node *> params;
};

struct PointerTypeData {
    Node *underlyingType;
};

struct SymbolTypeData {
    int64_t atomId;
};

struct ParameterizedTypeTypeData {
    Node *value;
};

struct TypeData {
    NodeTypekind kind;
    Node *name;

    vector_t<Node *> *attributes;
    Node *polyCameFrom;
    vector_t<Node *> *polyParams;

    union {
        FnTypeData fnTypeData;
        StructTypeData structTypeData;
        EnumTypeData enumTypeData;
        PointerTypeData pointerTypeData;
        SymbolTypeData symbolTypeData;
        Node *dotTypeData;
        ParameterizedTypeTypeData polymorphTypeTypeData;
        double floatTypeData;
        int64_t intTypeData;
        bool boolTypeData;
        Node *autocastData;
    };
};

struct ParamData {
    Node *name;
    Node *type;
    Node *value;

    Node *polyLink;
    Node *polyCameFrom;
    bool isAutoPolyParam;

    bool isContext;

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
    Scope *bodyScope;

    int32_t stackSize;
    uint64_t instOffset;
    bool isLiteral;
    bool isExternal;
    uint32_t tableIndex;
    bool cameFromPolymorph;
    bool isImpl;
    int64_t debugLocalOffset;
    bool skipContext;
};

struct DeclData {
    Node *lhs;
    Node *type;
    Node *initialValue;
};

struct AssignData {
    Node *lhs;
    Node *rhs;
};

// todo(chad): inline these
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
    bool skipContext;
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
    string *value;
    Node *allocFn;
    Node *arrayLiteralRepresentation;
};

struct IfData {
    Node *condition;
    vector_t<Node *> stmts;
    vector_t<Node *> elseStmts;

    bool isStatic;
    Scope *ifScope;
    Scope *elseScope;

    vector_t<Node *> trueImports;
    vector_t<Node *> falseImports;

    vector_t<Node *> trueContexts;
    vector_t<Node *> falseContexts;

    vector_t<Node *> trueContextInits;
    vector_t<Node *> falseContextInits;

    vector_t<Node *> trueImpls;
    vector_t<Node *> falseImpls;
};

struct WhileData {
    Node *condition;
    vector_t<Node *> stmts;
};

struct CastData {
    Node *type;
    Node *value;
};

struct RetData {
    Node *value;
};

struct SymbolData {
    int64_t atomId; // todo(chad): why not uint64_t?
    bool isAttr;
};

struct ArrayLiteralData {
    Node *elementType;
    vector_t<Node *> elements;
    Node *structLiteralRepresentation;
    Node *allocFn;
};

struct ForData {
    Node *element_alias;
    Node *iterator_alias;
    Node *target;

    vector_t<Node *> stmts;
    vector_t<Node *> rewritten;

    bool isStatic;
    vector_t<Node *> staticStmts;
};

struct UnaryNegData {
    Node *target;
    Node *rewritten;
};

struct IsKindData {
    Node *type;
    LexerTokenType tokenType;
};

struct ModuleData {
    Node *name;
    vector_t<Node *> stmts;
    int64_t fullImportAtomId;
};

struct ScopeData {
    Node *targetType;
    vector_t<Node *> stmts;
    Scope *scope;
};

struct ImportData {
    bool isFile;
    Node *target;
    Node *alias;
};

struct AttrData {
    Node *target;
    vector_t<Node *> stmts;
};

struct AttrofData {
    Node *target;
    Node *attr;
};

struct ContextData {
    vector_t<Node *> decls;
};

struct ParameterizedTypeData {
    vector_t<Node *> ctParams;
    Node *typeDecl;

    vector_t<Node *> attributes;
};

struct EnumLiteralData {
    Node *type;
    Node *value;
};

struct DeferData {
    vector_t<Node *> stmts;
};

struct LinkData {
    string *name;
};

class Scope {
public:
    hash_t<int64_t, Node *> *symbols;
    Scope *parent = nullptr;

    // for deferred
    vector_t<Node *> deferredStmts = vector_init<Node *>(4);
    bool insertedDeferredStmts = false;

    vector_t<Node *> staticIfs = vector_init<Node *>(4);
    bool addedStaticIfs = false;

    bool isFunctionScope = false;
    vector_t<Node *> fnScopeParams = vector_init<Node *>(4);
    Node *fnReturnType = nullptr;

    // methods
    explicit Scope(Scope *parent);
    Node *find(int64_t atomId);
};

class Node {
public:
    unsigned long id;

    Region region = {};
    Scope *scope = nullptr;
    Node *typeInfo = nullptr;
    Node *resolved = nullptr;
    Node *bytecodeResolved = nullptr;
    NodeType type;

    vector_t<Node *> preStmts = vector_init<Node *>(10);
    vector_t<Node *> postStmts = vector_init<Node *>(10);

    union {
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
        ArrayLiteralData arrayLiteralData;
        ForData forData;
        IsKindData isKindData;
        ModuleData moduleData;
        DeferData deferData;
        UnaryNegData unaryNegData;
        LinkData linkData;
        ImportData importData;
        ParameterizedTypeData parameterizedTypeData;
        EnumLiteralData enumLiteralData;
        AttrData attrData;
        AttrofData attrofData;
        ContextData contextData;
    };

    Node *staticValue = nullptr;

    bool isUsedInError = false;
    bool semantic = false;
    bool printed = false;
    bool tagCheck = false;
    bool llvmGen = false;
    bool sourceMapStatement = false;

    uint32_t genId = 0;

    // todo(chad): better way to store this?
    vector<unsigned char> bytecode;
    bool isLocal = false;

    bool isBytecodeLocal = false;
    bool skipAllButPostStmts = false;
    bool debugBytecodeAdjusted = false;

    // the offset of the storage for this node from the current base pointer
    int64_t localOffset = 0;

    void *llvmLocal = nullptr;
    void *llvmData = nullptr;

    Node(Region r = {});
    explicit Node(NodeTypekind typekind);
    Node(SourceInfo srcInfo, NodeType type_, Scope *scope_);
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
inline vector<unsigned char> toBytes32(const T object) {
    return toBytes(static_cast<int32_t>(object));
}

template<typename T>
inline vector<unsigned char> toBytes64(const T object) {
    return toBytes(static_cast<int64_t>(object));
}

template<typename T>
inline T bytesTo(const vector<unsigned char> &bytes, int64_t start) {
    return *((const T *) &bytes[start]);
}

template<>
inline int64_t bytesTo(const vector<unsigned char> &bytes, int64_t start) {
    return *(reinterpret_cast<const int64_t *>(&bytes[start]));
}

template<>
inline int32_t bytesTo(const vector<unsigned char> &bytes, int64_t start) {
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

    cpi_assert(false);
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
    Node *node;
};

struct SourceMap {
    SourceInfo sourceInfo = {};
    vector<SourceMapStatement> statements = {};
};

bool isFloatType(Node *type);
bool isNumericType(Node *type);

/////////////
//  ATOMS  //
/////////////
class AtomTable {
public:
    hash_t<string, int64_t> *atoms;
    vector<string> backwardAtoms;

    int64_t insert(Region &r);
    int64_t insertStr(string s);
};

extern AtomTable *atomTable;

Node *resolve(Node *n);

bool hasNoLocalByDefault(Node *node);

#endif
