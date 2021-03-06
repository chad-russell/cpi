#include <string>
#include <fstream>
#include <streambuf>
#include <iostream>
#include <cassert>

#include "util.h"
#include "lexer.h"

bool isSpecial(char c) {
    return c == ' '
           || c == '\t'
           || c == '\n'
           || c == '\r'
           || c == '{'
           || c == '}'
           || c == '('
           || c == ')'
           || c == '['
           || c == ']'
           || c == '+'
           || c == '-'
           || c == '*'
           || c == '/'
           || c == '-'
           || c == '.'
           || c == ':'
           || c == '\''
           || c == '"'
           || c == '`'
           || c == '!'
           || c == '|'
           || c == ','
           || c == ';';
}

Lexer::Lexer(SourceInfo srcInfo, Node *node) {
    this->srcInfo = srcInfo;
    this->lastLoc = node->region.start;
    this->loc = node->region.start;
    this->popFront();
    this->popFront();
}

Lexer::Lexer(string *fileName, string *fileSrc) {
    auto lines = vector_init<unsigned long>(1);
    vector_append(lines, (unsigned long) 0);

    if (fileName != nullptr && fileSrc != nullptr) {
        srcInfo = {fileName, fileSrc, lines};
    }
    else if (fileName != nullptr) {
        ifstream t(*fileName);
        string fileBytes;

        t.seekg(0, ios::end);
        fileBytes.reserve((unsigned long) t.tellg());
        t.seekg(0, ios::beg);

        fileBytes.assign((istreambuf_iterator<char>(t)),
                         istreambuf_iterator<char>());

        srcInfo = {fileName, new string(fileBytes), lines};
    }
    else if (fileSrc != nullptr) {
        srcInfo = {nullptr, fileSrc};
    }
    else { cpi_assert(false); }

    lastLoc = {0, 1, 1};
    loc = {0, 1, 1};

    popFront();
    popFront();
}

bool Lexer::tryEat(LexerToken *token, string pre, LexerTokenType type) {
    if (prefix(pre)) {
        token->type = type;
        popFrontFinalize((int) pre.length(), *token);
        return 1;
    }
    return 0;
}

bool isNumericDigit(char c, bool isParsingHex, bool isParsingBin) {
    if (isParsingHex) {
        return isdigit(c) || c == '_' || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
    }
    else if (isParsingBin) {
        return c == '0' || c == '1' || c == '_';
    }
    else {
        return isdigit(c) || c == '_';
    }
}

bool Lexer::tryEatKeyword(LexerToken *token, string pre, LexerTokenType type) {
    if (prefixKeyword(pre)) {
        token->type = type;
        popFrontFinalize((int) pre.length(), *token);
        return true;
    }

    return false;
}

