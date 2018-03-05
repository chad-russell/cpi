#include "semantic.h"
#include "bytecodegen.h"
#include "interpreter.h"
#include "parser.h"

#include <sstream>
#include <memory>

int32_t typeSize(Node *type) {
    auto resolved = resolve(type);
    assert(resolved->type == NodeType::TYPE);

    switch (resolved->typeData.kind) {
        case NodeTypekind::NONE:
            return 0;
        case NodeTypekind::I8:
            return 1;
        case NodeTypekind::BOOLEAN:
        case NodeTypekind::I32:
        case NodeTypekind::FN:
        case NodeTypekind::FLOAT_LITERAL:
        case NodeTypekind::F32:
            return 4;
        case NodeTypekind::POINTER:
        case NodeTypekind::INT_LITERAL:
        case NodeTypekind::I64:
        case NodeTypekind::F64:
            return 8;
        case NodeTypekind::SYMBOL:
            assert(false);
        case NodeTypekind::STRUCT: {
            auto total = 0;
            auto index = 0;

            for (auto param : resolved->typeData.structTypeData.params) {
                param->localOffset = total;

                if (resolved->typeData.structTypeData.isLiteral) {
                    total += typeSize(param->valueParamData.value->typeInfo);
                } else {
                    total += typeSize(param->declParamData.type);
                    index += 1;
                }

            }

            return total;
        }
        default:
            assert(false);
    }
}

