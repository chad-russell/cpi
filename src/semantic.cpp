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
            if (resolved->typeData.structTypeData.coercedType != nullptr) {
                return typeSize(resolved->typeData.structTypeData.coercedType);
            }

            auto total = 0;
            auto largest = 0;

            // todo(chad): this should be configurable based on the width specified by the user (if/when we allow that)
            auto tagSizeInBytes = 4;

            for (auto param : resolved->typeData.structTypeData.params) {
                // if it's a union and we're not assigning to the 'tag' part, then the offset is the size of the tag
                if (resolved->typeData.structTypeData.isSecretlyUnion && param->declParamData.index > 0) {
                    param->localOffset = tagSizeInBytes;
                }
                else {
                    param->localOffset = total;
                }
                assert(param->type == NodeType::DECL_PARAM);

                auto size = typeSize(param->declParamData.type);
                total += size;

                if (size > largest) {
                    largest = size;
                }
            }

            if (resolved->typeData.structTypeData.isSecretlyUnion) {
                return tagSizeInBytes + largest;
            }

            return total;
        }
        case NodeTypekind::EXPOSED_AST: {
            return typeSize(resolved->staticValue);
        }
        default:
            assert(false);
    }
}

int maybeMatchUnionToStructLiteral(Node *desired, Node *actual) {
    if ((actual->typeData.structTypeData.isSecretlyUnion && !desired->typeData.structTypeData.isSecretlyUnion)
        || (desired->typeData.structTypeData.isSecretlyUnion && !actual->typeData.structTypeData.isSecretlyUnion)) {
        // if the other one is a struct literal, then we just need to match that there is only one param,
        // and that it matches something in the other one

        Node *unionToMatchAgainst = desired;
        Node *other = actual;
        if (actual->typeData.structTypeData.isSecretlyUnion) {
            unionToMatchAgainst = actual;
            other = desired;
        }

        if (other->typeData.structTypeData.params.size() != 1) {
            return 0;
        }

        assert(other->typeData.structTypeData.params[0]->type == NodeType::DECL_PARAM);

        // if it doesn't have a name it obviously can't be a match
        if (other->typeData.structTypeData.params[0]->declParamData.name == nullptr) {
            return 0;
        }

        assert(other->typeData.structTypeData.params[0]->declParamData.name->type == NodeType::SYMBOL);
        auto otherAtomId = other->typeData.structTypeData.params[0]->declParamData.name->symbolData.atomId;

        auto paramIndex = 0;
        for (auto p : unionToMatchAgainst->typeData.structTypeData.params) {
            assert(p->type == NodeType::DECL_PARAM);
            assert(p->declParamData.name->type == NodeType::SYMBOL);
            auto pAtomId = p->declParamData.name->symbolData.atomId;

            if (pAtomId == otherAtomId) {
                other->typeData.structTypeData.coercedType = unionToMatchAgainst;
                other->typeData.structTypeData.params[0]->declParamData.index = paramIndex;

                other->typeData.structTypeData.enumCoerced = true;
                return 2;
            }

            paramIndex += 1;
        }

        return 1;
    }
    else {
        return 0;
    }
}