void Lexer::popFront() {
    front = next;

    LexerToken next;

    // ignore whitespace
    while (loc.byteIndex < srcInfo.source->length() && isspace(srcInfo.source->at(loc.byteIndex))) {
        eat();
    }

    // Comment
    if (prefix("--")) {
        // eat until newline
        while (srcInfo.source->at(loc.byteIndex) != '\n') {
            eat();
        }

        next.type = LexerTokenType::COMMENT;
        popFrontFinalize(next);
        return;
    }

    // ignore whitespace
    while (loc.byteIndex < srcInfo.source->length() && isspace(srcInfo.source->at(loc.byteIndex))) {
        eat();
    }

    // look for EOF
    if (loc.byteIndex >= srcInfo.source->length()) {
        next.type = LexerTokenType::EOF_;
        popFrontFinalize(next);
        return;
    }

    // save location. next LexerToken effectively starts here
    lastLoc = loc;

    if (tryEat(&next, ";", LexerTokenType::SEMICOLON)) { return; }
    if (tryEat(&next, "{", LexerTokenType::LCURLY)) { return; }
    if (tryEat(&next, "}", LexerTokenType::RCURLY)) { return; }
    if (tryEat(&next, "(", LexerTokenType::LPAREN)) { return; }
    if (tryEat(&next, ")", LexerTokenType::RPAREN)) { return; }
    if (tryEat(&next, "[", LexerTokenType::LSQUARE)) { return; }
    if (tryEat(&next, "]", LexerTokenType::RSQUARE)) { return; }
    if (tryEat(&next, "-=", LexerTokenType::SUBEQ)) { return; }
    if (tryEat(&next, "+=", LexerTokenType::ADDEQ)) { return; }
    if (tryEat(&next, "*=", LexerTokenType::MULEQ)) { return; }
    if (tryEat(&next, "/=", LexerTokenType::DIVEQ)) { return; }
    if (tryEat(&next, "-", LexerTokenType::SUB)) { return; }
    if (tryEat(&next, "+", LexerTokenType::ADD)) { return; }
    if (tryEat(&next, "*", LexerTokenType::MUL)) { return; }
    if (tryEat(&next, "/", LexerTokenType::DIV)) { return; }
    if (tryEat(&next, "^", LexerTokenType::DEREF)) { return; }
    if (tryEat(&next, "==", LexerTokenType::EQ_EQ)) { return; }
    if (tryEat(&next, "!=", LexerTokenType::NE)) { return; }
    if (tryEat(&next, "<=", LexerTokenType::LE)) { return; }
    if (tryEat(&next, ">=", LexerTokenType::GE)) { return; }
    if (tryEat(&next, "<=", LexerTokenType::LE)) { return; }
    if (tryEat(&next, "<", LexerTokenType::LT)) { return; }
    if (tryEat(&next, ">", LexerTokenType::GT)) { return; }
    if (tryEat(&next, "&", LexerTokenType::AMP)) { return; }
    if (tryEat(&next, ".", LexerTokenType::DOT)) { return; }
    if (tryEat(&next, ":=", LexerTokenType::COLON_EQ)) { return; }
    if (tryEat(&next, ":", LexerTokenType::COLON)) { return; }
    if (tryEat(&next, "=", LexerTokenType::EQ)) { return; }
    if (tryEat(&next, ",", LexerTokenType::COMMA)) { return; }
    if (tryEat(&next, "~!", LexerTokenType::BITNOT)) { return; }
    if (tryEat(&next, "!", LexerTokenType::NOT)) { return; }
    if (tryEat(&next, "@", LexerTokenType::AT)) { return; }
    if (tryEat(&next, "|", LexerTokenType::VERTICAL_BAR)) { return; }
    if (tryEatKeyword(&next, "#link", LexerTokenType::LINK)) { return; }
    if (tryEatKeyword(&next, "defer", LexerTokenType::DEFER)) { return; }
    if (tryEatKeyword(&next, "#run", LexerTokenType::RUN)) { return; }
    if (tryEatKeyword(&next, "~and", LexerTokenType::BITAND)) { return; }
    if (tryEatKeyword(&next, "and", LexerTokenType::AND)) { return; }
    if (tryEatKeyword(&next, "~or", LexerTokenType::BITOR)) { return; }
    if (tryEatKeyword(&next, "or", LexerTokenType::OR)) { return; }
    if (tryEatKeyword(&next, "~xor", LexerTokenType::BITXOR)) { return; }
    if (tryEatKeyword(&next, "~shl", LexerTokenType::BITSHL)) { return; }
    if (tryEatKeyword(&next, "~shr", LexerTokenType::BITSHR)) { return; }
    if (tryEatKeyword(&next, "mod", LexerTokenType::MOD)) { return; }
    if (tryEatKeyword(&next, "fn", LexerTokenType::FN)) { return; }
    if (tryEatKeyword(&next, "type", LexerTokenType::TYPE)) { return; }
    if (tryEatKeyword(&next, "struct", LexerTokenType::STRUCT)) { return; }
    if (tryEatKeyword(&next, "union", LexerTokenType::UNION)) { return; }
    if (tryEatKeyword(&next, "enum", LexerTokenType::ENUM)) { return; }
    if (tryEatKeyword(&next, "return", LexerTokenType::RETURN)) { return; }
    if (tryEatKeyword(&next, "bool", LexerTokenType::BOOLEAN)) { return; }
    if (tryEatKeyword(&next, "u8", LexerTokenType::U8)) { return;}
    if (tryEatKeyword(&next, "i8", LexerTokenType::I8)) { return;}
    if (tryEatKeyword(&next, "u16", LexerTokenType::U16)) { return;}
    if (tryEatKeyword(&next, "i16", LexerTokenType::I16)) { return;}
    if (tryEatKeyword(&next, "u32", LexerTokenType::U32)) { return;}
    if (tryEatKeyword(&next, "i32", LexerTokenType::I32)) { return;}
    if (tryEatKeyword(&next, "u64", LexerTokenType::U64)) { return;}
    if (tryEatKeyword(&next, "i64", LexerTokenType::I64)) { return;}
    if (tryEatKeyword(&next, "f32", LexerTokenType::F32)) { return;}
    if (tryEatKeyword(&next, "f64", LexerTokenType::F64)) { return;}
    if (tryEatKeyword(&next, "#iskind", LexerTokenType::ISKIND)) { return; }
    if (tryEatKeyword(&next, "#if", LexerTokenType::STATIC_IF)) { return; }
    if (tryEatKeyword(&next, "if", LexerTokenType::IF)) { return; }
    if (tryEatKeyword(&next, "while", LexerTokenType::WHILE)) { return; }
    if (tryEatKeyword(&next, "else", LexerTokenType::ELSE)) { return; }
    if (tryEatKeyword(&next, "true", LexerTokenType::TRUE_)) { return; }
    if (tryEatKeyword(&next, "false", LexerTokenType::FALSE_)) { return; }
    if (tryEatKeyword(&next, "nil", LexerTokenType::NIL)) { return; }
    if (tryEatKeyword(&next, "module", LexerTokenType::MODULE)) { return; }
    if (tryEatKeyword(&next, "#hasattr", LexerTokenType::HASATTR)) { return; }
    if (tryEatKeyword(&next, "#attrof", LexerTokenType::ATTROF)) { return; }
    if (tryEatKeyword(&next, "#attr", LexerTokenType::ATTR)) { return; }
    if (tryEatKeyword(&next, "#type", LexerTokenType::TYPE_STMT)) { return; }
    if (tryEatKeyword(&next, "#contextinit", LexerTokenType::CONTEXT_INIT)) { return; }
    if (tryEatKeyword(&next, "#context", LexerTokenType::CONTEXT)) { return; }
    if (tryEatKeyword(&next, "#import", LexerTokenType::IMPORT)) { return; }
    if (tryEatKeyword(&next, "cast", LexerTokenType::CAST)) { return; }
    if (tryEatKeyword(&next, "Ast", LexerTokenType::EXPOSED_AST)) { return; }
    if (tryEatKeyword(&next, "returntypeof", LexerTokenType::RETURNTYPEOF)) { return; }
    if (tryEatKeyword(&next, "typeof", LexerTokenType::TYPEOF)) { return; }
    if (tryEatKeyword(&next, "sizeof", LexerTokenType::SIZEOF)) { return; }
    if (tryEatKeyword(&next, "#fieldsof", LexerTokenType::FIELDSOF)) { return; }
    if (tryEatKeyword(&next, "puts", LexerTokenType::PUTS)) { return; }
    if (tryEatKeyword(&next, "panic", LexerTokenType::PANIC)) { return; }
    if (tryEatKeyword(&next, "none", LexerTokenType::NONE)) { return; }
    if (tryEatKeyword(&next, "tagcheck", LexerTokenType::TAGCHECK)) { return; }
    if (tryEatKeyword(&next, "for", LexerTokenType::FOR)) { return; }
    if (tryEatKeyword(&next, "#for", LexerTokenType::STATIC_FOR)) { return; }

    // BACK_TICK
    if (srcInfo.source->at(loc.byteIndex) == '`') {
        next.type = LexerTokenType::SYMBOL;
        auto saved_loc = loc;

        eat(); // eat first back tick
        this->lastLoc = loc;

        while (loc.byteIndex < srcInfo.source->length() && srcInfo.source->at(loc.byteIndex) != '`') {
            eat();
        }

        if (loc.byteIndex >= srcInfo.source->length()) {
            Note note = {{srcInfo, saved_loc, saved_loc}, "Leading ` here"};

            reportError({{srcInfo, loc, loc},
                         "reached EOF without closing `",
                         {note}});
        }

        popFrontFinalize(0, next);

        eat(); // eat trailing back tick

        return;
    }

    // SINGLE_QUOTE
    if (srcInfo.source->at(loc.byteIndex) == '\'') {
        next.type = LexerTokenType::SINGLE_QUOTE;
        auto savedLoc = loc;

        eat(); // eat first single quote
        while (loc.byteIndex < srcInfo.source->length() && srcInfo.source->at(loc.byteIndex) != '\'') {
            eat();
        }

        if (loc.byteIndex >= srcInfo.source->length()) {
            Note note = {{srcInfo, savedLoc, savedLoc}, "Leading ' here"};
            Error error = {{srcInfo, lastLoc, loc},
                           "reached EOF without closing '",
                           {note}};
            reportError(error);
        }

        eat(); // eat trailing single quote

        popFrontFinalize(0, next);
        return;
    }

    // DOUBLE_QUOTE
    if (srcInfo.source->at(loc.byteIndex) == '"') {
        next.type = LexerTokenType::DOUBLE_QUOTE;
        auto savedLoc = loc;

        eat(); // eat first double quote
        while (loc.byteIndex < srcInfo.source->length()) {
            if (srcInfo.source->at(loc.byteIndex) == '\\') {
                // eat the next 2 tokens
                eat();

                if (loc.byteIndex >= srcInfo.source->length()) {
                    Note note = {{srcInfo, savedLoc, savedLoc}, "String started here"};
                    Error error = {{srcInfo, loc, loc},
                                   "reached EOF after an escape character while parsing a string. WTF are you even doing??",
                                   {note}};
                    reportError(error);
                }
                eat();

                continue;
            }

            if (srcInfo.source->at(loc.byteIndex) == '"') {
                break;
            }

            eat();
        }

        if (loc.byteIndex >= srcInfo.source->length()) {
            Note note = {{srcInfo, loc, loc}, "Leading \" here"};
            Error error = {{srcInfo, savedLoc, loc},
                           "reached EOF without closing '\"'",
                           {note}};
            reportError(error);
        }

        eat(); // eat trailing single quote

        popFrontFinalize(0, next);
        return;
    }

    // int
    auto parsingInt = false;
    auto parsingHex = false;
    auto parsingBin = false;
    if (srcInfo.source->at(loc.byteIndex) == '0' && srcInfo.source->length() > loc.byteIndex + 1) {
        if (srcInfo.source->at(loc.byteIndex + 1) == 'x') {
            parsingHex = true;
            eat(2); // 0x
        }
        else if (srcInfo.source->at(loc.byteIndex + 1) == 'b') {
            parsingBin = true;
            eat(2); // 0b
        }
        else {
            parsingInt = true;
        }
    }
    else if (isdigit(srcInfo.source->at(loc.byteIndex))) {
        parsingInt = true;
    }
    if (parsingInt || parsingHex || parsingBin) {
        next.type = LexerTokenType::INT_LITERAL;
        while (isNumericDigit(srcInfo.source->at(loc.byteIndex), parsingHex, parsingBin)) {
            eat();

            if (loc.byteIndex >= srcInfo.source->size()) {
                popFrontFinalize(0, next);
                return;
            }
        }

        if (srcInfo.source->at(loc.byteIndex) == '.') {
            if (parsingBin || parsingHex) {
                reportError(Error{{this->srcInfo, lastLoc, loc}, "decimals not allowed when parsing binary/hex literal"});
                return;
            }

            next.type = LexerTokenType::FLOAT_LITERAL;
            eat();
            while (isdigit(srcInfo.source->at(loc.byteIndex)) || srcInfo.source->at(loc.byteIndex) == '_') {
                eat();
            }
        }

        popFrontFinalize(0, next);
        return;
    }

    // SYMBOL
    next.type = LexerTokenType::SYMBOL;
    while (loc.byteIndex < srcInfo.source->length() && !isSpecial(srcInfo.source->at(loc.byteIndex))) {
        eat();
    }
    popFrontFinalize(0, next);
}

