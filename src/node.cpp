#include "node.h"

Node::Node(SourceInfo srcInfo, NodeType type_, Scope *scope_) : Node() {
    type = type_;

    scope = scope_;

    typeInfo = nullptr;
    resolved = nullptr;
    region.srcInfo = srcInfo;
}

Node::Node(NodeTypekind typekind) : Node() {
    type = NodeType::TYPE;
    typeData.kind = typekind;
}

Node::Node() {
    id = nodeId;
    nodeId += 1;
}

Node *resolve(Node *n) {
    if (n == nullptr) { return nullptr; }

    auto resolved = n;

    while (resolved->resolved != nullptr || resolved->staticValue != nullptr) {
        if (resolved->resolved != nullptr) {
            resolved = resolved->resolved;
        } else {
            resolved = resolved->staticValue;
        }
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
        }
        case NodeType::SYMBOL: {
            return os << "symbol";
        }
        case NodeType::DECL: {
            return os << "declaration";
        }
        case NodeType::ASSIGN: {
            return os << "assign";
        }
        case NodeType::BINOP: {
            return os << "binary operation";
        }
        case NodeType::PIPE: {
            return os << "pipe";
        }
        case NodeType::DOT: {
            return os << "dot";
        }
        case NodeType::FN_CALL: {
            return os << "fn call";
        }
        case NodeType::INT_LITERAL: {
            return os << "int literal";
        }
        case NodeType::FLOAT_LITERAL: {
            return os << "float literal";
        }
        case NodeType::STRING_LITERAL: {
            return os << "string literal";
        }
        case NodeType::NIL_LITERAL: {
            return os << "nil literal";
        }
        case NodeType::BOOLEAN_LITERAL: {
            return os << "boolean literal";
        }
        case NodeType::RET: {
            return os << "return";
        }
        case NodeType::IF: {
            return os << "if";
        }
        case NodeType::WHILE: {
            return os << "while";
        }
        case NodeType::DECL_PARAM: {
            return os << "declaration parameter";
        }
        case NodeType::VALUE_PARAM: {
            return os << "param";
        }
        case NodeType::TYPE: {
            return os << "type";
        }
        case NodeType::DEREF: {
            return os << "dereference";
        }
        case NodeType::ADDRESS_OF: {
            return os << "address-of";
        }
        case NodeType::STRUCT_LITERAL: {
            return os << "struct literal";
        }
        case NodeType::UNARY_NOT: {
            return os << "unary not";
        }
        case NodeType::MODULE: {
            return os << "module";
        }
        case NodeType::IMPORT: {
            return os << "import";
        }
        case NodeType::CAST: {
            return os << "cast";
        }
        case NodeType::RUN: {
            return os << "run";
        }
        case NodeType::TYPEOF: {
            return os << "typeof";
        }
        case NodeType::RETTYPEOF: {
            return os << "rettypeof";
        }
        case NodeType::SIZEOF: {
            return os << "sizeof";
        }
        case NodeType::FIELDSOF: {
            return os << "fieldsof";
        }
        case NodeType::ARRAY_INDEX: {
            return os << "array index";
        }
        case NodeType::PANIC: {
            return os << "panic";
        }
        case NodeType::UNARY_NEG: {
            return os << "unary neg";
        }
        case NodeType::MALLOC: {
            return os << "malloc";
        }
        case NodeType::FREE: {
            return os << "free";
        }
        case NodeType::TAGCHECK: {
            return os << "tagcheck";
        }
        case NodeType::ARRAY_LITERAL: {
            return os << "array literal";
        }
        case NodeType::ISKIND: {
            return os << "#iskind";
        }
    }
}

Node *makeArrayType(Node *elementType) {
    auto ptrTy = new Node(NodeTypekind::POINTER);
    ptrTy->typeData.pointerTypeData.underlyingType = elementType;

    auto countTy = new Node(NodeTypekind::I32);

    auto arrayType = new Node(NodeTypekind::STRUCT);
    arrayType->typeData.structTypeData.isSecretlyArray = true;

    arrayType->typeData.structTypeData.secretArrayElementType = elementType;

    arrayType->typeData.structTypeData.params = {wrapInDeclParam(ptrTy, "data", 0), wrapInDeclParam(countTy, "count", 1)};

    return arrayType;
}

Node *wrapInValueParam(Node *value, Node *name) {
    auto valueParam = new Node();
    valueParam->type = NodeType::VALUE_PARAM;
    valueParam->valueParamData.value = value;
    valueParam->valueParamData.name = name;
    return valueParam;
}

Node *wrapInValueParam(Node *value, int64_t atomId) {
    auto valueParam = new Node();
    valueParam->type = NodeType::VALUE_PARAM;
    valueParam->valueParamData.value = value;

    auto nameNode = new Node();
    nameNode->type = NodeType::SYMBOL;
    nameNode->scope = value->scope;
    nameNode->region = value->region;
    nameNode->symbolData.atomId = atomId;

    valueParam->valueParamData.name = nameNode;

    return valueParam;
}

Node *wrapInValueParam(Node *value, string name) {
    auto valueParam = new Node();
    valueParam->type = NodeType::VALUE_PARAM;
    valueParam->valueParamData.value = value;

    if (!name.empty()) {
        auto nameNode = new Node();
        nameNode->type = NodeType::SYMBOL;
        nameNode->scope = value->scope;
        nameNode->region = value->region;

        nameNode->symbolData.atomId = AtomTable::current->insertStr(name);
        valueParam->valueParamData.name = nameNode;
    }

    return valueParam;
}

Node *wrapInDeclParam(Node *type, string name, int index) {
    Node *nameNode = nullptr;
    if (!name.empty()) {
        nameNode = new Node();
        nameNode->type = NodeType::SYMBOL;

        nameNode->symbolData.atomId = AtomTable::current->insertStr(name);
    }

    return wrapInDeclParam(type, nameNode, index);
}

Node *wrapInDeclParam(Node *type, Node *name, int index) {
    auto param = new Node();
    param->type = NodeType::DECL_PARAM;
    param->declParamData.type = type;
    param->declParamData.index = index;
    param->declParamData.name = name;
    param->typeInfo = type;

    return param;
}
