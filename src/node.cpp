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
        case NodeType::ATTR: {
            initAttrData(this);
        } break;
        case NodeType::ATTROF: {
            initAttrofData(this);
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
        case NodeType::CAST: {
            initCastData(this);
        } break;
        case NodeType::ARRAY_LITERAL: {
            initArrayLiteralData(this);
        } break;
        case NodeType::FOR: {
            initForData(this);
        } break;
        case NodeType::MODULE: {
            initModuleData(this);
        } break;
        case NodeType::DEFER: {
            initDeferData(this);
        } break;
        case NodeType::END_SCOPE: {
            initEndScopeData(this);
        } break;
        case NodeType::PARAMETERIZED_TYPE: {
            initParameterizedTypeData(this);
        } break;
        case NodeType::TYPE: {
            initTypeData(this);
        } break;
        case NodeType::IMPORT: {
            initImportData(this);
        } break;
        default: {}
    }
}

Node::Node(NodeTypekind typekind) : Node() {
    type = NodeType::TYPE;
    typeData.kind = typekind;

    initTypeData(this);
}

Node::Node(Region r) {
    id = nodeId;
    nodeId += 1;

    this->region = r;
}

void initForData(Node *node) {
    node->forData.element_alias = nullptr;
    node->forData.iterator_alias = nullptr;
    node->forData.target = nullptr;
    node->forData.stmts = vector_init<Node *>(8);
    node->forData.rewritten = vector_init<Node *>(8);
    node->forData.isStatic = false;
    node->forData.staticStmts = vector_init<Node *>(8);
}

void initModuleData(Node *node) {
    node->moduleData.name = nullptr;
    node->moduleData.stmts = vector_init<Node *>(8);
}

void initDeferData(Node *node) {
    node->deferData.stmts = vector_init<Node *>(8);
}

void initEndScopeData(Node *node) {
    node->sourceMapStatement = true;
}

void initParameterizedTypeData(Node *node) {
    node->parameterizedTypeData.ctParams = vector_init<Node *>(4);
    node->parameterizedTypeData.typeDecl = nullptr;
    node->parameterizedTypeData.attributes = vector_init<Node *>(4);
}

void initImportData(Node *node) {
    node->importData.isFile = false;
    node->importData.target = nullptr;
    node->importData.alias = nullptr;
}

void initTypeData(Node *node) {
    node->typeData.attributes = (vector_t<Node *> *) malloc(sizeof(vector_t<Node *>));
    *node->typeData.attributes = vector_init<Node *>(4);

    node->typeData.name = nullptr;
    node->typeData.polyCameFrom = nullptr;

    node->typeData.polyParams = (vector_t<Node *> *) malloc(sizeof(vector_t<Node *>));
    *node->typeData.polyParams = vector_init<Node *>(4);
}

void initArrayLiteralData(Node *node) {
    node->arrayLiteralData.elementType = nullptr;
    node->arrayLiteralData.elements = vector_init<Node *>(8);
    node->arrayLiteralData.structLiteralRepresentation = nullptr;
    node->arrayLiteralData.allocFn = nullptr;
}

void initCastData(Node *node) {
    node->castData.type = nullptr;
    node->castData.value = nullptr;
}

void initWhileData(Node *node) {
    node->whileData.condition = nullptr;
    node->whileData.stmts = vector_init<Node *>(8);
}

void initIfData(Node *node) {
    node->ifData.condition = nullptr;
    node->ifData.stmts = vector_init<Node *>(8);
    node->ifData.elseStmts = vector_init<Node *>(8);

    node->ifData.isStatic = false;
    node->ifData.ifScope = nullptr;
    node->ifData.elseScope = nullptr;
    node->ifData.trueImports = vector_init<Node *>(8);
    node->ifData.falseImports = vector_init<Node *>(8);
    node->ifData.trueImpls = vector_init<Node *>(8);
    node->ifData.falseImpls = vector_init<Node *>(8);
}

void initStructLiteralData(Node *node) {
  node->structLiteralData.params = vector_init<Node *>(8);
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
    node->fnCallData.ctParams = vector_init<Node *>(8);
    node->fnCallData.params = vector_init<Node *>(8);
    node->fnCallData.hasRuntimeParams = true;
}

void initAssignData(Node *node) {
    node->assignData.lhs = nullptr;
    node->assignData.rhs = nullptr;
}