void Lexer::eat(int eatLength) {
    for (auto i = 0; i < eatLength; i++) {
        eat();
    }
}

void Lexer::eat() {
    auto frontChar = srcInfo.source->at(loc.byteIndex);

    if (frontChar == '\n') {
        totalLines += 1;

        loc.line += 1;
        loc.col = 1;

        unsigned long byteIndex = loc.byteIndex + 1;
        vector_append(srcInfo.lines, byteIndex);
    } else {
        loc.col += 1;
    }

    loc.byteIndex += bytesInCodepoint(frontChar);
}

void Lexer::popFrontFinalize(int eatLength, LexerToken newNext) {
    eat(eatLength);
    popFrontFinalize(newNext);
}

void Lexer::popFrontFinalize(LexerToken newNext) {
    newNext.region.srcInfo = srcInfo;

    newNext.region.start.byteIndex = lastLoc.byteIndex;
    newNext.region.start.line = lastLoc.line;
    newNext.region.start.col = lastLoc.col;

    newNext.region.end.byteIndex = loc.byteIndex;
    newNext.region.end.line = loc.line;
    newNext.region.end.col = loc.col;

    next = newNext;
}

bool Lexer::prefix(string pre) {
    if (srcInfo.source->length() - loc.byteIndex < pre.length()) {
        return 0;
    }

    for (unsigned long i = 0; i < pre.length(); i++) {
        if (srcInfo.source->at(loc.byteIndex + i) != pre[i]) {
            return 0;
        }
    }

    return 1;
}

