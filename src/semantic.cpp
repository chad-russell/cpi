#include "semantic.h"

#include <sstream>

int64_t typeSize(Node *type) {
    assert(type->type == NodeType::TYPE);

    switch (type->typeData.kind) {
        case NodeTypekind::INT_LITERAL:
        case NodeTypekind::I32:
        case NodeTypekind::FN:
            return 4;
        case NodeTypekind::I64:
            return 8;
        case NodeTypekind::NONE:
            return 0;
        default: assert(false);
    }
}

bool typesMatch(Node *desired, Node *actual) {
    assert(desired->type == NodeType::TYPE);
    assert(actual->type == NodeType::TYPE);

    // coercion from integer literal to any integer is valid
    // todo(chad): check for overflow?
    if (desired->typeData.kind == NodeTypekind::INT_LITERAL) {
        if (actual->typeData.kind == NodeTypekind::I32
            || actual->typeData.kind == NodeTypekind::I64) {
            desired->typeData.kind = actual->typeData.kind;
            return true;
        }
        else if (actual->typeData.kind == NodeTypekind::INT_LITERAL) {
            // if they're both literals then they become i64 by default
            desired->typeData.kind = NodeTypekind::I64;
            actual->typeData.kind = NodeTypekind::I64;
            return true;
        }
        return false;
    }
    if (actual->typeData.kind == NodeTypekind::INT_LITERAL) {
        if (desired->typeData.kind == NodeTypekind::I32
            || desired->typeData.kind == NodeTypekind::I64) {
            actual->typeData.kind = desired->typeData.kind;
            return true;
        }
        return false;
    }

    if (desired->typeData.kind != actual->typeData.kind) { return false; }

    return true;
}

void Semantic::reportError(vector<Node *> nodes, Error error) {
    encounteredErrors = true;

    auto seenBefore = false;
    for (auto node : nodes) {
        if (node != nullptr) {
            if (node->isUsedInError) {
                seenBefore = true;
            }
            node->isUsedInError = true;
        }
    }
    if (seenBefore) { return; }

    ostringstream s("");
    s << SourceInfoRegion{error.region}
      << Colored<string>{"error: ", {Color::FG_RED}, true}
      << Colored<string>{error.message, {Color::FG_DEFAULT}, true}
      << endl
      << HighlightedRegion{error.region}
      << endl;

    for (auto note : error.notes) {
        s << SourceInfoRegion{note.region}
          << Colored<string>{note.message, {Color::FG_BLUE}, true};
    }

    cout << s.str() << endl;
}

void resolveFnDecl(Semantic *semantic, Node *node) {
    auto data = &node->fnDeclData;
    auto savedFnDecl = semantic->currentFnDecl;
    semantic->currentFnDecl = data;

    for (auto param : data->params) {
        semantic->resolveTypes(param);
    }

    if (data->returnType != nullptr) {
        if (data->returns.empty()) {
            semantic->reportError({node},
                                  Error{node->region, "fn has a return type, but there are no return statements!"});
        }
    }
    else if (data->returns.empty()) {
        data->returnType = new Node(NodeTypekind::NONE);
    }
    else {
        auto firstReturn = data->returns[0];
        semantic->resolveTypes(firstReturn);
        data->returnType = firstReturn->typeInfo;
    }

    node->typeInfo = new Node(NodeTypekind::FN);
    node->typeInfo->typeData.fnTypeData.params = {};
    for (auto param : data->params) {
        node->typeInfo->typeData.fnTypeData.params.push_back(param);
    }

    // if the return type is just an int/float literal,
    // lock it down instead of leaving it open
    if (data->returnType != nullptr
        && data->returnType->typeData.kind == NodeTypekind::INT_LITERAL) {
        data->returnType->typeData.kind = NodeTypekind::I64;
    }
    if (data->returnType != nullptr
        && data->returnType ->typeData.kind == NodeTypekind::FLOAT_LITERAL) {
        data->returnType->typeData.kind = NodeTypekind::F64;
    }

    node->typeInfo->typeData.fnTypeData.returnType = data->returnType;

    semantic->currentFnDecl->stackSize += typeSize(data->returnType);

    for (auto ret : data->returns) {
        semantic->resolveTypes(ret);

         if (ret->typeInfo != nullptr) {
             if (!typesMatch(ret->typeInfo, data->returnType)) {
                 ostringstream s("");
                 s << "Type mismatch; function return type is "
                   << data->returnType->typeData.kind
                   << ", but this is "
                   << ret->typeInfo->typeData.kind;

                 semantic->reportError({ret, ret->typeInfo, data->returnType},
                 Error{ret->retData.value->region, s.str()});
             }
         }
         else {
             if (data->returnType == nullptr) {
                 auto note = Note{ret->retData.value->region, "because we cannot find the type of this returned value"};
                 semantic->reportError({node, ret},
                                       Error{node->region,
                                             "could not resolve return type for fn",
                                             {note}});
             }
             else {
                 semantic->reportError({node, ret, ret->retData.value},
                 Error{ret->retData.value->region, "could not resolved type of return statement"});
             }
         }
     }

    for (auto stmt : data->body) {
        semantic->resolveTypes(stmt);
    }

    // check to see everything worked out ok
    if (data->returnType == nullptr) {
        ostringstream s("");
        s << "could not resolve return type for fn '"
          << AtomTable::current->backwardAtoms[data->atomId]
          << "'";

        semantic->reportError({node}, Error{node->region, s.str()});
    }

    for (auto local : data->locals) {
        local->localOffset = semantic->currentFnDecl->stackSize;
        semantic->currentFnDecl->stackSize += typeSize(local->typeInfo);
    }

    semantic->currentFnDecl = savedFnDecl;
}

