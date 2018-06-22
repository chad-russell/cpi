#include "util.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <utility>

const char *readFile(const char *fileName) {
    ifstream fileStream;
    fileStream.open(fileName);

    stringstream fileStringStream;

    fileStringStream << fileStream.rdbuf();

    fileStream.close();
    return fileStringStream.str().c_str();
}

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

bool startsWith(string *s, string pre) {
    return startsWith(s, 0, std::move(pre));
}

bool startsWith(string *s, unsigned long startPos, string pre) {
    if (s->length() - startPos < pre.length()) { return false; }

    for (auto index = 0; index < pre.length(); index++) {
        if (s->at(startPos + index) != pre[index]) { return false; }
    }

    return true;
}

bool endsWith(string *s, string suf) {
    if (s->length() < suf.length()) {
        return false;
    }

    return (0 == s->compare(s->length() - suf.length(), suf.length(), suf));
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
    return os << region.region.srcInfo.source->substr(region.region.start.byteIndex,
                                             region.region.end.byteIndex - region.region.start.byteIndex);
}

ostream &operator<<(ostream &os, SourceInfoRegion region) {
    ostringstream message("");

    if (region.region.srcInfo.fileName == nullptr) {
        return os;
    }

    message << *region.region.srcInfo.fileName
            << ":" << region.region.start.line
            << ":" << region.region.start.col << ": ";
    return os << Colored<string>{message.str(), {Color::FG_DEFAULT}, 1};
}