bool Lexer::prefixKeyword(string pre) {
    if (srcInfo.source->length() - loc.byteIndex < pre.length() + 1) {
        return 0;
    }

    // if we have a match but there are more non-special characters, then it's not a match
    return prefix(pre) && isSpecial(srcInfo.source->at(loc.byteIndex + pre.length()));
}

void Lexer::reportError(Error error) {
    cout << SourceInfoRegion{error.region}
         << Colored<string>{"lexer error: ", {Color::FG_RED}, true}
         << Colored<string>{error.message, {Color::FG_DEFAULT}, true}
         << endl
         << HighlightedRegion{error.region}
         << endl;

    for (auto note : error.notes) {
        cout << SourceInfoRegion{note.region}
             << Colored<string>{"note: ", {Color::FG_BLUE}, 1}
             << note.message << endl;
    }

    exit(1);
}

bool Lexer::isEmpty() {
    return front.type == LexerTokenType::EOF_;
}

const vector<string> Lexer::lexerTokenTypeStrings = {
    "EOF_",
    "COMMENT",
    "LCURLY",
    "RCURLY",
    "LPAREN",
    "RPAREN",
    "LSQUARE",
    "RSQUARE",
    "SUB",
    "ADD",
    "MUL",
    "DIV",
    "DEREF",
    "EQ_EQ",
    "NE",
    "LE",
    "GE",
    "LT",
    "GT",
    "AMP",
    "DOT",
    "COLON_EQ",
    "COLON",
    "EQ",
    "COMMA",
    "SINGLE_QUOTE",
    "DOUBLE_QUOTE",
    "BACK_TICK",
    "VERTICAL_BAR",
    "AT",
    "FN",
    "TYPE",
    "STRUCT",
    "SYMBOL",
    "INT_LITERAL",
    "FLOAT_LITERAL",
    "RET",
    "BOOLEAN",
    "I8",
    "I16",
    "I32",
    "I64",
    "F32",
    "F64",
    "IF",
    "WHILE",
    "ELSE",
    "TRUE_",
    "FALSE_",
    "AND",
    "BITAND",
    "OR",
    "BITOR",
    "NOT",
    "BITNOT",
    "NIL",
    "MODULE",
    "SCOPE",
    "IMPORT",
    "CAST",
    "SEMICOLON",
    "RUN",
    "EXPOSED_AST",
    "TYPEOF",
    "RETURNTYPEOF",
    "SIZEOF",
    "FIELDSOF",
    "PANIC",
    "NONE",
    "PUTS",
    "TAGCHECK",
};

