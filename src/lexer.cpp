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

Lexer::Lexer(Lexer *lexer, Node *node) {
    this->srcInfo = lexer->srcInfo;
    this->lastLoc = node->region.start;
    this->loc = node->region.start;
    this->popFront();
    this->popFront();
}

Lexer::Lexer(string fileName, bool isFile) {
    auto lines = vector_init<unsigned long>(1);
    vector_append(lines, (unsigned long) 0);

    if (isFile) {
        ifstream t(fileName);
        string fileBytes;

        t.seekg(0, ios::end);
        fileBytes.reserve((unsigned long) t.tellg());
        t.seekg(0, ios::beg);

        fileBytes.assign((istreambuf_iterator<char>(t)),
                         istreambuf_iterator<char>());

        srcInfo = {new string(fileName), new string(fileBytes), lines};
    }
    else {
        srcInfo = {new string("unknown"), new string(fileName), lines};
    }

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

bool Lexer::tryEatKeyword(LexerToken *token, string pre, LexerTokenType type) {
    if (prefixKeyword(pre)) {
        token->type = type;
        popFrontFinalize(pre.length(), *token);
        return 1;
    }

    return 0;
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
    if (tryEat(&next, "-", LexerTokenType::SUB)) { return; }
    if (tryEat(&next, "+", LexerTokenType::ADD)) { return; }
    if (tryEat(&next, "*", LexerTokenType::MUL)) { return; }
    if (tryEat(&next, "^", LexerTokenType::DEREF)) { return; }
    if (tryEat(&next, "/", LexerTokenType::DIV)) { return; }
    if (tryEat(&next, "==", LexerTokenType::EQ_EQ)) { return; }
    if (tryEat(&next, "!=", LexerTokenType::NE)) { return; }
    if (tryEat(&next, "<=", LexerTokenType::LE)) { return; }
    if (tryEat(&next, ">=", LexerTokenType::GE)) { return; }
    if (tryEat(&next, "<=", LexerTokenType::LE)) { return; }
    if (tryEat(&next, "<", LexerTokenType::LT)) { return; }
    if (tryEat(&next, ">", LexerTokenType::GT)) { return; }
    if (tryEat(&next, "&", LexerTokenType::AMP)) { return; }
    if (tryEat(&next, ".", LexerTokenType::DOT)) { return; }
    if (tryEat(&next, "::=", LexerTokenType::COLON_COLON_EQ)) { return; }
    if (tryEat(&next, "::", LexerTokenType::COLON_COLON)) { return; }
    if (tryEat(&next, ":=", LexerTokenType::COLON_EQ)) { return; }
    if (tryEat(&next, ":", LexerTokenType::COLON)) { return; }
    if (tryEat(&next, "=", LexerTokenType::EQ)) { return; }
    if (tryEat(&next, ",", LexerTokenType::COMMA)) { return; }
    if (tryEat(&next, "!", LexerTokenType::NOT)) { return; }
    if (tryEat(&next, "@", LexerTokenType::AT)) { return; }
    if (tryEat(&next, "|", LexerTokenType::VERTICAL_BAR)) { return; }
    if (tryEatKeyword(&next, "defer", LexerTokenType::DEFER)) { return; }
    if (tryEatKeyword(&next, "#run", LexerTokenType::RUN)) { return; }
    if (tryEatKeyword(&next, "and", LexerTokenType::AND)) { return; }
    if (tryEatKeyword(&next, "or", LexerTokenType::OR)) { return; }
    if (tryEatKeyword(&next, "fn", LexerTokenType::FN)) { return; }
    if (tryEatKeyword(&next, "type", LexerTokenType::TYPE)) { return; }
    if (tryEatKeyword(&next, "struct", LexerTokenType::STRUCT)) { return; }
    if (tryEatKeyword(&next, "enum", LexerTokenType::ENUM)) { return; }
    if (tryEatKeyword(&next, "return", LexerTokenType::RETURN)) { return; }
    if (tryEatKeyword(&next, "string", LexerTokenType::STRING)) { return; }
    if (tryEatKeyword(&next, "bool", LexerTokenType::BOOLEAN)) { return; }
    if (tryEatKeyword(&next, "i8", LexerTokenType::I8)) { return;}
    if (tryEatKeyword(&next, "i32", LexerTokenType::I32)) { return;}
    if (tryEatKeyword(&next, "i64", LexerTokenType::I64)) { return;}
    if (tryEatKeyword(&next, "f32", LexerTokenType::F32)) { return;}
    if (tryEatKeyword(&next, "f64", LexerTokenType::F64)) { return;}
    if (tryEatKeyword(&next, "#if", LexerTokenType::STATIC_IF)) { return; }
    if (tryEatKeyword(&next, "if", LexerTokenType::IF)) { return; }
    if (tryEatKeyword(&next, "while", LexerTokenType::WHILE)) { return; }
    if (tryEatKeyword(&next, "else", LexerTokenType::ELSE)) { return; }
    if (tryEatKeyword(&next, "true", LexerTokenType::TRUE_)) { return; }
    if (tryEatKeyword(&next, "false", LexerTokenType::FALSE_)) { return; }
    if (tryEatKeyword(&next, "nil", LexerTokenType::NIL)) { return; }
    if (tryEatKeyword(&next, "module", LexerTokenType::MODULE)) { return; }
    if (tryEatKeyword(&next, "import", LexerTokenType::IMPORT)) { return; }
    if (tryEatKeyword(&next, "cast", LexerTokenType::CAST)) { return; }
    if (tryEatKeyword(&next, "Ast", LexerTokenType::EXPOSED_AST)) { return; }
    if (tryEatKeyword(&next, "rettypeof", LexerTokenType::RETTYPEOF)) { return; }
    if (tryEatKeyword(&next, "typeof", LexerTokenType::TYPEOF)) { return; }
    if (tryEatKeyword(&next, "#iskind", LexerTokenType::ISKIND)) { return; }
    if (tryEatKeyword(&next, "sizeof", LexerTokenType::SIZEOF)) { return; }
    if (tryEatKeyword(&next, "#fieldsof", LexerTokenType::FIELDSOF)) { return; }
    if (tryEatKeyword(&next, "heap", LexerTokenType::HEAP)) { return; }
    if (tryEatKeyword(&next, "puts", LexerTokenType::PUTS)) { return; }
    if (tryEatKeyword(&next, "panic", LexerTokenType::PANIC)) { return; }
    if (tryEatKeyword(&next, "none", LexerTokenType::NONE)) { return; }
    if (tryEatKeyword(&next, "malloc", LexerTokenType::MALLOC)) { return; }
    if (tryEatKeyword(&next, "free", LexerTokenType::FREE)) { return; }
    if (tryEatKeyword(&next, "tagcheck", LexerTokenType::TAGCHECK)) { return; }
    if (tryEatKeyword(&next, "for", LexerTokenType::FOR)) { return; }
    if (tryEatKeyword(&next, "#for", LexerTokenType::STATIC_FOR)) { return; }

    // BACK_TICK
    if (srcInfo.source->at(loc.byteIndex) == '`') {
        next.type = LexerTokenType::BACK_TICK;
        auto saved_loc = loc;

        eat(); // eat first back tick
        while (loc.byteIndex < srcInfo.source->length() && srcInfo.source->at(loc.byteIndex) != '`') {
            eat();
        }

        if (loc.byteIndex >= srcInfo.source->length()) {
            Note note = {{srcInfo, saved_loc, saved_loc}, "Leading ` here"};

            reportError({{srcInfo, loc, loc},
                         "reached EOF without closing `",
                         {note}});
        }

        eat(); // eat trailing back tick

        popFrontFinalize(0, next);
        return;
    }

    // SINGLE_QUOTE
    if (srcInfo.source->at(loc.byteIndex) == '\'') {
        next.type = LexerTokenType::BACK_TICK;
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
    if (isdigit(srcInfo.source->at(loc.byteIndex))) {
        next.type = LexerTokenType::INT_LITERAL;
        while (isdigit(srcInfo.source->at(loc.byteIndex)) || srcInfo.source->at(loc.byteIndex) == '_') {
            eat();

            if (loc.byteIndex >= srcInfo.source->size()) {
                popFrontFinalize(0, next);
                return;
            }
        }

        if (srcInfo.source->at(loc.byteIndex) == '.') {
            next.type = LexerTokenType::FLOAT_LITERAL;
            eat();
            while (isdigit(srcInfo.source->at(loc.byteIndex)) || srcInfo.source->at(loc.byteIndex) == '_') {
                eat();
            }
        }

        popFrontFinalize(0, next);
        return;
    }

    // todo(chad): more number formats for convenience
    // hex (0xCAFE)
    // binary (0b100010)

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
    "STRING",
    "BOOLEAN",
    "I8",
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
    "OR",
    "NOT",
    "NIL",
    "MODULE",
    "IMPORT",
    "CAST",
    "SEMICOLON",
    "RUN",
    "EXPOSED_AST",
    "TYPEOF",
    "RETTYPEOF",
    "SIZEOF",
    "FIELDSOF",
    "PANIC",
    "NONE",
    "MALLOC",
    "FREE",
    "PUTS",
    "TAGCHECK",
};

ostream &operator<<(ostream &os, LexerTokenType tokenType) {
    return os << Lexer::lexerTokenTypeStrings[static_cast<int32_t>(tokenType)];
}

bool isBinop(LexerTokenType type) {
    return isBooleanBinop(type)
           || type == LexerTokenType::ADD
           || type == LexerTokenType::SUB
           || type == LexerTokenType::MUL
           || type == LexerTokenType::DIV
           || type == LexerTokenType::VERTICAL_BAR;
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
