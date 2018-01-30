#include "util.h"

#include <iostream>
#include <sstream>

int bytesInCodepoint(char firstByte) {
    if (firstByte >= '\x00' && firstByte <= '\x7F') {
        return 1;
    }
    if (firstByte >= '\xC2' && firstByte <= '\xDF') {
        return 2;
    }
    if (firstByte >= '\xE0' && firstByte <= '\xEF') {
        return 3;
    }
    if (firstByte >= '\xF0' && firstByte <= '\xF4') {
        return 4;
    }
    assert(false);
}

bool startsWith(string s, string pre) {
    return startsWith(s, 0, pre);
}

bool startsWith(string s, unsigned long startPos, string pre) {
    if (s.length() - startPos < pre.length()) { return false; }

    for (auto index = 0; index < pre.length(); index++) {
        if (s[startPos + index] != pre[index]) { return false; }
    }

    return true;
}

bool endsWith(string s, string suf) {
    if (s.length() < suf.length()) {
        return false;
    }

    return (0 == s.compare(s.length() - suf.length(), suf.length(), suf));
}

ostream &operator<<(ostream &os, Location location) {
    return os << "("
              << location.line << ", " << location.col
              << ")";
}

ostream &operator<<(ostream &os, Region region) {
    return os << "[" << region.start << " - " << region.end << "]";
}

ostream &operator<<(ostream &os, SourceRegion region) {
    return os << region.region.srcInfo.source.substr(region.region.start.byteIndex,
                                             region.region.end.byteIndex - region.region.start.byteIndex);
}

ostream &operator<<(ostream &os, SourceInfoRegion region) {
    ostringstream message("");
    message << region.region.srcInfo.fileName
            << ":" << region.region.start.line
            << ":" << region.region.start.col << ": ";
    return os << Colored<string>{message.str(), {Color::FG_DEFAULT}, 1};
}

ostream &operator<<(ostream &os, HighlightedRegion region) {
    if (region.region.srcInfo.fileName == "") {
        return os << "NO SOURCE INFO" << endl;
    }

    auto startLine = region.region.start.line;
    if (startLine > 0) { startLine -= 1; }
    auto startSrc = region.region.srcInfo.lines[startLine];
    for (auto i = startSrc; i < region.region.srcInfo.source.length() && region.region.srcInfo.source[i] != '\n'; i++) {
        Colored<string> colored;
        if (i == region.region.start.byteIndex) {
            os << "\e[" << Color::BG_MAGENTA << "m";
        }
        if (i == region.region.end.byteIndex) {
            os << Colored<string>{"", {}};
        }

        os << region.region.srcInfo.source[i];
    }

    return os << Colored<string>{"", {}} << endl;
}

ostream &operator<<(ostream &os, Color color) {
    switch (color) {
    case Color::FG_RED: {
        return os << "31";
    } break;
    case Color::FG_GREEN: {
        return os << "32";
    } break;
     case Color::FG_BLUE: {
        return os << "34";
    } break;
    case Color::FG_LIGHT_GREY: {
        return os << "37";
    } break;
    case Color::FG_DARK_GREY: {
        return os << "90";
    } break;
    case Color::FG_DEFAULT: {
        return os << "39";
    } break;
    case Color::BG_RED: {
        return os << "41";
    } break;
    case Color::BG_GREEN: {
        return os << "42";
    } break;
    case Color::BG_MAGENTA: {
        return os << "45";
    } break;
    case Color::BG_BLUE: {
        return os << "44";
    } break;
    case Color::BG_DEFAULT: {
        return os << "49";
    } break;
    }
}

// todo(chad): make this shorter -- maybe need an id on srcInfo
// so we can just hash 3 ints without it being a string at all
string Region::hash() {
    ostringstream s("");
    s << srcInfo.fileName << start.byteIndex << end.byteIndex;
    return s.str();
}