ostream &operator<<(ostream &os, LexerTokenType tokenType) {
    return os << Lexer::lexerTokenTypeStrings[static_cast<int32_t>(tokenType)];
}

bool isBinop(LexerTokenType type) {
    return isBooleanBinop(type)
           || type == LexerTokenType::ADDEQ
           || type == LexerTokenType::SUBEQ
           || type == LexerTokenType::MULEQ
           || type == LexerTokenType::DIVEQ
           || type == LexerTokenType::ADD
           || type == LexerTokenType::SUB
           || type == LexerTokenType::MUL
           || type == LexerTokenType::DIV
           || type == LexerTokenType::VERTICAL_BAR
           || type == LexerTokenType::MOD
           || type == LexerTokenType::BITAND
           || type == LexerTokenType::BITOR
           || type == LexerTokenType::BITXOR
           || type == LexerTokenType::BITSHL
           || type == LexerTokenType::BITSHR;
}

bool isBooleanBinop(LexerTokenType type) {
    return type == LexerTokenType::EQ_EQ
           || type == LexerTokenType::NE
           || type == LexerTokenType::LT
           || type == LexerTokenType::GT
           || type == LexerTokenType::LE
           || type == LexerTokenType::GE
           || type == LexerTokenType::AND
           || type == LexerTokenType::OR;
}