bool typesMatch(Node *desired, Node *actual) {
    desired = resolve(desired);
    actual = resolve(actual);

    assert(desired->type == NodeType::TYPE);
    assert(actual->type == NodeType::TYPE);

    // exposed types match exposed types, always
    if (desired->typeData.kind == NodeTypekind::EXPOSED_TYPE && actual->typeData.kind == NodeTypekind::EXPOSED_TYPE) {
        return true;
    }

    // but exposed types also match concrete types if the exposed type has a static value
    // and it matches the given type.
    if (actual->typeData.kind == NodeTypekind::EXPOSED_TYPE) {
        if (actual->staticValue != nullptr) {
            actual = actual->staticValue;
        }
    }
    if (desired->typeData.kind == NodeTypekind::EXPOSED_TYPE) {
        if (desired->staticValue != nullptr) {
            desired = desired->staticValue;
        }
    }

    // coercion from integer literal to any integer is valid
    // todo(chad): check for overflow?
    if (desired->typeData.kind == NodeTypekind::INT_LITERAL) {
        if (actual->typeData.kind == NodeTypekind::I8
            || actual->typeData.kind == NodeTypekind::I32
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
        if (desired->typeData.kind == NodeTypekind::I8
            || desired->typeData.kind == NodeTypekind::I32
            || desired->typeData.kind == NodeTypekind::I64) {
            actual->typeData.kind = desired->typeData.kind;
            return true;
        }
        return false;
    }

    // coercion from float literal to any float is valid
    // todo(chad): check for overflow?
    if (desired->typeData.kind == NodeTypekind::FLOAT_LITERAL) {
        if (actual->typeData.kind == NodeTypekind::F32 || actual->typeData.kind == NodeTypekind::F64) {
            desired->typeData.kind = actual->typeData.kind;
            return true;
        }
        else if (actual->typeData.kind == NodeTypekind::FLOAT_LITERAL) {
            // if they're both literals then they become f32 by default
            desired->typeData.kind = NodeTypekind::F32;
            actual->typeData.kind = NodeTypekind::F32;
            return true;
        }
        return false;
    }
    if (actual->typeData.kind == NodeTypekind::FLOAT_LITERAL) {
        if (desired->typeData.kind == NodeTypekind::F32 || desired->typeData.kind == NodeTypekind::F64) {
            actual->typeData.kind = desired->typeData.kind;
            return true;
        }
        return false;
    }

    if (actual->typeData.kind == NodeTypekind::STRUCT) {
        if (desired->typeData.kind != NodeTypekind::STRUCT) {
            return false;
        }

        if (actual->typeData.structTypeData.params.size() != desired->typeData.structTypeData.params.size()) {
            return false;
        }

        for (auto i = 0; i < desired->typeData.structTypeData.params.size(); i++) {
            if (!typesMatch(desired->typeData.structTypeData.params[i]->typeInfo, actual->typeData.structTypeData.params[i]->typeInfo)) {
                return false;
            }
        }

        return true;
    }

    return desired->typeData.kind == actual->typeData.kind;
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

    auto index = 0;
    for (auto param : node->fnDeclData.params) {
        param->declParamData.index = index;
        index += 1;
    }

    if (!data->ctParams.empty() && !data->cameFromPolymorph) {
        return;
    }

    auto savedFnDecl = semantic->currentFnDecl;
    semantic->currentFnDecl = data;

    data->tableIndex = fnTableId;
    fnTableId += 1;

    for (auto param : data->ctParams) {
        semantic->resolveTypes(param);
    }

    for (auto param : data->params) {
        semantic->resolveTypes(param);
    }

    if (data->body.empty()) {
        if (data->returnType == nullptr) {
            data->returnType = new Node(NodeTypekind::NONE);
        }
    }
    else if (data->returnType != nullptr && data->returns.empty()) {
        semantic->reportError({node}, Error{node->region, "fn has a return type, but there are no return statements!"});
    }
    else if (data->returns.empty()) {
        data->returnType = new Node(NodeTypekind::NONE);
    }
    else if (data->returnType == nullptr) {
        auto firstReturn = data->returns[0];
        semantic->resolveTypes(firstReturn);
        data->returnType = firstReturn->typeInfo;
    }

    assert(data->returnType != nullptr);
    semantic->resolveTypes(data->returnType);

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
        data->returnType->typeData.kind = NodeTypekind::F32;
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

    auto localIndex = 0;
    for (auto local : data->locals) {
        auto resolvedLocal = resolve(local);
        if (resolvedLocal != local && resolvedLocal->isLocal) { continue; }

        local->localOffset = semantic->currentFnDecl->stackSize;

        semantic->currentFnDecl->stackSize += typeSize(local->typeInfo);
        localIndex += 1;
    }

    auto paramOffset = -8;
    localIndex = 0;
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
    if (node->typeInfo) { return; }

    node->typeInfo = new Node(NodeTypekind::INT_LITERAL);
}

void resolveBooleanLiteral(Semantic *semantic, Node *node) {
    node->typeInfo = new Node(NodeTypekind::BOOLEAN);
}

void resolveArrayIndex(Semantic *semantic, Node *node) {
    semantic->resolveTypes(node->arrayIndexData.target);
    semantic->resolveTypes(node->arrayIndexData.indexValue);

    switch (node->arrayIndexData.indexValue->typeInfo->typeData.kind) {
        case NodeTypekind::INT_LITERAL:
        case NodeTypekind::I32:
        case NodeTypekind::I64:
            break;
        default: {
            semantic->reportError({node->arrayIndexData.target},
                                  Error{node->arrayIndexData.target->region,
                                        "expected some kind of integer for array index"});
        }
    }

    if (node->arrayIndexData.target->typeInfo->typeData.kind != NodeTypekind::STRUCT) {
        semantic->reportError({node}, Error{node->region, "cannot index a non-array"});
        return;
    }
    if (!node->arrayIndexData.target->typeInfo->typeData.structTypeData.isSecretlyArray) {
        semantic->reportError({node}, Error{node->region, "cannot index a non-array"});
        return;
    }

    node->typeInfo = node->arrayIndexData.target->typeInfo->typeData.structTypeData.secretArrayElementType;

    // a[i] <==> ^((cast(*i32) a) + i * sizeof(i32)), for example (if typeof(a) is []i32)
    // todo(chad): our size calculations may not always be the same as llvm's, because of alignment ets. etc. So make a sizeof(...) operator
    auto typeSizeNode = new Node();
    typeSizeNode->type = NodeType::INT_LITERAL;
    typeSizeNode->intLiteralData.value = typeSize(node->typeInfo);

    auto iTimesElementSize = new Node(node->region.srcInfo, nullptr, NodeType::BINOP, node->scope);
    iTimesElementSize->binopData.type = LexerTokenType::MUL;
    iTimesElementSize->binopData.lhs = node->arrayIndexData.indexValue;
    iTimesElementSize->binopData.rhs = typeSizeNode;

    semantic->addLocal(iTimesElementSize);

    auto aCasted = new Node();
    aCasted->type = NodeType::CAST;
    aCasted->castData.isCastFromArrayToDataPtr = true;
    aCasted->castData.type = new Node(NodeTypekind::POINTER);
    aCasted->castData.type->typeData.pointerTypeData.underlyingType = node->typeInfo;
    aCasted->castData.value = node->arrayIndexData.target;

    auto add = new Node(node->region.srcInfo, nullptr, NodeType::BINOP, node->scope);
    add->binopData.type = LexerTokenType::ADD;
    add->binopData.lhs = aCasted;
    add->binopData.rhs = iTimesElementSize;

    auto deref = new Node(node->region.srcInfo, nullptr, NodeType::DEREF, node->scope);
    deref->derefData.target = add;

    semantic->addLocal(deref->derefData.target);
    semantic->addLocal(deref);

    node->resolved = deref;

    semantic->resolveTypes(node->resolved);
}

void resolveCast(Semantic *semantic, Node *node) {
    semantic->resolveTypes(node->castData.type);
    semantic->resolveTypes(node->castData.value);

    node->typeInfo = node->castData.type;
}

void resolveStringLiteral(Semantic *semantic, Node *node) {
    // typeInfo = []i8;
    node->typeInfo = makeArrayType(new Node(NodeTypekind::I8));
    semantic->resolveTypes(node->typeInfo);

    // "hello" <==> {&{'h', 'e', 'l', 'l', 'o'}, 5};
    auto arrayLiteral = new Node(node->region.srcInfo, nullptr, NodeType::STRUCT_LITERAL, node->scope);
    arrayLiteral->region = node->region;

    // charArrayLiteral = {'h', 'e', 'l', 'l', 'o'}
    auto charArrayLiteral = new Node();
    charArrayLiteral->type = NodeType::STRUCT_LITERAL;

    for (auto c : node->stringLiteralData.value) {
        auto charNode = new Node();
        charNode->type = NodeType::INT_LITERAL;
        charNode->typeInfo = new Node(NodeTypekind::I8);
        charNode->intLiteralData.value = static_cast<int64_t>(c);

        charArrayLiteral->structLiteralData.params.push_back(wrapInValueParam(charNode, ""));
    }

    // addrOfCharArrayLiteral = &{'h', 'e', 'l', 'l', 'o'}
    auto addrOfCharArrayLiteral = new Node();
    addrOfCharArrayLiteral->type = NodeType::ADDRESS_OF;
    addrOfCharArrayLiteral->nodeData = charArrayLiteral;

    semantic->addLocal(addrOfCharArrayLiteral->nodeData);

    // countNode = 5
    auto countNode = new Node();
    countNode->type = NodeType::INT_LITERAL;
    countNode->typeInfo = new Node(NodeTypekind::I32);
    countNode->intLiteralData.value = static_cast<int64_t>(node->stringLiteralData.value.size());

    // arrayLiteral = {&{'h', 'e', 'l', 'l', 'o'}, 5}
    arrayLiteral->structLiteralData.params.push_back(wrapInValueParam(addrOfCharArrayLiteral, "data"));
    arrayLiteral->structLiteralData.params.push_back(wrapInValueParam(countNode, "count"));

    semantic->resolveTypes(arrayLiteral);

    node->resolved = arrayLiteral;
}

void resolveNilLiteral(Semantic *semantic, Node *node) {
    node->typeInfo = new Node(NodeTypekind::POINTER);
    node->typeInfo->typeData.pointerTypeData.underlyingType = new Node(NodeTypekind::NONE);
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
           || node->resolved->type == NodeType::DECL_PARAM
           || node->resolved->type == NodeType::TYPE);

    semantic->resolveTypes(node->resolved);
    node->typeInfo = node->resolved->typeInfo;
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
        if (passedParam->type == NodeType::VALUE_PARAM && passedParam->valueParamData.name != nullptr) {
            // look up that name in the declaration
            auto found = false;
            for (unsigned long j = 0; j < declParams.size(); j++) {
                auto declParam = declParams[j];
                assert(declParam->type == NodeType::DECL_PARAM);

                if (declParam->declParamData.name->symbolData.atomId == passedParam->valueParamData.name->symbolData.atomId) {
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
        auto openParamsI = openParams[i];
        if (openParamsI || newParams[i] == nullptr) {
            encounteredError = true;
            semantic->reportError({errorReportTarget}, Error{errorReportTarget->region, "unassigned parameter!"});
        }
    }

    givenParams = newParams;

    return encounteredError;
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
    auto shouldCheckTypeMatch = node->declData.initialValue != nullptr
                                && node->declData.initialValue->typeInfo != nullptr
                                && node->declData.type != nullptr;
    if (shouldCheckTypeMatch && !typesMatch(node->declData.type, node->declData.initialValue->typeInfo)) {
        ostringstream s("");
        s << "Type mismatch! wanted " << node->declData.type->typeData.kind
          << ", got " << node->declData.initialValue->typeInfo->typeData.kind;
        semantic->reportError({node, node->declData.initialValue, node->declData.type},
                              Error{node->region, s.str()});
    }
    else if (node->declData.type == nullptr) {
        node->declData.type = node->declData.initialValue->typeInfo;
    }

    if (shouldCheckTypeMatch && resolve(node->declData.type)->typeData.kind == NodeTypekind::STRUCT
            && node->declData.initialValue->type == NodeType::STRUCT_LITERAL) {
        assignParams(semantic, node, resolve(node->declData.type)->typeData.structTypeData.params, node->declData.initialValue->structLiteralData.params);
    }

    node->typeInfo = node->declData.type;
}

void resolveType(Semantic *semantic, Node *node) {
    switch (node->typeData.kind) {
        case NodeTypekind::NONE:
        case NodeTypekind::BOOLEAN:
        case NodeTypekind::EXPOSED_TYPE:
        case NodeTypekind::EXPOSED_ANY:
        case NodeTypekind::INT_LITERAL:
        case NodeTypekind::FLOAT_LITERAL:
        case NodeTypekind::I8:
        case NodeTypekind::I32:
        case NodeTypekind::I64:
        case NodeTypekind::F32:
        case NodeTypekind::F64:
            break;
        case NodeTypekind::FN: {
            for (auto param : node->typeData.fnTypeData.params) {
                semantic->resolveTypes(param);
            }
            semantic->resolveTypes(node->typeData.fnTypeData.returnType);
        } break;
        case NodeTypekind::STRUCT: {
            auto total = 0;
            auto localIndex = 0;

            for (auto param : node->typeData.structTypeData.params) {
                param->localOffset = total;
                param->declParamData.index = localIndex;

                semantic->resolveTypes(param);

                total += typeSize(param->declParamData.type);
                localIndex += 1;
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

            auto resolved = resolve(node);

            semantic->resolveTypes(resolved);
        } break;
        case NodeTypekind::POINTER: {
            semantic->resolveTypes(node->typeData.pointerTypeData.underlyingType);
        } break;
        default: assert(false);
    }

    node->typeInfo = new Node(NodeTypekind::EXPOSED_TYPE);
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

        newLocalStorage->isLocal = true;
        semantic->currentFnDecl->locals.push_back(newLocalStorage);
    }
    if (node->binopData.rhs->type == NodeType::DOT) {
        auto newLocalStorage = new Node();
        newLocalStorage->type = NodeType::TYPE;
        newLocalStorage->typeInfo = node->binopData.rhs->typeInfo;

        newLocalStorage->isLocal = true;
        semantic->currentFnDecl->locals.push_back(newLocalStorage);
    }

    if (node->binopData.lhs->typeInfo->typeData.kind == NodeTypekind::POINTER
        && (node->binopData.rhs->typeInfo->typeData.kind == NodeTypekind::INT_LITERAL
            || node->binopData.rhs->typeInfo->typeData.kind == NodeTypekind::I32
            || node->binopData.rhs->typeInfo->typeData.kind == NodeTypekind::I64)) {
        node->typeInfo = node->binopData.lhs->typeInfo;
        return;
    }

    if (node->binopData.rhs->typeInfo->typeData.kind == NodeTypekind::POINTER
        && (node->binopData.lhs->typeInfo->typeData.kind == NodeTypekind::INT_LITERAL
            || node->binopData.lhs->typeInfo->typeData.kind == NodeTypekind::I32
            || node->binopData.lhs->typeInfo->typeData.kind == NodeTypekind::I64)) {
        node->typeInfo = node->binopData.rhs->typeInfo;
        return;
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

Node *Semantic::deepCopy(Node *node, Scope *scope) {
    // for now make sure we only copy fn decls
    assert(node->type == NodeType::FN_DECL);

    auto copyingLexer = new Lexer(lexer, node);

    auto copyingParser = new Parser(copyingLexer);
    copyingParser->scopes.pop();
    copyingParser->scopes.push(scope);
    return copyingParser->parseFnDecl(true);
}

void resolveFnCall(Semantic *semantic, Node *node) {
    semantic->resolveTypes(node->fnCallData.fn);
    auto resolvedFn = resolve(node->fnCallData.fn);
    auto isPoly = resolvedFn->type == NodeType::FN_DECL && !resolvedFn->fnDeclData.ctParams.empty();

    if (isPoly) {
        // make a new function
        // todo(chad): memoize this based on the ctParams
        auto newResolvedFn = semantic->deepCopy(resolvedFn, resolvedFn->scope);
        assert(newResolvedFn->type == NodeType::FN_DECL);
        newResolvedFn->fnDeclData.cameFromPolymorph = true;

        node->fnCallData.fn->resolved = newResolvedFn;
        resolvedFn = newResolvedFn;
    }

    // assign runtime params
    if (node->fnCallData.hasRuntimeParams) {
        vector<Node *> declParams;

        if (isPoly) {
            assert(resolvedFn->type == NodeType::FN_DECL);
            declParams = resolvedFn->fnDeclData.params;
        } else {
            auto resolvedFnType = resolve(resolvedFn->typeInfo)->typeData;
            assert(resolvedFnType.kind == NodeTypekind::FN);

            declParams = resolvedFnType.fnTypeData.params;
        }

        auto givenParams = node->fnCallData.params;
        auto encounteredError = assignParams(semantic, node, declParams, givenParams);
        assert(!encounteredError);

        if (isPoly) {
            // 'link' each decl param to its runtime param
            for (auto p = 0; p < declParams.size(); p++) {
                declParams[p]->declParamData.polyLink = givenParams[p];
            }
        }
    }

    // assign compile-time params
    if (isPoly) {
        auto ctDeclParams = resolvedFn->fnDeclData.ctParams;
        auto ctGivenParams = node->fnCallData.ctParams;
        auto encounteredError = assignParams(semantic, node, ctDeclParams, ctGivenParams);
        assert(!encounteredError);

        // Make sure the ctDeclParams resolve to their compile-time values
        for (auto i = 0; i < ctGivenParams.size(); i++) {
            const auto& ctParam = ctGivenParams[i];

            if (ctParam->type == NodeType::VALUE_PARAM) {
                auto ctValue = ctParam->valueParamData.value;
                semantic->resolveTypes(ctValue);
                ctDeclParams[i]->staticValue = ctValue;
            } else {
                ctDeclParams[i]->staticValue = ctParam;
            }

            semantic->resolveTypes(ctParam);
        }
    }

    // un-assign runtime params
    if (node->fnCallData.hasRuntimeParams) {
        vector<Node *> declParams;

        if (isPoly) {
            assert(resolvedFn->type == NodeType::FN_DECL);
            declParams = resolvedFn->fnDeclData.params;
        } else {
            auto resolvedFnType = resolve(resolvedFn->typeInfo)->typeData;
            assert(resolvedFnType.kind == NodeTypekind::FN);

            declParams = resolvedFnType.fnTypeData.params;
        }

        if (isPoly) {
            // un-link
            for (auto p = 0; p < declParams.size(); p++) {
                declParams[p]->declParamData.polyLink = nullptr;
            }

            // @Hack!!!
            for (auto d : declParams) {
                d->semantic = false;
                semantic->resolveTypes(d);
            }
        }
    }

    if (isPoly) {
        auto ctDeclParams = resolvedFn->fnDeclData.ctParams;
        auto ctGivenParams = node->fnCallData.ctParams;

        for (unsigned long i = 0; i < ctGivenParams.size(); i++) {
            auto declParam = ctDeclParams[i];
            auto givenParam = ctGivenParams[i];

            semantic->resolveTypes(declParam);
            semantic->resolveTypes(givenParam);

            // if declParam is an exposed 'Any' type, convert the givenParam to that
            if (declParam->typeInfo->typeData.kind == NodeTypekind::EXPOSED_ANY) {
                givenParam->typeInfo = new Node(NodeTypekind::EXPOSED_ANY);
                givenParam->typeInfo->staticValue = givenParam;
            }
            else {
                if (!typesMatch(declParam->typeInfo, givenParam->valueParamData.value->typeInfo)) {
                    // todo if match exposed to real type then set resolved on exposed to real and pass match
                    // unless already set then error?
                    semantic->reportError({node, declParam, givenParam}, Error{node->region, "static type mismatch!"});
                }
            }

            semantic->resolveTypes(givenParam);
        }
    }

    semantic->resolveTypes(resolvedFn);

    if (node->fnCallData.hasRuntimeParams) {
        auto resolvedFnType = resolve(resolvedFn->typeInfo)->typeData;
        assert(resolvedFnType.kind == NodeTypekind::FN);

        auto declParams = resolvedFnType.fnTypeData.params;
        auto givenParams = node->fnCallData.params;

        for (unsigned long i = 0; i < givenParams.size(); i++) {
            auto declParam = declParams[i];
            auto givenParam = givenParams[i];
            semantic->resolveTypes(givenParam);
            if (!typesMatch(declParam->typeInfo, givenParam->typeInfo)) {
                semantic->reportError({node, declParam, givenParam}, Error{node->region, "type mismatch!"});
            }
        }

        node->typeInfo = resolvedFnType.fnTypeData.returnType;
    } else {
        node->resolved = resolvedFn;
        node->typeInfo = resolvedFn->typeInfo;
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
    if (node->declParamData.polyLink != nullptr) {
        semantic->resolveTypes(node->declParamData.polyLink);
        node->typeInfo = node->declParamData.polyLink->typeInfo;
    } else {
        node->typeInfo = node->declParamData.type;
    }

    if (node->declParamData.type != nullptr && node->declParamData.initialValue != nullptr) {
        if (!typesMatch(node->declParamData.type, node->declParamData.initialValue->typeInfo)) {
            semantic->reportError({node}, Error{node->region, "Type mismatch decl param"});
        }
    }
}

void resolveParam(Semantic *semantic, Node *node) {
    semantic->resolveTypes(node->valueParamData.value);
    node->typeInfo = node->valueParamData.value->typeInfo;
}

void resolveDeref(Semantic *semantic, Node *node) {
    semantic->resolveTypes(node->derefData.target);
    auto pointerType = resolve(node->derefData.target->typeInfo);
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
    if (structData.isLiteral) {
        for (auto param : structData.params) {
            if (param->valueParamData.name->symbolData.atomId == node->dotData.rhs->symbolData.atomId) {
                foundParam = param;
                break;
            }
        }
    } else {
        for (auto param : structData.params) {
            if (param->declParamData.name->symbolData.atomId == node->dotData.rhs->symbolData.atomId) {
                foundParam = param;
                break;
            }
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

    // if the lhs is a pointer, then our dot needs a pointer-sized local
    if (node->dotData.lhs->typeInfo->typeData.kind == NodeTypekind::POINTER) {
        auto local = new Node(NodeTypekind::POINTER);
        local->typeInfo = local;
        local->isLocal = true;
        local->isAutoDerefStorage = true;
        semantic->currentFnDecl->locals.push_back(local);

        node->dotData.autoDerefStorage = local;
    }
}

void resolveDot(Semantic *semantic, Node *node) {
    resolveDot(semantic, node, resolve(node->dotData.lhs), node->dotData.rhs);
}

void resolvePanic(Semantic *semantic, Node *node) {
    // nothing to do here, we're just gonna panic anyway...
}

void resolveStructLiteral(Semantic *semantic, Node *node) {
    node->typeInfo = new Node(NodeTypekind::STRUCT);
    node->typeInfo->typeData.structTypeData.isLiteral = true;

    auto paramIndex = 0;
    for (auto param : node->structLiteralData.params) {
        semantic->resolveTypes(param->valueParamData.value);
        node->typeInfo->typeData.structTypeData.params.push_back(param);
        param->typeInfo = param->valueParamData.value->typeInfo;

        param->valueParamData.index = paramIndex;
        paramIndex += 1;
    }
}

void resolveIf(Semantic *semantic, Node *node) {
    semantic->resolveTypes(node->ifData.condition);

    if (node->ifData.condition->typeInfo->typeData.kind != NodeTypekind::BOOLEAN) {
        semantic->reportError({node, node->ifData.condition}, Error{node->ifData.condition->region, "Condition for 'if' must be a boolean!"});
    }

    for (const auto& stmt : node->ifData.stmts) {
        semantic->resolveTypes(stmt);
    }
    for (const auto& stmt : node->ifData.elseStmts) {
        semantic->resolveTypes(stmt);
    }
}

void resolveWhile(Semantic *semantic, Node *node) {
    semantic->resolveTypes(node->whileData.condition);

    if (node->whileData.condition->typeInfo->typeData.kind != NodeTypekind::BOOLEAN) {
        semantic->reportError({node, node->whileData.condition}, Error{node->whileData.condition->region, "Condition for 'while' must be a boolean!"});
    }

    for (const auto& stmt : node->whileData.stmts) {
        semantic->resolveTypes(stmt);
    }
}

void resolveRun(Semantic *semantic, Node *node) {
    semantic->resolveTypes(node->nodeData);

    node->typeInfo = node->nodeData->typeInfo;

    assert(node->nodeData->type == NodeType::FN_CALL);
    auto ctFn = node->nodeData->fnCallData.fn;

    auto ctSemantic = make_unique<Semantic>();
    ctSemantic->resolveTypes(ctFn);

    auto gen = make_unique<BytecodeGen>();
    gen->isMainFn = true;
    gen->sourceMap.sourceInfo = node->region.srcInfo;
    gen->processFnDecls = true;

    gen->gen(ctFn);
    while (!gen->toProcess.empty()) {
        gen->isMainFn = false;
        gen->processFnDecls = true;
        gen->gen(gen->toProcess.front());
        gen->toProcess.pop();
    }
    gen->fixup();

    auto interp = make_unique<Interpreter>();
    interp->instructions = gen->instructions;
    interp->fnTable = gen->fnTable;
    interp->sourceMap = gen->sourceMap;
    auto instructions = gen->instructions;
    auto fnTable = gen->fnTable;

    interp->continuing = true;
    interp->instructions = instructions;
    interp->fnTable = fnTable;
    interp->interpret();

    switch (node->typeInfo->typeData.kind) {
        case NodeTypekind::I32:
        case NodeTypekind::I64: {
            auto resolved = new Node(node->region.srcInfo, nullptr, NodeType::INT_LITERAL, node->scope);
            resolved->typeInfo = node->typeInfo;
            resolved->intLiteralData.value = interp->readFromStack<int32_t>(0);
            node->resolved = resolved;
        } break;
        default: {
            assert(false && "unsupported typekind for CTE");
        } break;
    }
}

void resolveTypeof(Semantic *semantic, Node *node) {
    semantic->resolveTypes(node->nodeData);

    node->typeInfo = new Node(NodeTypekind::EXPOSED_TYPE);

    node->staticValue = node->nodeData->typeInfo;
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
        case NodeType::VALUE_PARAM: {
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
        case NodeType::PANIC: {
            resolvePanic(this, node);
        } break;
        case NodeType::STRUCT_LITERAL: {
            resolveStructLiteral(this, node);
        } break;
        case NodeType::IF: {
            resolveIf(this, node);
        } break;
        case NodeType::RUN: {
            resolveRun(this, node);
        } break;
        case NodeType::TYPEOF: {
            resolveTypeof(this, node);
        } break;
        case NodeType::BOOLEAN_LITERAL: {
            resolveBooleanLiteral(this, node);
        } break;
        case NodeType::ARRAY_INDEX: {
            resolveArrayIndex(this, node);
        } break;
        case NodeType::CAST: {
            resolveCast(this, node);
        } break;
        case NodeType::STRING_LITERAL: {
            resolveStringLiteral(this, node);
        } break;
        case NodeType::NIL_LITERAL: {
            resolveNilLiteral(this, node);
        } break;
        case NodeType::WHILE: {
            resolveWhile(this, node);
        } break;
        default: assert(false);
    }
}

void Semantic::addLocal(Node *local) {
    if (local->isLocal) { return; }
    local->isLocal = true;

    if (currentFnDecl) {
        currentFnDecl->locals.push_back(local);
    }
}
