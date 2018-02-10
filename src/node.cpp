#include "node.h"

Node::Node(SourceInfo srcInfo, vector<Node *> *allNodes, NodeType type_, Scope *scope_) {
    id = nodeId;
    nodeId += 1;

    type = type_;

    scope = scope_;

    typeInfo = nullptr;
    resolved = nullptr;
    region.srcInfo = srcInfo;

    allNodes->push_back(this);
}

Node::Node(NodeTypekind typekind) {
    type = NodeType::TYPE;
    typeData.kind = typekind;
}

int32_t Node::fullOffset() {
    return localOffset + localStorageOffset;
}

Node *resolve(Node *n) {
    if (n == nullptr) { return nullptr; }
    if (n->resolved == nullptr) { return n; }

    auto resolved = n->resolved;
    while (resolved->resolved != nullptr) {
        resolved = resolved->resolved;
    }
    return resolved;
}

Scope::Scope(Scope *parent_) {
    parent = parent_;
}

Node *Scope::find(int64_t atomId) {
    auto found = symbols.find(atomId);
    if (found != symbols.end()) {
        return found->second;
    }

    if (parent != nullptr) {
        auto foundNode = parent->find(atomId);
        if (foundNode != nullptr) {
            return foundNode;
        }
    }

    return nullptr;
}

ostream &operator<<(ostream &os, NodeType type) {
    switch (type) {
    case NodeType::FN_DECL: {
        return os << "fn decl";
    } break;
    case NodeType::SYMBOL: {
        return os << "symbol";
    } break;
    case NodeType::DECL: {
        return os << "declaration";
    } break;
    case NodeType::ASSIGN: {
        return os << "assign";
    } break;
    case NodeType::BINOP: {
        return os << "binary operation";
    } break;
    case NodeType::PIPE: {
        return os << "pipe";
    } break;
    case NodeType::DOT: {
        return os << "dot";
    } break;
    case NodeType::FN_CALL: {
        return os << "fn call";
    } break;
    case NodeType::INT_LITERAL: {
        return os << "int literal";
    } break;
    case NodeType::FLOAT_LITERAL: {
        return os << "float literal";
    } break;
    case NodeType::STRING_LITERAL: {
        return os << "string literal";
    } break;
    case NodeType::NIL_LITERAL: {
        return os << "nil literal";
    } break;
    case NodeType::BOOLEAN_LITERAL: {
        return os << "boolean literal";
    } break;
    case NodeType::UNARY_NEG: {
        return os << "unary negation";
    } break;
    case NodeType::RET: {
        return os << "return";
    } break;
    case NodeType::IF: {
        return os << "if";
    } break;
    case NodeType::WHILE: {
        return os << "while";
    } break;
    case NodeType::DECL_PARAM: {
        return os << "declaration parameter";
    } break;
    case NodeType::PARAM: {
        return os << "param";
    } break;
    case NodeType::TYPE: {
        return os << "type";
    } break;
    case NodeType::DEREF: {
        return os << "dereference";
    } break;
    case NodeType::ADDRESS_OF: {
        return os << "address-of";
    } break;
    case NodeType::STRUCT_LITERAL: {
        return os << "struct literal";
    } break;
    case NodeType::UNARY_NOT: {
        return os << "unary not";
    } break;
    case NodeType::MODULE: {
        return os << "module";
    } break;
    case NodeType::IMPORT: {
        return os << "import";
    } break;
    case NodeType::CAST: {
        return os << "cast";
    } break;
    case NodeType::ASSERT: {
        return os << "assert";
    } break;
    }
}