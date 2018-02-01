#ifndef LEXER_H
#define LEXER_H

#include <stdint.h>

#include "util.h"

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
    DEREF,
    SYMBOL,
    INT_LITERAL,
    FLOAT_LITERAL,
    RET,
    STRING,
    BOOLEAN,
    I32,
    I64,
    F32,
    F64,
    IF,
    WHILE,
    ELIF,
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
    ASSERT
};

struct LexerToken{
    LexerTokenType type;
    Region region;
};

class Lexer {
public:
    LexerToken front;
    LexerToken next;

    Location lastLoc;
    Location loc;

    SourceInfo srcInfo;

    const static vector<string> lexerTokenTypeStrings;

    Lexer(string fileName);

    void popFront();
    void popFrontFinalize(LexerToken newNext);
    void popFrontFinalize(int eatLength, LexerToken newNext);
    void eat(int eatLength);
    void eat();
    bool isEmpty();
    bool prefix(string pre);
    bool prefixKeyword(string pre);
    void reportError(Error error);
//    bool isBooleanBinop(LexerTokenType type);
//    bool isBinop(LexerTokenType type);
    bool tryEat(LexerToken*token, string pre, LexerTokenType type);
    bool tryEatKeyword(LexerToken*token, string pre, LexerTokenType type);
};

ostream &operator<<(ostream &os, LexerTokenType tokenType);

#endif // LEXER_H