ostream &operator<<(ostream &os, HighlightedRegion region) {
    if (region.region.srcInfo.fileName == nullptr) {
        return os << "NO SOURCE INFO" << endl;
    }

    auto startLine = region.region.start.line;
    if (startLine > 0) { startLine -= 1; }

    auto startSrc = vector_at(region.region.srcInfo.lines, (int64_t) startLine);
    for (auto i = startSrc; i < region.region.srcInfo.source->length() && region.region.srcInfo.source->at(i) != '\n'; i++) {
        Colored<string> colored;
        if (i == region.region.start.byteIndex) {
            os << "\e[" << Color::BG_MAGENTA << ";" << Color::FG_DARK_GREY << "m";
        }
        if (i == region.region.end.byteIndex) {
            os << Colored<string>{"", {}};
        }

        os << region.region.srcInfo.source->at(i);
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

ostream &operator<<(ostream &os, NodeTypekind kind) {
    switch (kind) {
        case NodeTypekind::NONE: {
            return os << "none";
        }
        case NodeTypekind::INT_LITERAL: {
            return os << "an integer literal";
        }
        case NodeTypekind::I8: {
            return os << "an i8";
        }
        case NodeTypekind::I32: {
            return os << "an i32";
        }
        case NodeTypekind::I64: {
            return os << "an i64";
        }
        case NodeTypekind::FLOAT_LITERAL: {
            return os << "an float literal";
        }
        case NodeTypekind::BOOLEAN: {
            return os << "an bool";
        }
        case NodeTypekind::F32: {
            return os << "an f32";
        }
        case NodeTypekind::F64: {
            return os << "an f64";
        }
        case NodeTypekind::FN: {
            return os << "a fn";
        }
        case NodeTypekind::STRUCT: {
            return os << "a struct";
        }
        case NodeTypekind::SYMBOL: {
            return os << "a symbol";
        }
        case NodeTypekind::POINTER: {
            return os << "a pointer";
        }
        case NodeTypekind::EXPOSED_AST: {
            return os << "ast";
        }
        default: assert(false);
    }
}

ostream &operator<<(ostream &os, vector<unsigned char> v) {
    for (auto c : v) {
        os << c;
    }
    return os;
}

ostream &operator<<(ostream &os, TypeData td) {
    switch (td.kind) {
        case NodeTypekind::NONE:
        case NodeTypekind::INT_LITERAL:
        case NodeTypekind::I8:
        case NodeTypekind::I32:
        case NodeTypekind::I64:
        case NodeTypekind::FLOAT_LITERAL:
        case NodeTypekind::BOOLEAN:
        case NodeTypekind::BOOLEAN_LITERAL:
        case NodeTypekind::F32:
        case NodeTypekind::F64:
        case NodeTypekind::EXPOSED_AST: {
            os << td.kind;
        } break;
        case NodeTypekind::FN: {
            os << "fn (";
            for (auto param : td.fnTypeData.params) {
                assert(param->type == NodeType::DECL_PARAM);
                os << param->paramData.type->typeData << ", ";
            }
            os << ")";
            os << td.fnTypeData.returnType->typeData;
        } break;

        case NodeTypekind::STRUCT: {
            os << "struct {";

            // todo(chad): @Incomplete -- handle array/union types here too
            for (auto param : td.structTypeData.params) {
                if (param->paramData.type) {
                    os << param->paramData.type->typeData << ", ";
                }
                else {
                    os << "<<unresolved>>" << ", ";
                }
            }

            os << "}";
        } break;
        case NodeTypekind::SYMBOL: {
            os << AtomTable::current->backwardAtoms[td.symbolTypeData.atomId];
        } break;
        case NodeTypekind::POINTER: {
            os << "*";
            os << td.pointerTypeData.underlyingType->typeData;
        } break;
    }

    return os;
}

ostream &operator<<(ostream &os, Node *node) {
    if (node->printed && node->type != NodeType::SYMBOL) { return os; }

    node->printed = true;

    for (auto ps: node->preStmts) {
        cout << ps;
    }

    if (node->skipAllButPostStmts) {
        for (auto stmt : node->postStmts) {
            cout << stmt;
        }

        return os;
    }

    switch (node->type) {
        case NodeType::FN_DECL: {
            cout << "fn (";
            for (auto p : node->fnDeclData.params) {
                cout << p << ", ";
            }
            cout << ") {" << endl;
            for (auto b : node->fnDeclData.body) {
                cout << b;
            }
            cout << "}" << endl;
        } break;
        case NodeType::SYMBOL:{
            auto sym = AtomTable::current->backwardAtoms[node->symbolData.atomId];
            cout << sym;
        } break;
        case NodeType::DECL: {
            if (node->declData.type == nullptr) {
                cout << node->declData.lvalue << " := " << node->declData.initialValue;
            }
            else if (node->declData.initialValue == nullptr) {
                cout << node->declData.lvalue << " : " << node->declData.type;
            }
            else {
                cout << node->declData.lvalue << " : " << node->declData.type << " = " << node->declData.initialValue;
            }

            cout << ";" << endl;
        } break;
        case NodeType::TYPE: {
            cout << node->typeData;
        } break;
        case NodeType::STRUCT_LITERAL: {
            cout << "{ ";
            for (auto p : node->structLiteralData.params) {
                cout << p << ", ";
            }
            cout << " }";
        } break;
        case NodeType::VALUE_PARAM: {
            if (node->paramData.name != nullptr) {
                cout << node->paramData.name << " : " << node->paramData.value;
            }
            else {
                cout << node->paramData.value;
            }
        } break;
        case NodeType::INT_LITERAL: {
            cout << node->intLiteralData.value;
        } break;
        case NodeType::ASSIGN: {
            cout << node->assignData.lhs << " = " << node->assignData.rhs << ";" << endl;
        } break;
        case NodeType::DOT: {
            cout << node->dotData.lhs << "." << node->dotData.rhs;
        } break;
        case NodeType::NIL_LITERAL: {
            cout << "nil";
        } break;
        case NodeType::ARRAY_LITERAL: {
            cout << "[]" << node->arrayLiteralData.elementType << "{";
            for (auto e : node->arrayLiteralData.elements) {
                cout << e << ", ";
            }
            cout << "}";

//             cout << node->arrayLiteralData.structLiteralRepresentation;
        } break;
        case NodeType::RET: {
            cout << "return " << node->retData.value << ";" << endl;
        } break;
        case NodeType::HEAPIFY: {
            cout << "heap(" << node->nodeData << ")";
        } break;
        case NodeType::CAST: {
            cout << "cast(" << node->castData.type << ") " << node->castData.value;
        } break;
        case NodeType::STRING_LITERAL: {
            cout << "\"" << node->stringLiteralData.value << "\"";
        } break;
        case NodeType::ADDRESS_OF: {
            cout << "&" << node->nodeData;
        } break;
        case NodeType::IF: {
            cout << "if " << node->ifData.condition << " {" << endl;
            for (auto s : node->ifData.stmts) {
                cout << s;
            }
            // todo(chad): else clause
            cout << "}" << endl;
        } break;
        case NodeType::BINOP: {
            cout << node->binopData.lhs << " " << node->binopData.type << " " << node->binopData.rhs;
        } break;
        case NodeType::PANIC: {
            cout << "panic()" << endl;
        } break;
        case NodeType::DECL_PARAM: {
            cout << node->paramData.name;
            if (node->paramData.type != nullptr) {
                cout << " : " << resolve(node->paramData.type);
            }
            if (node->paramData.value != nullptr) {
                cout << " = " << node->paramData.value;
            }
        } break;
        case NodeType::ARRAY_INDEX: {
            cout << node->arrayIndexData.target << "[" << node->arrayIndexData.indexValue << "]";
        } break;
        case NodeType::FN_CALL: {
            cout << node->fnCallData.fn->resolved;
            cout << "(";
            for (auto p : node->fnCallData.params) {
                cout << p << ", ";
            }
            cout << ")";
        } break;
        case NodeType::TAGCHECK: {
            cout << "tagcheck(" << node->nodeData << ")";
        } break;
        case NodeType::BOOLEAN_LITERAL: {
            cout << node->boolLiteralData.value;
        } break;
        case NodeType::PUTS: {
            cout << "puts(" << node->nodeData << ")";
        } break;
        case NodeType::FOR: {
            cout << "for " << node->forData.element_alias << " : " << node->forData.target << " {" << endl;
            for (auto stmt : node->forData.stmts) {
                cout << stmt;
                cout << endl;
            }
            cout << "}";
            cout << endl;
        } break;
        case NodeType::DEREF: {
            cout << "^" << node->derefData.target;
        } break;
        case NodeType::FLOAT_LITERAL: {
            cout << node->floatLiteralData.value;
        } break;
        default: assert(false);
    }

    for (auto stmt : node->postStmts) {
        cout << stmt;
    }

    return os;
}

/////////////
//  ATOMS  //
/////////////
AtomTable *AtomTable::current = nullptr;

AtomTable::AtomTable() {
    current = this;

    atoms = hash_t_init<string, int64_t>(1000);
}

int64_t AtomTable::insertStr(string s) {
    auto found = hash_get(atoms, s);
    if (found != nullptr) {
        return *found;
    }

    auto tableIndex = (int64_t) AtomTable::current->backwardAtoms.size();
    hash_insert(atoms, s, tableIndex);
    backwardAtoms.push_back(s);
    return tableIndex;
}

int64_t AtomTable::insert(Region &r) {
    auto sourceStr = r.srcInfo.source->substr(r.start.byteIndex, r.end.byteIndex - r.start.byteIndex);

    auto found = hash_get(atoms, sourceStr);
    if (found != nullptr) {
        return *found;
    }

    auto tableIndex = (int64_t) AtomTable::current->backwardAtoms.size();
    hash_insert(atoms, sourceStr, tableIndex);
    backwardAtoms.push_back(sourceStr);
    return tableIndex;
}

bool hasNoLocalByDefault(Node *node) {
    switch (resolve(node)->type) {
        case NodeType::INT_LITERAL:
        case NodeType::FLOAT_LITERAL:
        case NodeType::BOOLEAN_LITERAL:
        case NodeType::ADDRESS_OF:
        case NodeType::DEREF:
        case NodeType::FN_DECL:
        case NodeType::STRUCT_LITERAL:
        case NodeType::UNARY_NEG:
        case NodeType::CAST:
            return true;
        default: return false;
    }
}
