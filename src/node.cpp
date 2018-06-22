#include "node.h"

Node::Node(SourceInfo srcInfo, NodeType type_, Scope *scope_) : Node() {
    type = type_;

    scope = scope_;

    typeInfo = nullptr;
    resolved = nullptr;
    region.srcInfo = srcInfo;

    switch (type_) {
        case NodeType::FN_DECL: {
            initFnDeclData(this);
        } break;
        case NodeType::DECL_PARAM:
        case NodeType::VALUE_PARAM: {
            initParamData(this);
        } break;
        case NodeType::DECL: {
            initDeclData(this);
        } break;
        case NodeType::ASSIGN: {
            initAssignData(this);
        } break;
        case NodeType::FN_CALL: {
            initFnCallData(this);
        } break;
        case NodeType::BINOP: {
            initBinopData(this);
        } break;
        case NodeType::DOT: {
            initDotData(this);
        } break;
        case NodeType::STRUCT_LITERAL: {
            initStructLiteralData(this);
        } break;
        case NodeType::IF: {
            initIfData(this);
        } break;
        case NodeType::WHILE: {
            initWhileData(this);
        } break;
        default: {}
    }
}

Node::Node(NodeTypekind typekind) : Node() {
    type = NodeType::TYPE;
    typeData.kind = typekind;
}

Node::Node() {
    id = nodeId;
    nodeId += 1;
}

void initWhileData(Node *node) {
    node->whileData.condition = nullptr;
    node->whileData.stmts = vector_init<Node *>(10);
}

void initIfData(Node *node) {
    node->ifData.condition = nullptr;
    node->ifData.stmts = vector_init<Node *>(10);
    node->ifData.elseStmts = vector_init<Node *>(10);
}

void initStructLiteralData(Node *node) {
  node->structLiteralData.params = vector_init<Node *>(10);
}

void initDotData(Node *node) {
    node->dotData.lhs = nullptr;
    node->dotData.rhs = nullptr;
    node->dotData.resolved = nullptr;
    node->dotData.autoDerefStorage = nullptr;
    node->dotData.pointerIsRelative = false;
}

void initBinopData(Node *node) {
    node->binopData.lhs = nullptr;
    node->binopData.rhs = nullptr;
    node->binopData.rhsScale = 1;
    node->binopData.lhsTemporary = nullptr;
    node->binopData.rhsTemporary = nullptr;
}

void initFnCallData(Node *node) {
    node->fnCallData.fn = nullptr;
    node->fnCallData.ctParams = vector_init<Node *>(10);
    node->fnCallData.params = vector_init<Node *>(10);
    node->fnCallData.hasRuntimeParams = true;
}

void initAssignData(Node *node) {
    node->assignData.lhs = nullptr;
    node->assignData.rhs = nullptr;
}

void initDeclData(Node *node) {
    node->declData.lvalue = nullptr;
    node->declData.type = nullptr;
    node->declData.initialValue = nullptr;
}

void initParamData(Node *node) {
    node->paramData.name = nullptr;
    node->paramData.type = nullptr;
    node->paramData.value = nullptr;
    node->paramData.polyLink = nullptr;
    node->paramData.index = 0;
}

void initFnDeclData(Node *node) {
    node->fnDeclData.name = nullptr;
    node->fnDeclData.ctParams = vector_init<Node *>(10);
    node->fnDeclData.params = vector_init<Node *>(10);
    node->fnDeclData.returnType = nullptr;
    node->fnDeclData.body = vector_init<Node *>(10);
    node->fnDeclData.locals = vector_init<Node *>(10);
    node->fnDeclData.returns = vector_init<Node *>(10);
    node->fnDeclData.stackSize = 0;
    node->fnDeclData.instOffset = 0;
    node->fnDeclData.cameFromPolymorph = false;
    node->fnDeclData.isLiteral = false;
    node->fnDeclData.tableIndex = 0;
}

void initStructTypeData(Node *node) {
    node->typeData.kind = NodeTypekind::STRUCT;

    node->typeData.structTypeData.isLiteral = false;
    node->typeData.structTypeData.params = vector_init<Node *>(10);
    node->typeData.structTypeData.isSecretlyArray = false;
    node->typeData.structTypeData.secretArrayElementType = nullptr;
    node->typeData.structTypeData.isSecretlyEnum = false;
    node->typeData.structTypeData.coercedType = nullptr;
}

void initFnTypeData(Node *node) {
    node->typeData.kind = NodeTypekind::FN;

    node->typeData.fnTypeData.returnType = nullptr;
    node->typeData.fnTypeData.params = vector_init<Node *>(10);
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

    symbols = hash_t_init<int64_t, Node *>(100);
}

Node *Scope::find(int64_t atomId) {
    auto found = hash_get(symbols, atomId);
    if (found != nullptr) {
        return *found;
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
            return os << "#run";
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

    auto countTy = new Node(NodeTypekind::I64);

    auto arrayType = new Node(NodeTypekind::STRUCT);
    initStructTypeData(arrayType);
    arrayType->typeData.structTypeData.isSecretlyArray = true;

    arrayType->typeData.structTypeData.secretArrayElementType = elementType;

    arrayType->typeData.structTypeData.params = vector_init<Node *>(2);
    vector_append(arrayType->typeData.structTypeData.params, wrapInDeclParam(ptrTy, "data", 0));
    vector_append(arrayType->typeData.structTypeData.params, wrapInDeclParam(countTy, "count", 1));

    return arrayType;
}

Node *wrapInValueParam(Node *value, Node *name) {
    auto valueParam = new Node();
    valueParam->type = NodeType::VALUE_PARAM;
    valueParam->paramData.value = value;
    valueParam->paramData.name = name;
    return valueParam;
}

Node *wrapInValueParam(Node *value, int64_t atomId) {
    auto valueParam = new Node();
    valueParam->type = NodeType::VALUE_PARAM;
    valueParam->paramData.value = value;

    auto nameNode = new Node();
    nameNode->type = NodeType::SYMBOL;
    nameNode->scope = value->scope;
    nameNode->region = value->region;
    nameNode->symbolData.atomId = atomId;

    valueParam->paramData.name = nameNode;

    return valueParam;
}

Node *wrapInValueParam(Node *value, string name) {
    auto valueParam = new Node();
    valueParam->type = NodeType::VALUE_PARAM;
    valueParam->paramData.value = value;

    if (!name.empty()) {
        auto nameNode = new Node();
        nameNode->type = NodeType::SYMBOL;
        nameNode->scope = value->scope;
        nameNode->region = value->region;

        nameNode->symbolData.atomId = AtomTable::current->insertStr(name);
        valueParam->paramData.name = nameNode;
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
    initParamData(param);
    param->paramData.type = type;
    param->paramData.index = index;
    param->paramData.name = name;
    param->typeInfo = type;

    return param;
}
