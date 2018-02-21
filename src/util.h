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

bool startsWith(string s, unsigned long startPos, string pre);
bool startsWith(string s, string pre);

bool endsWith(string s, string suf);

int bytesInCodepoint(char firstByte);

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
    VALUE_PARAM,
    TYPE,
    DEREF,
    ADDRESS_OF,
    STRUCT_LITERAL,
    UNARY_NOT,
    MODULE,
    IMPORT,
    CAST,
    ASSERT,
    RUN
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
    EXPOSED_TYPE,
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

    AtomTable();
};

#endif
