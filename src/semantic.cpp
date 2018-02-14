#include "semantic.h"

#include <sstream>

int32_t typeSize(Node *type) {
    assert(type->type == NodeType::TYPE);

    switch (type->typeData.kind) {
        case NodeTypekind::NONE:
            return 0;
        case NodeTypekind::BOOLEAN:
        case NodeTypekind::I32:
        case NodeTypekind::FN:
        case NodeTypekind::POINTER:
            return 4;
        case NodeTypekind::INT_LITERAL:
        case NodeTypekind::I64:
            return 8;
        case NodeTypekind::SYMBOL:
            return typeSize(type->resolved);
        case NodeTypekind::STRUCT: {
            auto total = 0;
            for (auto param : type->typeData.structTypeData.params) {
                param->localOffset = total;
                total += typeSize(param->declParamData.type);
            }
            return total;
        }
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

Node *localTarget(Node *local) {
    auto resolved = resolve(local);

    if (resolved->type == NodeType::DOT) {
        return localTarget(resolved->dotData.lhs);
    }

    return resolved;
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
          << AtomTable::current->backwardAtoms[data->name->symbolData.atomId]
          << "'";

        semantic->reportError({node}, Error{node->region, s.str()});
    }

    for (auto local : data->locals) {
        auto resolved = localTarget(local);
        if (resolved != local && resolved->isLocal) { continue; }

        local->localOffset = semantic->currentFnDecl->stackSize;
        semantic->currentFnDecl->stackSize += typeSize(local->typeInfo);
    }

    auto paramOffset = -8;
    for (auto declParam : node->fnDeclData.params) {
        // we push the params onto the stack in reverse order
        paramOffset -= typeSize(declParam->typeInfo);
        declParam->localOffset = paramOffset;
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
           || node->resolved->type == NodeType::FN_DECL
           || node->resolved->type == NodeType::DECL_PARAM);

    semantic->resolveTypes(node->resolved);
    node->typeInfo = node->resolved->typeInfo;
}

void resolveDecl(Semantic *semantic, Node *node) {
    semantic->resolveTypes(node->declData.type);
    semantic->resolveTypes(node->declData.initialValue);

    if (node->declData.initialValue != nullptr && node->declData.initialValue->typeInfo == nullptr) {
        semantic->reportError({node, node->declData.initialValue},
                              Error{node->declData.initialValue->region, "could not resolve type of initial value for this declaration"});
        return;
    }

    // if there is an initial value and a declared type, they should match
    if (node->declData.initialValue != nullptr && node->declData.initialValue->typeInfo != nullptr
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

void resolveType(Semantic *semantic, Node *node) {
    switch (node->typeData.kind) {
        case NodeTypekind::NONE:
        case NodeTypekind::INT_LITERAL:
        case NodeTypekind::I32:
        case NodeTypekind::I64:
            break;
        case NodeTypekind::FN: {
            for (auto param : node->typeData.fnTypeData.params) {
                semantic->resolveTypes(param);
            }
            semantic->resolveTypes(node->typeData.fnTypeData.returnType);
        } break;
        case NodeTypekind::STRUCT: {
            int32_t total = 0;
            for (auto param : node->typeData.structTypeData.params) {
                param->localOffset = total;
                semantic->resolveTypes(param);
                total += typeSize(param->declParamData.type);
            }
        } break;
        case NodeTypekind::SYMBOL: {
            node->resolved = node->scope->find(node->typeData.symbolTypeData.atomId);

            if (node->resolved == nullptr) {
                ostringstream s("");
                s << "undeclared identifier " << AtomTable::current->backwardAtoms[node->typeData.symbolTypeData.atomId];
                semantic->reportError({node}, Error{node->region, s.str()});
                return;
            }

            assert(node->resolved->type == NodeType::TYPE);

            semantic->resolveTypes(node->resolved);
        } break;
        case NodeTypekind::POINTER: {
            semantic->resolveTypes(node->typeData.pointerTypeData.underlyingType);
        } break;
        default: assert(false);
    }

    node->typeInfo = node;
}

Node *makeTemporary(Semantic *semantic, Node *n) {
    auto node = new Node();
    node->typeInfo = n->typeInfo;
    node->isLocal = true;
    semantic->currentFnDecl->locals.push_back(node);
    return node;
}

void resolveBinop(Semantic *semantic, Node *node) {
    semantic->resolveTypes(node->binopData.lhs);
    semantic->resolveTypes(node->binopData.rhs);

    node->binopData.lhsTemporary = makeTemporary(semantic, node->binopData.lhs);
    node->binopData.rhsTemporary = makeTemporary(semantic, node->binopData.rhs);

    if (node->binopData.lhs->type == NodeType::DOT) {
        auto newLocalStorage = new Node();
        newLocalStorage->type = NodeType::TYPE;
        newLocalStorage->typeInfo = node->binopData.lhs->typeInfo;
        node->binopData.lhs->dotData.nodeLocalStorage = newLocalStorage;

        newLocalStorage->isLocal = true;
        semantic->currentFnDecl->locals.push_back(newLocalStorage);
    }
    if (node->binopData.rhs->type == NodeType::DOT) {
        auto newLocalStorage = new Node();
        newLocalStorage->type = NodeType::TYPE;
        newLocalStorage->typeInfo = node->binopData.rhs->typeInfo;
        node->binopData.rhs->dotData.nodeLocalStorage = newLocalStorage;

        newLocalStorage->isLocal = true;
        semantic->currentFnDecl->locals.push_back(newLocalStorage);
    }

    if (!typesMatch(node->binopData.lhs->typeInfo, node->binopData.rhs->typeInfo)) {
        semantic->reportError({node, node->binopData.lhs, node->binopData.rhs},
                              Error{node->region, "type mismatch - both sides of binary operation need to be the same type"});
    }


    if (isBooleanBinop(node->binopData.type)) {
        node->typeInfo = new Node(NodeTypekind::BOOLEAN);
    } else {
        node->typeInfo = node->binopData.lhs->typeInfo;
    }
}

bool assignParams(Semantic *semantic,
                  Node *errorReportTarget,
                  const vector<Node *> &declParams,
                  vector<Node *> &givenParams) {
    vector<bool> openParams = {};
    vector<Node *> newParams = {};

    for (auto i = 0; i < declParams.size(); i++) {
        openParams.push_back(true);
        newParams.push_back(nullptr);
    }

    auto encounteredError = false;

    for (auto passedParam : givenParams) {
        if (passedParam->type == NodeType::PARAM && passedParam->paramData.name != nullptr) {
            // look up that name in the declaration
            auto found = false;
            for (unsigned long j = 0; j < declParams.size(); j++) {
                auto declParam = declParams[j];
                assert(declParam->type == NodeType::DECL_PARAM);

                if (declParam->declParamData.name->symbolData.atomId == passedParam->paramData.name->symbolData.atomId) {
                    found = true;
                    auto openParam = openParams.at(j);
                    if (!openParam) {
                        encounteredError = true;
                        semantic->reportError({errorReportTarget},
                                              Error{errorReportTarget->region, "passed a parameter twice"});
                    }
                    passedParam->typeInfo = declParam->declParamData.type;
                    newParams[j] = passedParam;
                    openParams[j] = false;
                }
            }
            if (!found) {
                encounteredError = true;
                semantic->reportError({errorReportTarget}, Error{"unable to find one of the parameters"});
            }
        }
        else {
            // put it in next open slot
            unsigned long j = 0;
            while (j < newParams.size() && !(openParams[j])) { j += 1; }
            if (j >= newParams.size()) {
                encounteredError = true;
                semantic->reportError({errorReportTarget}, Error{"too many parameters passed!"});
            }
            else {
                auto declParam = declParams[j];
                assert(declParam->type == NodeType::DECL_PARAM);
                passedParam->typeInfo = declParam->declParamData.type;

                newParams[j] = passedParam;
                openParams[j] = false;
            }
        }
    }

    for (unsigned long i = 0; i < newParams.size(); i++) {
        auto declParam = declParams[i];

        // if the decl parameter is unassigned but there is a default value, then fill in that param with the default
        auto openParamsI = openParams[i];
        if (declParam->declParamData.initialValue != nullptr && openParamsI) {
            openParams[i] = false;
            declParam->declParamData.initialValue->typeInfo = declParam->declParamData.type;
            newParams[i] = declParam->declParamData.initialValue;
        }
    }

    for (auto i = 0; i < declParams.size(); i++) {
        auto declParam = declParams[i];

        auto openParamsI = openParams[i];
        if (openParamsI || newParams[i] == nullptr) {
            encounteredError = true;
            semantic->reportError({errorReportTarget}, Error{errorReportTarget->region, "unassigned parameter!"});
        }
    }

    givenParams = newParams;

    return encounteredError;
}

void resolveFnCall(Semantic *semantic, Node *node) {
    semantic->resolveTypes(node->fnCallData.fn);
    auto resolvedFnType = resolve(node->fnCallData.fn->typeInfo)->typeData;
    assert(resolvedFnType.kind == NodeTypekind::FN);
    node->typeInfo = resolvedFnType.fnTypeData.returnType;

    auto declParams = resolvedFnType.fnTypeData.params;
    auto encounteredError = assignParams(semantic, node, declParams, node->fnCallData.params);

    if (encounteredError) { return; }

    for (unsigned long i = 0; i < node->fnCallData.params.size(); i++) {
        auto declParam = declParams[i];
        auto newParam = node->fnCallData.params[i];
        semantic->resolveTypes(newParam);
        if (!typesMatch(declParam->typeInfo, newParam->typeInfo)) {
            semantic->reportError({node, declParam, newParam}, Error{node->region, "type mismatch!"});
        }
    }
}

void resolveAssign(Semantic *semantic, Node *node) {
    semantic->resolveTypes(node->assignData.lhs);
    semantic->resolveTypes(node->assignData.rhs);
    if (!typesMatch(node->assignData.lhs->typeInfo, node->assignData.rhs->typeInfo)) {
        semantic->reportError({node, node->assignData.lhs, node->assignData.rhs},
                              Error{node->region, "assignment type mismatch"});
    }
}

void resolveDeclParam(Semantic *semantic, Node *node) {
    assert(!(node->declParamData.type == nullptr && node->declParamData.initialValue == nullptr));
    semantic->resolveTypes(node->declParamData.initialValue);
    if (node->declParamData.type == nullptr) {
        node->declParamData.type = node->declParamData.initialValue->typeInfo;
    }
    semantic->resolveTypes(node->declParamData.type);
    node->typeInfo = node->declParamData.type;
}

void resolveParam(Semantic *semantic, Node *node) {
    semantic->resolveTypes(node->paramData.value);
    node->typeInfo = node->paramData.value->typeInfo;
}

void resolveDeref(Semantic *semantic, Node *node) {
    semantic->resolveTypes(node->derefData.target);
    auto pointerType = node->derefData.target->typeInfo;
    assert(pointerType->typeData.kind == NodeTypekind::POINTER);
    node->typeInfo = pointerType->typeData.pointerTypeData.underlyingType;
}

void resolveAddressOf(Semantic *semantic, Node *node) {
    semantic->resolveTypes(node->nodeData);
    auto pointerTypeInfo = new Node(NodeTypekind::POINTER);
    pointerTypeInfo->typeData.pointerTypeData.underlyingType = node->nodeData->typeInfo;
    node->typeInfo = pointerTypeInfo;
}

Node *findParam(Semantic *semantic, Node *node) {
    auto resolvedTypeInfo = resolve(node->dotData.lhs)->typeInfo;
    assert(resolvedTypeInfo != nullptr);

    auto typeData = &resolve(resolvedTypeInfo)->typeData;
    while (typeData->kind == NodeTypekind::POINTER) {
        typeData = &resolve(typeData->pointerTypeData.underlyingType)->typeData;
    }
    if (typeData->kind != NodeTypekind::STRUCT) {
        semantic->reportError({node}, Error{node->region, "cannot perform dot operation on this!"});
    }
    auto structData = typeData->structTypeData;

    Node *foundParam = nullptr;
    for (auto param : structData.params) {
        if (param->declParamData.name->symbolData.atomId == node->dotData.rhs->symbolData.atomId) {
            foundParam = param;
            break;
        }
    }

    node->dotData.resolved = foundParam;

    return foundParam;
}

void resolveDot(Semantic *semantic, Node *node, Node *lhs, Node *rhs) {
    auto resolvedLhs = resolve(lhs);

    if (resolvedLhs->type == NodeType::DOT) {
        resolveDot(semantic, resolvedLhs, resolvedLhs->dotData.lhs, resolvedLhs->dotData.rhs);
        resolveDot(semantic, node, resolvedLhs->dotData.resolved, rhs);

        node->dotData.resolved = rhs->resolved;
        node->typeInfo = rhs->typeInfo;
    }
    else {
        semantic->resolveTypes(resolvedLhs);
        if (resolvedLhs->typeInfo == nullptr) {
            semantic->reportError({node, resolvedLhs}, Error{resolvedLhs->region, "cannot resolve type of lhs of dot operation"});
            return;
        }

        auto foundParam = findParam(semantic, node);
        semantic->resolveTypes(foundParam);

        if (foundParam == nullptr) {
            semantic->reportError({node}, Error{node->region, "invalid rhs in dot expression"});
        } else {
            rhs->resolved = foundParam;
            rhs->typeInfo = foundParam->typeInfo;

            node->dotData.resolved = foundParam;
            node->typeInfo = foundParam->typeInfo;
        }
    }

    semantic->resolveTypes(node->dotData.nodeLocalStorage);

    // if the lhs is a pointer, then our dot needs a pointer-sized local
    if (node->dotData.lhs->typeInfo->typeData.kind == NodeTypekind::POINTER) {
        node->dotData.autoDeref = true;

        auto local = new Node(NodeTypekind::POINTER);
        local->typeInfo = local;
        local->isLocal = true;
        semantic->currentFnDecl->locals.push_back(local);

        node->dotData.autoDerefStorage = local;
    }
}

void resolveDot(Semantic *semantic, Node *node) {
    resolveDot(semantic, node, resolve(node->dotData.lhs), node->dotData.rhs);
}

void resolveAssert(Semantic *semantic, Node *node) {
    semantic->resolveTypes(node->nodeData);
    if (node->nodeData->typeInfo->typeData.kind != NodeTypekind::BOOLEAN) {
        semantic->reportError({node, node->nodeData},
                              Error{node->nodeData->region, "condition for assert must be a boolean"});
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
            resolveType(this, node);
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
        case NodeType::DECL_PARAM: {
            resolveDeclParam(this, node);
        } break;
        case NodeType::PARAM: {
            resolveParam(this, node);
        } break;
        case NodeType::DEREF: {
            resolveDeref(this, node);
        } break;
        case NodeType::ADDRESS_OF: {
            resolveAddressOf(this, node);
        } break;
        case NodeType::DOT: {
            resolveDot(this, node);
        } break;
        case NodeType::ASSERT: {
            resolveAssert(this, node);
        } break;
        default: assert(false);
    }
}