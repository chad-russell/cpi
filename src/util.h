#ifndef UTIL_H
#define UTIL_H

#include <string>
#include <cassert>
#include <vector>
#include <iostream>

using namespace std;

extern int64_t nodeId;

bool startsWith(string s, unsigned long startPos, string pre);
bool startsWith(string s, string pre);

bool endsWith(string s, string suf);

int bytesInCodepoint(char firstByte);

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
T bytesTo(vector<unsigned char> bytes, int32_t start) {
    T object;
    unsigned char *begin_object = reinterpret_cast<unsigned char *>(&object);
    memcpy(begin_object, &bytes[start], sizeof(T));
    return object;
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
    string fileName;
    string source;
    vector<unsigned long> lines = {};
};

struct Location {
    unsigned long byteIndex;
    unsigned long line;
    unsigned long col;
};

struct Region {
    SourceInfo srcInfo;
    Location start;
    Location end;

    string hash();
};

struct SourceRegion {
    Region region;
};

struct HighlightedRegion {
    Region region;
};

struct SourceInfoRegion {
    Region region;
};

struct Note {
    Region region;
    string message;
};

struct Error {
    Region region;
    string message;

    vector<Note> notes;
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

template<typename T>
ostream &operator<<(ostream &os, Colored<T> colored) {
    os << "\e[" << (colored.bold ? "1" : "0");
    for (auto color : colored.colors) {
        os << ";" << color;
    }
    return os << "m" << colored.value << "\e[0m";
}

#endif