bool typesMatch(Node *desired, Node *actual, Semantic *semantic, Node *actualValue = nullptr) {
    desired = resolve(desired);
    actual = resolve(actual);

    if (desired == actual) {
        return true;
    }

    assert(desired->type == NodeType::TYPE);
    assert(actual->type == NodeType::TYPE);

    // if we're dealing with an EXPOSED_AST with a static value, then simply propagate that
    if (actual->typeData.kind == NodeTypekind::EXPOSED_AST && actual->staticValue != nullptr) {
        actual = actual->staticValue;
    }
    if (desired->typeData.kind == NodeTypekind::EXPOSED_AST && desired->staticValue != nullptr) {
        desired = desired->staticValue;
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

    if (actual->typeData.kind == NodeTypekind::NONE && desired->typeData.kind == NodeTypekind::NONE) {
        return true;
    }

    if (actual->typeData.kind == NodeTypekind::NONE) {
        if (desired->typeData.kind == NodeTypekind::STRUCT && desired->typeData.structTypeData.params.empty()) {
            return true;
        }
    }

    if (desired->typeData.kind == NodeTypekind::NONE) {
        if (actual->typeData.kind == NodeTypekind::STRUCT && actual->typeData.structTypeData.params.empty()) {
            return true;
        }
    }

    if (actual->typeData.kind == NodeTypekind::STRUCT) {
        if (desired->typeData.kind != NodeTypekind::STRUCT) {
            return false;
        }

        if (actual->typeData.structTypeData.isSecretlyArray || desired->typeData.structTypeData.isSecretlyArray) {
            return desired->typeData.structTypeData.isSecretlyArray
                   && actual->typeData.structTypeData.isSecretlyArray
                   && typesMatch(desired->typeData.structTypeData.secretArrayElementType,
                                 actual->typeData.structTypeData.secretArrayElementType,
                                 semantic);
        }

        auto matchedUnion = maybeMatchUnionToStructLiteral(desired, actual);
        if (matchedUnion == 1) {
            return false;
        }
        else if (matchedUnion == 2) {
            return true;
        }

        if (desired->typeData.structTypeData.params.size() != actual->typeData.structTypeData.params.size()) {
            return false;
        }

        auto encounteredError = assignParams(semantic, actual, desired->typeData.structTypeData.params, actual->typeData.structTypeData.params);

        if (!encounteredError) {
            if (!actual->typeData.structTypeData.isLiteral && desired->typeData.structTypeData.isLiteral) {
                desired->typeData.structTypeData.coercedType = actual;
            }
            else if (actual->typeData.structTypeData.isLiteral && !desired->typeData.structTypeData.isLiteral) {
                actual->typeData.structTypeData.coercedType = desired;
            }
        }

        for (auto i = 0; i < desired->typeData.structTypeData.params.size(); i++) {
            auto desiredParamI = desired->typeData.structTypeData.params[i];
            auto actualParamI = actual->typeData.structTypeData.params[i];

            Node *newT1;
            Node *newT2;

            if (desiredParamI->type == NodeType::DECL_PARAM) {
                newT1 = resolve(desiredParamI->declParamData.type);
            } else {
                assert(desiredParamI->type == NodeType::VALUE_PARAM);
                newT1 = resolve(desiredParamI->typeInfo);
            }

            if (actualParamI->type == NodeType::DECL_PARAM) {
                newT2 = resolve(actualParamI->declParamData.type);
            } else {
                assert(actualParamI->type == NodeType::VALUE_PARAM);
                newT2 = resolve(actualParamI->typeInfo);
            }

            while (newT1->typeData.kind == NodeTypekind::POINTER) {
                newT1 = resolve(newT1->typeData.pointerTypeData.underlyingType);
            }
            while (newT2->typeData.kind == NodeTypekind::POINTER) {
                newT2 = resolve(newT2->typeData.pointerTypeData.underlyingType);
            }

            // if we get to a situation where two types match iff they match, then they match
            auto tautology = (newT1 == desired && newT2 == actual) || (newT1 == actual && newT2 == desired);

            if (!tautology && !typesMatch(newT1, newT2, semantic)) {
                return false;
            }
        }

        return !encounteredError;
    }

    if (actual->typeData.kind == NodeTypekind::POINTER) {
        if (desired->typeData.kind == NodeTypekind::POINTER) {
            return typesMatch(actual->typeData.pointerTypeData.underlyingType, desired->typeData.pointerTypeData.underlyingType, semantic);
        }

        else { return false; }
    }

    return desired->typeData.kind == actual->typeData.kind;
}

Node *defaultValueFor(Semantic *semantic, Node *type) {
    assert(type->type == NodeType::TYPE);

    switch (type->typeData.kind) {
        case NodeTypekind::INT_LITERAL:
        case NodeTypekind::I8:
        case NodeTypekind::I32:
        case NodeTypekind::I64: {
            auto def = new Node();
            def->type = NodeType::INT_LITERAL;
            def->intLiteralData.value = 0;
            def->typeInfo = type;
            return def;
        }
        case NodeTypekind::FLOAT_LITERAL:
        case NodeTypekind::F32:
        case NodeTypekind::F64: {
            auto def = new Node();
            def->type = NodeType::FLOAT_LITERAL;
            def->floatLiteralData.value = 0;
            def->typeInfo = type;
            return def;
        }
        case NodeTypekind::BOOLEAN: {
            auto def = new Node();
            def->type = NodeType::BOOLEAN_LITERAL;
            def->boolLiteralData.value = false;
            def->typeInfo = type;
            return def;
        }
        case NodeTypekind::FN:
        case NodeTypekind::POINTER: {
            auto def = new Node();
            def->type = NodeType::NIL_LITERAL;
            def->typeInfo = type;
            return def;
        }
        case NodeTypekind::STRUCT: {
            auto def = new Node();
            def->type = NodeType::STRUCT_LITERAL;

            for (auto param : type->typeData.structTypeData.params) {
                auto vp = param->declParamData.initialValue;
                if (vp == nullptr) {
                    vp = wrapInValueParam(defaultValueFor(semantic, param->typeInfo), param->declParamData.name->symbolData.atomId);
                }
                else {
                    vp = wrapInValueParam(vp, param->declParamData.name->symbolData.atomId);
                }

                semantic->resolveTypes(vp);
                def->structLiteralData.params.push_back(vp);
            }

            def->typeInfo = type;

            return def;
        }
        case NodeTypekind::SYMBOL: {
            return defaultValueFor(semantic, resolve(type));
        }
        case NodeTypekind::NONE: {
            auto def = new Node();
            def->type = NodeType::STRUCT_LITERAL;
            def->typeInfo = type;
            return def;
        }
        default:
            assert(false);
    }
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
             if (!typesMatch(ret->typeInfo, data->returnType, semantic)) {
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
    node->typeInfo->typeData.floatTypeData = node->floatLiteralData.value;
}

void resolveIntLiteral(Semantic *semantic, Node *node) {
    if (node->typeInfo == nullptr) {
        node->typeInfo = new Node(NodeTypekind::INT_LITERAL);
    }

    node->typeInfo->typeData.intTypeData = node->intLiteralData.value;
}

void resolveBooleanLiteral(Semantic *semantic, Node *node) {
    node->typeInfo = new Node(NodeTypekind::BOOLEAN);
    node->typeInfo->typeData.boolTypeData = node->boolLiteralData.value;
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

    auto resolvedTargetTypeInfo = resolve(node->arrayIndexData.target->typeInfo);
    if (resolvedTargetTypeInfo->typeData.kind != NodeTypekind::STRUCT) {
        semantic->reportError({node}, Error{node->region, "cannot index a non-array"});
        return;
    }
    if (!resolvedTargetTypeInfo->typeData.structTypeData.isSecretlyArray) {
        semantic->reportError({node}, Error{node->region, "cannot index a non-array"});
        return;
    }

    node->typeInfo = resolvedTargetTypeInfo->typeData.structTypeData.secretArrayElementType;

    // a[i] <==> ^((cast(*i32) a) + i), for example (if typeof(a) is []i32)

    auto aCasted = new Node();
    aCasted->type = NodeType::CAST;
    aCasted->castData.isCastFromArrayToDataPtr = true;
    aCasted->castData.type = new Node(NodeTypekind::POINTER);
    aCasted->castData.type->typeData.pointerTypeData.underlyingType = node->typeInfo;
    aCasted->castData.value = node->arrayIndexData.target;

    auto add = new Node(node->region.srcInfo, NodeType::BINOP, node->scope);
    add->binopData.type = LexerTokenType::ADD;
    add->binopData.lhs = aCasted;
    add->binopData.rhs = node->arrayIndexData.indexValue;
    add->binopData.rhsScale = typeSize(node->typeInfo);

    auto deref = new Node(node->region.srcInfo, NodeType::DEREF, node->scope);
    deref->derefData.target = add;

    semantic->addLocal(deref->derefData.target);

    node->resolved = deref;

    semantic->resolveTypes(node->resolved);

    node->typeInfo = node->resolved->typeInfo;
    assert(node->typeInfo != nullptr);
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
    auto arrayLiteral = new Node(node->region.srcInfo, NodeType::STRUCT_LITERAL, node->scope);
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

    // heapifiedCharArrayLiteral = &{'h', 'e', 'l', 'l', 'o'}
    auto heapifiedCharArrayLiteral = new Node();
    heapifiedCharArrayLiteral->type = NodeType::HEAPIFY;
    heapifiedCharArrayLiteral->nodeData = charArrayLiteral;

    semantic->addLocal(heapifiedCharArrayLiteral->nodeData);

    // countNode = 5
    auto countNode = new Node();
    countNode->type = NodeType::INT_LITERAL;
    countNode->typeInfo = new Node(NodeTypekind::I32);
    countNode->intLiteralData.value = static_cast<int64_t>(node->stringLiteralData.value.size());

    // arrayLiteral = {&{'h', 'e', 'l', 'l', 'o'}, 5}
    arrayLiteral->structLiteralData.params.push_back(wrapInValueParam(heapifiedCharArrayLiteral, "data"));
    arrayLiteral->structLiteralData.params.push_back(wrapInValueParam(countNode, "count"));

    semantic->resolveTypes(arrayLiteral);

    assert(arrayLiteral->typeInfo->typeData.kind == NodeTypekind::STRUCT);
    arrayLiteral->typeInfo->typeData.structTypeData.isSecretlyArray = true;

    arrayLiteral->typeInfo->typeData.structTypeData.secretArrayElementType = new Node(NodeTypekind::I8);

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

                if (declParam->type == NodeType::DECL_PARAM) {
                    passedParam->typeInfo = declParam->declParamData.type;

                    if (passedParam->type == NodeType::VALUE_PARAM) {
                        passedParam->valueParamData.name = declParam->declParamData.name;
                    }
                    else if (passedParam->type == NodeType::DECL_PARAM) {
                        passedParam->declParamData.name = declParam->declParamData.name;
                    }
                } else if (declParam->type == NodeType::VALUE_PARAM) {
                    passedParam->typeInfo = declParam->valueParamData.value->typeInfo;

                    if (passedParam->type == NodeType::VALUE_PARAM) {
                        passedParam->valueParamData.name = declParam->valueParamData.name;
                    }
                    else if (passedParam->type == NodeType::DECL_PARAM) {
                        passedParam->declParamData.name = declParam->valueParamData.name;
                    }
                } else {
                    assert(false);
                }

                newParams[j] = passedParam;
                openParams[j] = false;
            }
        }
    }

    for (unsigned long i = 0; i < newParams.size(); i++) {
        auto declParam = declParams[i];

        // if the decl parameter is unassigned but there is a default value, then fill in that param with the default
        bool openParamsI = openParams[i];
        if (declParam->declParamData.initialValue != nullptr && openParamsI) {
            openParams[i] = false;
            declParam->declParamData.initialValue->typeInfo = declParam->declParamData.type;
            newParams[i] = wrapInValueParam(declParam->declParamData.initialValue, declParam->declParamData.name);

            semantic->resolveTypes(newParams[i]);
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
    if (shouldCheckTypeMatch && !typesMatch(node->declData.type, node->declData.initialValue->typeInfo, semantic, node->declData.initialValue)) {
        ostringstream s("");
        s << "Type mismatch! wanted " << node->declData.type->typeData
          << ", got " << node->declData.initialValue->typeInfo->typeData;
        semantic->reportError({node, node->declData.initialValue, node->declData.type},
                              Error{node->region, s.str()});
    }
    else if (node->declData.type == nullptr) {
        node->declData.type = node->declData.initialValue->typeInfo;
    }
    else if (node->declData.initialValue == nullptr) {
        node->declData.initialValue = defaultValueFor(semantic, node->declData.type);
    }

    auto resolvedDeclDataType = resolve(node->declData.type);
    node->typeInfo = node->declData.type;

    if (node->declData.initialValue == nullptr) {
        return;
    }

    auto matchedUnion = maybeMatchUnionToStructLiteral(resolvedDeclDataType, node->declData.initialValue->typeInfo);
    if (matchedUnion == 1) {
        semantic->reportError({}, Error{node->region, "error assigning struct literal to union - unmatched field name"});
    }
}

void resolveType(Semantic *semantic, Node *node) {
    switch (node->typeData.kind) {
        case NodeTypekind::NONE:
        case NodeTypekind::BOOLEAN:
        case NodeTypekind::EXPOSED_AST:
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

    node->typeInfo = new Node(NodeTypekind::EXPOSED_AST);
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
        node->binopData.rhsScale = typeSize(node->binopData.lhs->typeInfo->typeData.pointerTypeData.underlyingType);
        return;
    }

    if (node->binopData.rhs->typeInfo->typeData.kind == NodeTypekind::POINTER
        && (node->binopData.lhs->typeInfo->typeData.kind == NodeTypekind::INT_LITERAL
            || node->binopData.lhs->typeInfo->typeData.kind == NodeTypekind::I32
            || node->binopData.lhs->typeInfo->typeData.kind == NodeTypekind::I64)) {
        node->typeInfo = node->binopData.rhs->typeInfo;
        node->binopData.rhsScale = typeSize(node->binopData.rhs->typeInfo->typeData.pointerTypeData.underlyingType);
        return;
    }

    if (!typesMatch(node->binopData.lhs->typeInfo, node->binopData.rhs->typeInfo, semantic)) {
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

        auto encounteredError = assignParams(semantic, node, declParams, node->fnCallData.params);
        assert(!encounteredError);

        if (isPoly) {
            // 'link' each decl param to its runtime param
            for (auto p = 0; p < declParams.size(); p++) {
                declParams[p]->declParamData.polyLink = node->fnCallData.params[p];
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
                ctDeclParams[i]->staticValue = resolve(ctValue);
            } else {
                ctDeclParams[i]->staticValue = resolve(ctParam);
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
            if (declParam->typeInfo->typeData.kind == NodeTypekind::EXPOSED_AST) {
                givenParam->typeInfo = new Node(NodeTypekind::EXPOSED_AST);
                givenParam->typeInfo->staticValue = givenParam;
            }
            else {
                if (!typesMatch(declParam->typeInfo, givenParam->valueParamData.value->typeInfo, semantic)) {
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
            if (!typesMatch(declParam->typeInfo, givenParam->typeInfo, semantic)) {
                semantic->reportError({node, declParam, givenParam}, Error{node->region, "type mismatch!"});
            }
        }

        node->typeInfo = resolvedFnType.fnTypeData.returnType;
    } else {
        node->resolved = resolvedFn;
        node->typeInfo = resolvedFn->typeInfo;
    }
}

void possiblyResolveAssignToUnion(Semantic *semantic, Node *originalAssignment, Node *node) {
    // if we are assigning to a union (or pointer to a union), add a 'pre-check' to set the tag

    // it must be a dot
    auto assigningToUnion = node->type == NodeType::DOT;

    // it also must be a dot on a struct or a pointer to a struct
    if (assigningToUnion) {
        auto typeData = resolve(node->dotData.lhs->typeInfo)->typeData;
        while (typeData.kind == NodeTypekind::POINTER) {
            typeData = typeData.pointerTypeData.underlyingType->typeData;
        }
        assigningToUnion = typeData.kind == NodeTypekind::STRUCT && typeData.structTypeData.isSecretlyUnion;
    }

    // don't set the tag when assigning to the tag parameter
    if (assigningToUnion) {
        assigningToUnion = node->dotData.rhs->symbolData.atomId != AtomTable::current->insertStr("tag");
    }

    if (assigningToUnion) {
        auto mostLhs = node->dotData.lhs;
        auto secondmostLhs = node;

        // get the type data
        auto typeData = resolve(node->dotData.lhs->typeInfo)->typeData;
        while (typeData.kind == NodeTypekind::POINTER) {
            typeData = typeData.pointerTypeData.underlyingType->typeData;
        }

        // the resolved parameter is the 0th parameter of the type data
        auto param0 = new Node();
        param0->scope = node->dotData.lhs->scope;
        param0->type = NodeType::SYMBOL;
        param0->symbolData.atomId = AtomTable::current->insertStr("tag");
        param0->resolved = typeData.structTypeData.params[0];
        assert(typeData.structTypeData.params[0]->type == NodeType::DECL_PARAM);
        param0->typeInfo = typeData.structTypeData.params[0]->declParamData.type;

        auto tagDot = new Node();
        tagDot->scope = originalAssignment->scope;
        tagDot->type = NodeType::DOT;
        tagDot->dotData.lhs = mostLhs;
        tagDot->dotData.rhs = param0;
        semantic->resolveTypes(tagDot);

        uint64_t paramIndex;
        auto foundParam = secondmostLhs->dotData.resolved;
        if (foundParam->type == NodeType::DECL_PARAM) {
            paramIndex = (uint64_t) foundParam->declParamData.index;
        } else if (foundParam->type == NodeType::VALUE_PARAM) {
            paramIndex = (uint64_t) foundParam->valueParamData.index;
        } else {
            assert(false);
        }

        // assign to 0th parameter the value of paramIndex
        auto constParamIndex = new Node();
        constParamIndex->scope = originalAssignment->scope;
        constParamIndex->type = NodeType::INT_LITERAL;
        constParamIndex->intLiteralData.value = paramIndex;

        // we add a local here because when storing something to a dot, a local is always expected.
        // todo(chad): optimizations for storing constant values to dots
        semantic->addLocal(constParamIndex);

        auto secretAss = new Node();
        secretAss->scope = originalAssignment->scope;
        secretAss->type = NodeType::ASSIGN;
        secretAss->assignData.lhs = tagDot;
        secretAss->assignData.rhs = constParamIndex;

        originalAssignment->postStmts.push_back(secretAss);

        semantic->resolveTypes(secretAss);
    }
    else if (node->type == NodeType::DOT && node->dotData.lhs->type == NodeType::DOT) {
        possiblyResolveAssignToUnion(semantic, originalAssignment, node->dotData.lhs);
    }
}

void resolveAssign(Semantic *semantic, Node *node) {
    semantic->lvalueAssignmentContext = true;
    semantic->resolveTypes(node->assignData.lhs);
    semantic->lvalueAssignmentContext = false;

    semantic->resolveTypes(node->assignData.rhs);

    if (!typesMatch(node->assignData.lhs->typeInfo, node->assignData.rhs->typeInfo, semantic)) {
        semantic->reportError({node, node->assignData.lhs, node->assignData.rhs},
                              Error{node->region, "assignment type mismatch"});
    }

    semantic->lvalueAssignmentContext = true;
    possiblyResolveAssignToUnion(semantic, node, node->assignData.lhs);
    semantic->lvalueAssignmentContext = false;

    auto resolvedDeclDataType = resolve(node->assignData.lhs->typeInfo);

    auto matchedUnion = maybeMatchUnionToStructLiteral(resolvedDeclDataType, node->assignData.rhs->typeInfo);
    if (matchedUnion == 1) {
        semantic->reportError({}, Error{node->region, "error assigning struct literal to union - unmatched field name"});
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
        if (!typesMatch(node->declParamData.type, node->declParamData.initialValue->typeInfo, semantic)) {
            semantic->reportError({node}, Error{node->region, "Type mismatch decl param"});
        }
    }
}

void resolveValueParam(Semantic *semantic, Node *node) {
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

    auto resolved = resolve(node->nodeData);
    if (resolved->type == NodeType::DECL_PARAM) {
        semantic->reportError({node}, Error{node->region, "Cannot take the address of a fn parameter, they are readonly"});
    }

    if (node->nodeData->type == NodeType::ARRAY_INDEX) {
        node->resolved = resolve(node->nodeData)->derefData.target;
    }
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
        auto debug2 = AtomTable::current->backwardAtoms[node->dotData.rhs->symbolData.atomId];

        for (auto param : structData.params) {
            if (param->declParamData.name != nullptr) {
                auto debug1 = AtomTable::current->backwardAtoms[param->declParamData.name->symbolData.atomId];

                if (param->declParamData.name->symbolData.atomId == node->dotData.rhs->symbolData.atomId) {
                    foundParam = param;
                    break;
                }
            }
        }
    } else {
        for (auto param : structData.params) {
            auto f1 = AtomTable::current->backwardAtoms[param->declParamData.name->symbolData.atomId];
            auto f2 = AtomTable::current->backwardAtoms[node->dotData.rhs->symbolData.atomId];

            if (param->declParamData.name->symbolData.atomId == node->dotData.rhs->symbolData.atomId) {
                foundParam = param;
                break;
            }
        }
    }

    node->dotData.resolved = foundParam;

    return foundParam;
}

void createTagCheck(Semantic *semantic, Node *node, Node *lhs, Node *rhs) {
    if (node->tagCheck) { return; }
    node->tagCheck = true;

    // get the type data
    auto typeData = resolve(node->dotData.lhs->typeInfo)->typeData;
    while (typeData.kind == NodeTypekind::POINTER) {
        typeData = resolve(typeData.pointerTypeData.underlyingType)->typeData;
    }

    auto foundParam = findParam(semantic, node);
    auto paramIndex = foundParam->declParamData.index;

    // don't do a check if we're accessing the tag itself
    if (paramIndex == 0) { return; }

    auto constTag = new Node();
    constTag->scope = node->dotData.lhs->scope;
    constTag->type = NodeType::INT_LITERAL;
    constTag->intLiteralData.value = paramIndex;
    semantic->resolveTypes(constTag);

    // the resolved parameter is the 0th parameter of the type data
    auto param0 = new Node();
    param0->scope = node->dotData.lhs->scope;
    param0->type = NodeType::SYMBOL;
    param0->symbolData.atomId = AtomTable::current->insertStr("tag");
    param0->resolved = typeData.structTypeData.params[0];
    assert(typeData.structTypeData.params[0]->type == NodeType::DECL_PARAM);
    param0->typeInfo = typeData.structTypeData.params[0]->declParamData.type;

    auto tagDot = new Node();
    tagDot->scope = node->scope;
    tagDot->type = NodeType::DOT;
    tagDot->dotData.lhs = lhs;
    tagDot->dotData.rhs = param0;
    semantic->resolveTypes(tagDot);

    auto neq = new Node();
    neq->scope = node->scope;
    neq->type = NodeType::BINOP;
    neq->binopData.type = LexerTokenType::NE;
    neq->binopData.lhs = tagDot;
    neq->binopData.rhs = constTag;
    semantic->addLocal(neq);
    semantic->resolveTypes(neq);

    auto panicStmt = new Node();
    panicStmt->scope = node->scope;
    panicStmt->type = NodeType::PANIC;

    // if
    auto ifCheck = new Node();
    ifCheck->scope = node->scope;
    ifCheck->type = NodeType::IF;
    ifCheck->ifData.condition = neq;
    ifCheck->ifData.stmts.push_back(panicStmt);

    node->preStmts.push_back(ifCheck);
}

void resolveDot(Semantic *semantic, Node *node, Node *lhs, Node *rhs) {
    semantic->addLocal(node->dotData.lhs);

    auto resolvedLhs = resolve(lhs);
    semantic->resolveTypes(resolvedLhs);

    // follow the lhs all the way through pointers, symbols, etc. and assert it's a struct
    if (!semantic->lvalueAssignmentContext) {
        auto resolvedLhsTypeInfo = resolve(resolvedLhs->typeInfo);
        while (resolvedLhsTypeInfo->typeData.kind == NodeTypekind::POINTER) {
            resolvedLhsTypeInfo = resolve(resolvedLhsTypeInfo->typeData.pointerTypeData.underlyingType);
        }
        assert(resolvedLhsTypeInfo->typeData.kind == NodeTypekind::STRUCT);

        auto isUnionAccess = resolvedLhsTypeInfo->typeData.structTypeData.isSecretlyUnion;

        if (isUnionAccess) {
            createTagCheck(semantic, node, lhs, rhs);
        }
    }

    if (resolvedLhs->type == NodeType::DOT) {
        resolveDot(semantic, resolvedLhs, resolvedLhs->dotData.lhs, resolvedLhs->dotData.rhs);
        resolveDot(semantic, node, resolvedLhs->dotData.resolved, rhs);

        node->dotData.resolved = rhs->resolved;
        node->typeInfo = rhs->typeInfo;
    }
    else {
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
    semantic->resolveTypes(node->dotData.lhs);
    if (resolve(node->dotData.lhs->typeInfo)->typeData.kind == NodeTypekind::POINTER) {
        auto local = new Node(NodeTypekind::POINTER);
        local->typeInfo = local;
        local->isLocal = true;
        semantic->currentFnDecl->locals.push_back(local);

        node->dotData.autoDerefStorage = local;
    }
}

void resolveDot(Semantic *semantic, Node *node) {
    auto resolvedLhs = resolve(node->dotData.lhs);
    resolveDot(semantic, node, resolvedLhs, node->dotData.rhs);
}

void resolvePanic(Semantic *semantic, Node *node) {
    // nothing to do here, we're just gonna panic anyway...
}

void resolveStructLiteral(Semantic *semantic, Node *node) {
    node->typeInfo = new Node(NodeTypekind::STRUCT);
    node->typeInfo->typeData.structTypeData.isLiteral = true;

    auto paramIndex = 0;
    for (auto param : node->structLiteralData.params) {
        semantic->resolveTypes(param);

        auto wrapped = wrapInDeclParam(param->typeInfo, param->valueParamData.name, paramIndex);
        node->typeInfo->typeData.structTypeData.params.push_back(wrapped);

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

void resolveUnaryNeg(Semantic *semantic, Node *node) {
    semantic->resolveTypes(node->nodeData);
    node->typeInfo = node->nodeData->typeInfo;
}

void resolveMalloc(Semantic *semantic, Node *node) {
    semantic->resolveTypes(node->nodeData);
    if (node->nodeData->typeInfo->typeData.kind != NodeTypekind::I64) {
        semantic->reportError({node, node->nodeData}, Error{node->nodeData->region, "Expected i64"});
    }

    node->typeInfo = new Node(NodeTypekind::POINTER);
    node->typeInfo->typeData.pointerTypeData.underlyingType = new Node(NodeTypekind::NONE);
}

void resolveFree(Semantic *semantic, Node *node) {
    semantic->resolveTypes(node->nodeData);
    if (node->nodeData->typeInfo->typeData.kind != NodeTypekind::POINTER) {
        semantic->reportError({node, node->nodeData}, Error{node->nodeData->region, "Expected a pointer"});
    }
}

void resolveArrayLiteral(Semantic *semantic, Node *node) {
    for (auto elem : node->arrayLiteralData.elements) {
        semantic->resolveTypes(elem);
    }

    if (node->arrayLiteralData.elementType == nullptr) {
        if (node->arrayLiteralData.elements.empty()) {
            semantic->reportError({node}, Error{
                    node->region,
                    "No type given for array literal and no elements from which to infer it"
            });
            return;
        }
        node->arrayLiteralData.elementType = node->arrayLiteralData.elements[0]->typeInfo;
    }

    semantic->resolveTypes(node->arrayLiteralData.elementType);

    for (auto elem : node->arrayLiteralData.elements) {
        if (!typesMatch(node->arrayLiteralData.elementType, elem->typeInfo, semantic)) {
            semantic->reportError({node}, Error{
                    node->region,
                    "Type mismatch!"
            });
            return;
        }
    }

    node->resolved = node->arrayLiteralData.structLiteralRepresentation;

    semantic->resolveTypes(node->resolved);
    node->resolved->typeInfo->typeData.structTypeData.isSecretlyArray = true;
    node->resolved->typeInfo->typeData.structTypeData.secretArrayElementType = node->arrayLiteralData.elementType;

    node->typeInfo = node->resolved->typeInfo;
}

void resolveFor(Semantic *semantic, Node *node) {
    semantic->resolveTypes(node->forData.target);

    if (!(node->forData.target->typeInfo->typeData.kind == NodeTypekind::STRUCT
          && node->forData.target->typeInfo->typeData.structTypeData.isSecretlyArray)) {
        semantic->reportError({node, node->forData.target}, Error{
                node->forData.target->region,
                "'for' target must be an array"
        });
    }

    // create a new declaration with the alias as the lvalue, and the array type as the type
    auto elementDecl = new Node();
    elementDecl->type = NodeType::DECL;
    elementDecl->scope = node->scope;
    elementDecl->region = node->forData.element_alias->region;
    elementDecl->declData.type = node->forData.target->typeInfo->typeData.structTypeData.secretArrayElementType;
    elementDecl->declData.lvalue = node->forData.element_alias;
    elementDecl->isLocal = true;
    semantic->currentFnDecl->locals.push_back(elementDecl);
    semantic->resolveTypes(elementDecl);

    // insert this new declaration into the scope of the 'for' statement
    node->scope->symbols.insert({node->forData.element_alias->symbolData.atomId, elementDecl});

    // 0
    auto zero = new Node();
    zero->type = NodeType::INT_LITERAL;
    zero->typeInfo = new Node(NodeTypekind::I32);
    zero->intLiteralData.value = 0;

    // indexDecl: i32 = 0
    auto indexDecl = new Node();
    indexDecl->type = NodeType::DECL;
    indexDecl->scope = node->scope;
    if (node->forData.iterator_alias != nullptr) {
        indexDecl->region = node->forData.iterator_alias->region;
    }
    indexDecl->declData.type = new Node(NodeTypekind::I32);
    indexDecl->declData.lvalue = node->forData.iterator_alias;
    indexDecl->declData.initialValue = zero;
    indexDecl->isLocal = true;
    semantic->currentFnDecl->locals.push_back(indexDecl);
    semantic->resolveTypes(indexDecl);

    auto index = indexDecl;
    if (node->forData.iterator_alias != nullptr) {
        node->forData.iterator_alias->resolved = indexDecl;
        node->scope->symbols.insert({node->forData.iterator_alias->symbolData.atomId, indexDecl});

        index = node->forData.iterator_alias;
    }

    // arrayDecl
    auto arrayDecl = new Node(node->region.srcInfo, NodeType::DECL, node->scope);
    arrayDecl->declData.type = node->forData.target->typeInfo;
    arrayDecl->declData.initialValue = node->forData.target;
    arrayDecl->isLocal = true;
    semantic->currentFnDecl->locals.push_back(arrayDecl);

    for (auto& stmt : node->forData.stmts) {
        semantic->resolveTypes(stmt);
    }

    auto countSymbol = new Node();
    countSymbol->scope = node->forData.target->scope;
    countSymbol->type = NodeType::SYMBOL;
    countSymbol->symbolData.atomId = AtomTable::current->insertStr("count");

    auto arrayDotCount = new Node();
    arrayDotCount->scope = node->scope;
    arrayDotCount->type = NodeType::DOT;
    arrayDotCount->dotData.lhs = arrayDecl;
    arrayDotCount->dotData.rhs = countSymbol;

    auto whileConditionBinop = new Node();
    whileConditionBinop->type = NodeType::BINOP;
    whileConditionBinop->binopData.type = LexerTokenType::LT;
    whileConditionBinop->binopData.lhs = index;
    whileConditionBinop->binopData.rhs = arrayDotCount;
    semantic->currentFnDecl->locals.push_back(whileConditionBinop);
    whileConditionBinop->isLocal = true;

    // one
    auto one = new Node();
    one->type = NodeType::INT_LITERAL;
    one->typeInfo = new Node(NodeTypekind::I32);
    one->intLiteralData.value = 1;

    // indexDecl + 1
    auto incrementIndexBinop = new Node(node->region.srcInfo, NodeType::BINOP, node->scope);
    incrementIndexBinop->type = NodeType::BINOP;
    incrementIndexBinop->binopData.type = LexerTokenType::ADD;
    incrementIndexBinop->binopData.lhs = index;
    incrementIndexBinop->binopData.rhs = one;
    semantic->currentFnDecl->locals.push_back(incrementIndexBinop);
    incrementIndexBinop->isLocal = true;

    auto incr = new Node(node->region.srcInfo, NodeType::ASSIGN, node->scope);
    incr->assignData.lhs = index;
    incr->assignData.rhs = incrementIndexBinop;

    // arrIndex
    auto arrIndex = new Node(node->region.srcInfo, NodeType::ARRAY_INDEX, node->scope);
    arrIndex->arrayIndexData.target = arrayDecl;
    arrIndex->arrayIndexData.indexValue = index;

    // elementAssign
    auto elementAssign = new Node(node->region.srcInfo, NodeType::ASSIGN, node->scope);
    elementAssign->assignData.lhs = elementDecl;
    elementAssign->assignData.rhs = arrIndex;

    // while binop(lessThan, indexDecl, dot(array, 'count'))
    // all stmts
    // indexDecl = indexDecl + 1
    auto while_ = new Node(node->region.srcInfo, NodeType::WHILE, node->scope);
    while_->whileData.condition = whileConditionBinop;

    // e := array[index]
    while_->whileData.stmts.push_back(elementAssign);

    // stmts
    for (auto stmt : node->forData.stmts) {
        while_->whileData.stmts.push_back(stmt);
    }

    // increment
    while_->whileData.stmts.push_back(incr);

    node->forData.rewritten.push_back(elementDecl);
    node->forData.rewritten.push_back(arrayDecl);
    node->forData.rewritten.push_back(index);
    node->forData.rewritten.push_back(while_);

    for (auto stmt : node->forData.rewritten) {
        semantic->resolveTypes(stmt);
    }
}

void resolveTagCheck(Semantic *semantic, Node *node) {
    if (node->nodeData->type != NodeType::DOT) {
        semantic->reportError({node, node->nodeData}, Error{
                node->region,
                "Expected dot for tagcheck"
        });
        return;
    }

    semantic->resolveTypes(node->nodeData);

    auto resolvedType = resolve(node->nodeData->dotData.lhs->typeInfo);
    assert(resolvedType->typeData.kind == NodeTypekind::STRUCT);
    if (!resolvedType->typeData.structTypeData.isSecretlyUnion) {
        semantic->reportError({node, node->nodeData}, Error{
                node->region,
                "Can only do a tagcheck on a union"
        });
    }

    auto resolved = node->nodeData->dotData.resolved;
    assert(resolved->type == NodeType::DECL_PARAM);

    // eqeq
    auto typeData = resolve(node->nodeData->dotData.lhs->typeInfo)->typeData;
    while (typeData.kind == NodeTypekind::POINTER) {
        typeData = typeData.pointerTypeData.underlyingType->typeData;
    }

    // the resolved parameter is the 0th parameter of the type data
    auto param0 = new Node();
    param0->scope = node->nodeData->dotData.lhs->scope;
    param0->type = NodeType::SYMBOL;
    param0->symbolData.atomId = AtomTable::current->insertStr("tag");
    param0->resolved = typeData.structTypeData.params[0];
    assert(typeData.structTypeData.params[0]->type == NodeType::DECL_PARAM);
    param0->typeInfo = typeData.structTypeData.params[0]->declParamData.type;

    auto tagDot = new Node();
    tagDot->scope = node->scope;
    tagDot->type = NodeType::DOT;
    tagDot->dotData.lhs = node->nodeData->dotData.lhs;
    tagDot->dotData.rhs = param0;
    semantic->resolveTypes(tagDot);

    auto paramIndex = resolved->declParamData.index;

    // assign to 0th parameter the value of paramIndex
    auto constParamIndex = new Node();
    constParamIndex->scope = node->scope;
    constParamIndex->type = NodeType::INT_LITERAL;
    constParamIndex->intLiteralData.value = paramIndex;

    auto eqeq = new Node();
    eqeq->scope = node->scope;
    eqeq->type = NodeType::BINOP;
    eqeq->binopData.type = LexerTokenType::EQ_EQ;
    eqeq->binopData.lhs = tagDot;
    eqeq->binopData.rhs = constParamIndex;
    semantic->resolveTypes(eqeq);

    node->resolved = eqeq;
    node->typeInfo = node->resolved->typeInfo;
}

void resolveHeapify(Semantic *semantic, Node *node) {
    semantic->addLocal(node);

    semantic->resolveTypes(node->nodeData);

    auto pointerTypeInfo = new Node(NodeTypekind::POINTER);
    pointerTypeInfo->typeData.pointerTypeData.underlyingType = node->nodeData->typeInfo;
    node->typeInfo = pointerTypeInfo;
}

Node *buildTypeInfoStructLiteral(Semantic *semantic, Node *node) {
//    NONE:          none,
//    INT_LITERAL:   i64,
//    I8:            none,
//    I32:           none,
//    I64:           none,
//    FLOAT_LITERAL: f64,
//    BOOLEAN:       none,
//    F32:           none,
//    F64:           none,
//    FN:            FnData,
//    STRUCT:        StructData,
//    POINTER:       PointerData,
//    ENUM:          EnumData,
//    ARRAY:         ArrayData,

    auto typeKindTag = 0;

    auto noneField = new Node();
    noneField->type = NodeType::STRUCT_LITERAL;
    noneField->typeInfo = new Node(NodeTypekind::NONE);

    Node *valueField = noneField;

    assert(node->type == NodeType::TYPE);
    switch (node->typeData.kind) {
        case NodeTypekind::NONE: typeKindTag = 1; break;
        case NodeTypekind::INT_LITERAL: typeKindTag = 2; break;
        case NodeTypekind::I8: typeKindTag = 3; break;
        case NodeTypekind::I32: typeKindTag = 4; break;
        case NodeTypekind::I64: typeKindTag = 5; break;
        case NodeTypekind::FLOAT_LITERAL: {
            typeKindTag = 6;

            auto lit = new Node();
            lit->type = NodeType::FLOAT_LITERAL;
            lit->floatLiteralData.value = node->typeData.floatTypeData;
            valueField = lit;
        } break;
        case NodeTypekind::BOOLEAN: {
            typeKindTag = 7;

            auto lit = new Node();
            lit->type = NodeType::BOOLEAN_LITERAL;
            lit->boolLiteralData.value = node->typeData.boolTypeData;
            valueField = lit;
        } break;
        case NodeTypekind::F32: typeKindTag = 8; break;
        case NodeTypekind::F64: typeKindTag = 9; break;
        case NodeTypekind::FN: {
            typeKindTag = 10;

            // todo(chad): build function type
            assert(false);
        } break;
        case NodeTypekind::STRUCT: {
            // todo(chad): build struct type(s)
            if (node->typeData.structTypeData.isSecretlyArray) {
                typeKindTag = 14;
            }
            else if (node->typeData.structTypeData.isSecretlyUnion) {
                typeKindTag = 13;
            }
            else {
                typeKindTag = 11;
            }

            assert(false);
        } break;
        case NodeTypekind::SYMBOL: {
            return buildTypeInfoStructLiteral(semantic, resolve(node));
        }
        case NodeTypekind::POINTER: typeKindTag = 12; break;
        default: assert(false);
    }

    // {tag, data}
    auto tagNode = new Node();
    tagNode->type = NodeType::INT_LITERAL;
    tagNode->typeInfo = new Node(NodeTypekind::I32);
    tagNode->intLiteralData.value = typeKindTag;

    assert(valueField != nullptr);
    auto returnStructNode = new Node();
    returnStructNode->type = NodeType::STRUCT_LITERAL;
    returnStructNode->structLiteralData.params.push_back(wrapInValueParam(tagNode, "tag"));
    returnStructNode->structLiteralData.params.push_back(wrapInValueParam(valueField, "data"));

    return returnStructNode;
}

void resolveAnyof(Semantic *semantic, Node *node) {
    auto heapifiedValue = new Node();
    heapifiedValue->type = NodeType::HEAPIFY;
    semantic->resolveTypes(node->nodeData);
    heapifiedValue->nodeData = node->nodeData;
    semantic->addLocal(node->nodeData);

    auto typeInfo = buildTypeInfoStructLiteral(semantic, node->nodeData->typeInfo);
    semantic->resolveTypes(typeInfo);

    node->resolved = new Node();
    node->resolved->type = NodeType::STRUCT_LITERAL;
    node->resolved->structLiteralData.params.push_back(wrapInValueParam(typeInfo, "_type"));
    node->resolved->structLiteralData.params.push_back(wrapInValueParam(heapifiedValue, "value"));

    semantic->resolveTypes(node->resolved);
    node->typeInfo = node->resolved->typeInfo;
}

void resolvePuts(Semantic *semantic, Node *node) {
    semantic->resolveTypes(node->nodeData);
    auto resolvedType = resolve(node->nodeData->typeInfo);
    if (resolvedType->typeData.kind == NodeTypekind::STRUCT 
        && resolvedType->typeData.structTypeData.isSecretlyArray 
        && resolvedType->typeData.structTypeData.secretArrayElementType->typeData.kind == NodeTypekind::I8) {
        return;
    }

    semantic->reportError({node}, Error{node->region, "Expected string for call to puts..."});
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
            auto resolved = new Node(node->region.srcInfo, NodeType::INT_LITERAL, node->scope);
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

    node->typeInfo = new Node(NodeTypekind::EXPOSED_AST);

    node->staticValue = node->nodeData->typeInfo;
}

void resolveSizeof(Semantic *semantic, Node *node) {
    semantic->resolveTypes(node->nodeData);

    node->typeInfo = new Node(NodeTypekind::I64);
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
            resolveValueParam(this, node);
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
        case NodeType::SIZEOF: {
            resolveSizeof(this, node);
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
        case NodeType::UNARY_NEG: {
            resolveUnaryNeg(this, node);
        } break;
        case NodeType::MALLOC: {
            resolveMalloc(this, node);
        } break;
        case NodeType::FREE: {
            resolveFree(this, node);
        } break;
        case NodeType::ARRAY_LITERAL: {
            resolveArrayLiteral(this, node);
        } break;
        case NodeType::FOR: {
            resolveFor(this, node);
        } break;
        case NodeType::TAGCHECK: {
            resolveTagCheck(this, node);
        } break;
        case NodeType::HEAPIFY: {
            resolveHeapify(this, node);
        } break;
        case NodeType::ANYOF: {
            resolveAnyof(this, node);
        } break;
        case NodeType::PUTS: {
            resolvePuts(this, node);
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
