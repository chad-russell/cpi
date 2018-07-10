#ifndef LEXER_H
#define LEXER_H

#include <stdint.h>

#include "util.h"

struct LexerToken {
    LexerTokenType type = {};
    Region region = {};
};

class Lexer {
public:
    LexerToken front = {};
    LexerToken next = {};

    Location lastLoc = {};
    Location loc = {};

    SourceInfo srcInfo = {};

    const static vector<string> lexerTokenTypeStrings;

    Lexer(Lexer *lexer, Node *node);
    Lexer(string fileName, bool isFile);

    void popFront();
    void popFrontFinalize(LexerToken newNext);
    void popFrontFinalize(int eatLength, LexerToken newNext);
    void eat(int eatLength);
    void eat();
    bool isEmpty();
    bool prefix(string pre);
    bool prefixKeyword(string pre);
    void reportError(Error error);
    bool tryEat(LexerToken*token, string pre, LexerTokenType type);
    bool tryEatKeyword(LexerToken*token, string pre, LexerTokenType type);
};

ostream &operator<<(ostream &os, LexerTokenType tokenType);

bool isBinop(LexerTokenType type);
bool isBooleanBinop(LexerTokenType type);

#endif // LEXER_H