void resolveRet(Semantic *semantic, Node *node) {
    semantic->resolveTypes(node->retData.value);
    node->typeInfo = node->retData.value->typeInfo;
}

void resolveFloatLiteral(Semantic *semantic, Node *node) {
    node->typeInfo = new Node(NodeTypekind::FLOAT_LITERAL);
}

void resolveIntLiteral(Semantic *semantic, Node *node) {
    node->typeInfo = new Node(NodeTypekind::INT_LITERAL);
}

void resolveSymbol(Semantic *semantic, Node *node) {
    node->resolved = node->scope->find(node->symbolData.atomId);

    if (node->resolved == nullptr) {
        ostringstream s("");
        s << "undeclared identifier " << AtomTable::current->backwardAtoms[node->symbolData.atomId];
        semantic->reportError({node}, Error{node->region, s.str()});
        return;
    }

    assert(node->resolved->type == NodeType::DECL
           || node->resolved->type == NodeType::FN_DECL);
    semantic->resolveTypes(node->resolved);
    node->typeInfo = node->resolved->typeInfo;
}

void resolveDecl(Semantic *semantic, Node *node) {
    semantic->resolveTypes(node->declData.type);
    semantic->resolveTypes(node->declData.initialValue);

    if (node->declData.initialValue->typeInfo == nullptr) {
        semantic->reportError({node, node->declData.initialValue},
                              Error{node->declData.initialValue->region, "could not resolve type of initial value for this declaration"});
        return;
    }

    // if there is an initial value and a declared type, they should match
    if (node->declData.initialValue->typeInfo != nullptr
            && node->declData.type != nullptr
            && !typesMatch(node->declData.initialValue->typeInfo, node->declData.type)) {
        ostringstream s("");
        s << "Type mismatch! wanted " << node->declData.type->typeData.kind
          << ", got " << node->declData.initialValue->typeInfo->typeData.kind;
        semantic->reportError({node, node->declData.initialValue, node->declData.type},
                              Error{node->region, s.str()});
    }
    else if (node->declData.type == nullptr) {
        node->declData.type = node->declData.initialValue->typeInfo;
    }

    node->typeInfo = node->declData.type;
}

void resolveBinop(Semantic *semantic, Node *node) {
    semantic->resolveTypes(node->binopData.lhs);
    semantic->resolveTypes(node->binopData.rhs);

    if (!typesMatch(node->binopData.lhs->typeInfo, node->binopData.rhs->typeInfo)) {
        semantic->reportError({node, node->binopData.lhs, node->binopData.rhs},
                              Error{node->region, "type mismatch - both sides of binary operation need to be the same type"});
    }

    node->typeInfo = node->binopData.lhs->typeInfo;
}

void resolveFnCall(Semantic *semantic, Node *node) {
    semantic->resolveTypes(node->fnCallData.fn);
    auto resolvedFnType = resolve(node->fnCallData.fn->typeInfo)->typeData;
    assert(resolvedFnType.kind == NodeTypekind::FN);
    node->typeInfo = resolvedFnType.fnTypeData.returnType;
}

void resolveAssign(Semantic *semantic, Node *node) {
    semantic->resolveTypes(node->assignData.lhs);
    semantic->resolveTypes(node->assignData.rhs);
    if (!typesMatch(node->assignData.lhs->typeInfo, node->assignData.rhs->typeInfo)) {
        semantic->reportError({node, node->assignData.lhs, node->assignData.rhs},
                              Error{node->region, "assignment type mismatch"});
    }
}

void Semantic::resolveTypes(Node *node) {
    if (node == nullptr) { return; }

    if (node->semantic) { return; }
    node->semantic = true;

    switch (node->type) {
        case NodeType::FN_DECL: {
            resolveFnDecl(this, node);
        } break;
        case NodeType::RET: {
            resolveRet(this, node);
        } break;
        case NodeType::INT_LITERAL: {
            resolveIntLiteral(this, node);
        } break;
        case NodeType::FLOAT_LITERAL: {
            resolveFloatLiteral(this, node);
        } break;
        case NodeType::SYMBOL: {
            resolveSymbol(this, node);
        } break;
        case NodeType::DECL: {
            resolveDecl(this, node);
        } break;
        case NodeType::TYPE: {
            // nothing to do? or maybe we should figure out the size?
        } break;
        case NodeType::BINOP: {
            resolveBinop(this, node);
        } break;
        case NodeType::FN_CALL: {
            resolveFnCall(this, node);
        } break;
        case NodeType::ASSIGN: {
            resolveAssign(this, node);
        } break;
        default: assert(false);
    }
}