void initAttrData(Node *node) {
    node->attrData.target = nullptr;
    node->attrData.stmts = vector_init<Node *>(4);
}

void initAttrofData(Node *node) {
    node->attrofData.target = nullptr;
    node->attrofData.attr = nullptr;
}

void initDeclData(Node *node) {
    node->declData.lhs = nullptr;
    node->declData.type = nullptr;
    node->declData.initialValue = nullptr;
}

void initParamData(Node *node) {
    node->paramData.name = nullptr;
    node->paramData.type = nullptr;
    node->paramData.value = nullptr;
    node->paramData.polyLink = nullptr;
    node->paramData.polyCameFrom = nullptr;
    node->paramData.index = 0;
}

void initFnDeclData(Node *node) {
    node->fnDeclData.name = nullptr;
    node->fnDeclData.ctParams = vector_init<Node *>(8);
    node->fnDeclData.params = vector_init<Node *>(8);
    node->fnDeclData.returnType = nullptr;
    node->fnDeclData.body = vector_init<Node *>(8);
    node->fnDeclData.locals = vector_init<Node *>(8);
    node->fnDeclData.returns = vector_init<Node *>(8);
    node->fnDeclData.stackSize = 0;
    node->fnDeclData.instOffset = 0;
    node->fnDeclData.cameFromPolymorph = false;
    node->fnDeclData.isLiteral = false;
    node->fnDeclData.isExternal = false;
    node->fnDeclData.tableIndex = 0;
    node->fnDeclData.isImpl = false;
    node->fnDeclData.bodyScope = nullptr;
    node->fnDeclData.debugLocalOffset = 0;
}

void initStructTypeData(Node *node) {
    node->typeData.kind = NodeTypekind::STRUCT;

    node->typeData.structTypeData.isLiteral = false;
    node->typeData.structTypeData.params = vector_init<Node *>(8);
    node->typeData.structTypeData.isSecretlyArray = false;
    node->typeData.structTypeData.secretArrayElementType = nullptr;
    node->typeData.structTypeData.isSecretlyUnion = false;
    node->typeData.structTypeData.unionTagType = nullptr;
    node->typeData.structTypeData.coercedType = nullptr;
}

void initEnumTypeData(Node *node) {
    node->typeData.kind = NodeTypekind::ENUM;

    node->typeData.enumTypeData.type = nullptr;
    node->typeData.enumTypeData.params = vector_init<Node *>(16);
}

void initFnTypeData(Node *node) {
    node->typeData.kind = NodeTypekind::FN;

    node->typeData.fnTypeData.returnType = nullptr;
    node->typeData.fnTypeData.params = vector_init<Node *>(8);
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

    symbols = hash_init<int64_t, Node *>(100);
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
        case NodeType::RETURN: {
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
        case NodeType::UNARY_BITNOT: {
            return os << "unary bitnot";
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
        case NodeType::RETURNTYPEOF: {
            return os << "returntypeof";
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
    auto valueParam = new Node(value->region);
    valueParam->type = NodeType::VALUE_PARAM;
    valueParam->paramData.value = value;
    valueParam->paramData.name = name;
    return valueParam;
}

Node *wrapInValueParam(Node *value, int64_t atomId) {
    auto valueParam = new Node(value->region);
    valueParam->type = NodeType::VALUE_PARAM;
    valueParam->paramData.value = value;

    auto nameNode = new Node(value->region);
    nameNode->type = NodeType::SYMBOL;
    nameNode->scope = value->scope;
    nameNode->region = value->region;
    nameNode->symbolData.atomId = atomId;

    valueParam->paramData.name = nameNode;

    return valueParam;
}

Node *wrapInValueParam(Node *value, string name) {
    auto valueParam = new Node(value->region);
    valueParam->type = NodeType::VALUE_PARAM;
    valueParam->paramData.value = value;

    if (!name.empty()) {
        auto nameNode = new Node(value->region);
        nameNode->type = NodeType::SYMBOL;
        nameNode->scope = value->scope;
        nameNode->region = value->region;

        nameNode->symbolData.atomId = atomTable->insertStr(name);
        valueParam->paramData.name = nameNode;
    }

    return valueParam;
}

Node *wrapInDeclParam(Node *type, string name, int index) {
    Node *nameNode = nullptr;
    if (!name.empty()) {
        nameNode = new Node();
        nameNode->type = NodeType::SYMBOL;

        nameNode->symbolData.atomId = atomTable->insertStr(name);
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
