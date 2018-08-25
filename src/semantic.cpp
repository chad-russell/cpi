#include "semantic.h"
#include "bytecodegen.h"
#include "interpreter.h"
#include "parser.h"

#include <sstream>
#include <memory>
#include <utility>

Node * makeTypeConcrete(Node *typeInfo) {
    auto resolved = resolve(typeInfo);
    if (resolved == nullptr) {
        return nullptr;
    }

    if (resolved->type != NodeType::TYPE) {
        return typeInfo;
    }

    auto kind = resolved->typeData.kind;

    if (kind == NodeTypekind::INT_LITERAL) {
        typeInfo->typeData.kind = NodeTypekind::I64;
    }
    else if (kind == NodeTypekind::FLOAT_LITERAL) {
        typeInfo->typeData.kind = NodeTypekind::F64;
    }

    return typeInfo;
}

void addContextParameterForCall(Semantic *semantic, Node *node) {
    auto newParams = vector_init<Node *>(node->fnCallData.params.length + 1);

    auto newParam = new Node(node->region.srcInfo, NodeType::SYMBOL, node->scope);
    newParam->symbolData.atomId = atomTable->insertStr("context");

    // if 'context' is in scope, then pass it. Otherwise create a new one
    if (node->scope->find(newParam->symbolData.atomId)) {
        semantic->resolveTypes(newParam);
    }
    else {
        newParam = semantic->parser->createInitContextCall(node->scope);
    }

    auto newWrappedParam = wrapInValueParam(newParam, "context");
    semantic->resolveTypes(newWrappedParam);
    vector_append(newParams, newWrappedParam);

    for (auto p : node->fnCallData.params) {
        vector_append(newParams, p);
    }

    node->fnCallData.params = newParams;
}

int32_t typeAlign(Node *type) {
    auto resolved = resolve(type);
    cpi_assert(resolved->type == NodeType::TYPE);

    switch (resolved->typeData.kind) {
        case NodeTypekind::STRUCT: {
            return resolved->typeData.structTypeData.alignment;
        }
        default: {
            return typeSize(resolved);
        }
    }

    cpi_assert(false);
    return 0;
}

int32_t typeSize(Node *type) {
    auto resolved = resolve(type);
    cpi_assert(resolved->type == NodeType::TYPE);

    switch (resolved->typeData.kind) {
        case NodeTypekind::NONE:
            return 0;
        case NodeTypekind::I8:
            return 1;
        case NodeTypekind::I16:
            return 2;
        case NodeTypekind::BOOLEAN:
        case NodeTypekind::BOOLEAN_LITERAL:
        case NodeTypekind::I32:
        case NodeTypekind::FLOAT_LITERAL:
        case NodeTypekind::F32:
        case NodeTypekind::FN:
            return 4;
        case NodeTypekind::POINTER:
        case NodeTypekind::INT_LITERAL:
        case NodeTypekind::I64:
        case NodeTypekind::F64:
            return 8;
        case NodeTypekind::SYMBOL:
            cpi_assert(false);
        case NodeTypekind::STRUCT: {
            if (resolved->typeData.structTypeData.coercedType != nullptr) {
                return typeSize(resolved->typeData.structTypeData.coercedType);
            }

            auto total = 0;
            auto largest = 0;
            auto largestAlign = 0;

            // todo(chad): this should be configurable based on the width specified by the user (if/when we allow that)
            auto tagSizeInBytes = 8;

            for (auto param : resolved->typeData.structTypeData.params) {
                auto size = typeSize(param->paramData.type);
                auto align = typeAlign(param->paramData.type);

                // alignment
                if (align > 0 && total % align > 0) {
                    total += total % align;
                }

                // if it's a union and we're not assigning to the 'tag' part, then the offset is the size of the tag
                if (resolved->typeData.structTypeData.isSecretlyEnum && param->paramData.index > 0) {
                    param->localOffset = tagSizeInBytes;
                }
                else {
                    param->localOffset = total;
                }
                cpi_assert(param->type == NodeType::DECL_PARAM);

                total += size;

                if (size > largest) {
                    largest = size;
                }
                if (align > largestAlign) {
                    largestAlign = align;
                }
            }

            if (resolved->typeData.structTypeData.isSecretlyEnum) {
                total = tagSizeInBytes + largest;
            }

            // alignment
            if (largestAlign > 0 && total % largestAlign > 0) {
                total += largestAlign - (total % largestAlign);
            }

            resolved->typeData.structTypeData.alignment = largestAlign;

            return total;
        }
    }

    cpi_assert(false);
    return 0;
}

bool typesMatch(Node *desired, Node *actual, Semantic *semantic);

int maybeMatchUnionToStructLiteral(Node *desired, Node *actual, Semantic *semantic) {
    auto actualTypeData = resolve(actual)->typeData;
    if (actualTypeData.kind != NodeTypekind::STRUCT) {
        return 0;
    }
    auto actualStructData = actualTypeData.structTypeData;

    auto desiredTypeData = resolve(desired)->typeData;
    if (desiredTypeData.kind != NodeTypekind::STRUCT) {
        return 0;
    }
    auto desiredStructData = desiredTypeData.structTypeData;

    if ((actualStructData.isSecretlyEnum && !desiredStructData.isSecretlyEnum)
        || (desiredStructData.isSecretlyEnum && !actualStructData.isSecretlyEnum)) {
        // if the other one is a struct literal, then we just need to match that there is only one param,
        // and that it matches something in the other one

        Node *unionToMatchAgainst = desired;
        Node *other = actual;
        if (actualStructData.isSecretlyEnum) {
            unionToMatchAgainst = actual;
            other = desired;
        }

        if (other->typeData.structTypeData.params.length != 1) {
            return 0;
        }

        cpi_assert(vector_at(other->typeData.structTypeData.params, 0)->type == NodeType::DECL_PARAM);

        // if it doesn't have a name it obviously can't be a match
        if (vector_at(other->typeData.structTypeData.params, 0)->paramData.name == nullptr) {
            return 0;
        }

        cpi_assert(vector_at(other->typeData.structTypeData.params, 0)->paramData.name->type == NodeType::SYMBOL);
        auto otherAtomId = vector_at(other->typeData.structTypeData.params, 0)->paramData.name->symbolData.atomId;

        auto debugAtom = atomTable->backwardAtoms[otherAtomId];

        auto paramIndex = 0;
        for (auto p : unionToMatchAgainst->typeData.structTypeData.params) {
            cpi_assert(p->type == NodeType::DECL_PARAM);
            cpi_assert(p->paramData.name->type == NodeType::SYMBOL);
            auto pAtomId = p->paramData.name->symbolData.atomId;

            if (pAtomId == otherAtomId) {
                if (!typesMatch(p->paramData.type, vector_at(other->typeData.structTypeData.params, 0)->paramData.type, semantic)) {
                    return 1;
                }

                other->typeData.structTypeData.coercedType = unionToMatchAgainst;
                vector_at(other->typeData.structTypeData.params, 0)->paramData.index = paramIndex;

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

bool typesMatch(Node *desired, Node *actual, Semantic *semantic) {
    desired = resolve(desired);
    actual = resolve(actual);

    if (desired == actual) {
        return true;
    }

    if (desired->type != NodeType::TYPE) {
        semantic->resolveTypes(desired);
        desired = resolve(desired);
    }
    if (actual->type != NodeType::TYPE) {
        semantic->resolveTypes(actual);
        actual = resolve(actual);
    }

    cpi_assert(desired->type == NodeType::TYPE);
    cpi_assert(actual->type == NodeType::TYPE);

    // if we are comparing two named types and they don't have the same names, then false
    if (desired->typeData.name != nullptr
        && actual->typeData.name != nullptr
        && desired->typeData.name->symbolData.atomId != actual->typeData.name->symbolData.atomId) {
        return false;
    }

    // if we're dealing with an EXPOSED_AST with a static value, then simply propagate that
    if (actual->typeData.kind == NodeTypekind::EXPOSED_AST && actual->staticValue != nullptr) {
        actual = actual->staticValue;
    }
    if (desired->typeData.kind == NodeTypekind::EXPOSED_AST && desired->staticValue != nullptr) {
        desired = desired->staticValue;
    }

    // coercion from boolean to boolean literal or vice versa is valid
    if (desired->typeData.kind == NodeTypekind::BOOLEAN && actual->typeData.kind == NodeTypekind::BOOLEAN_LITERAL) {
        desired->typeData = actual->typeData;

        return true;
    }
    if (actual->typeData.kind == NodeTypekind::BOOLEAN && desired->typeData.kind == NodeTypekind::BOOLEAN_LITERAL) {
        actual->typeData = desired->typeData;

        return true;
    }

    // coercion from integer literal to any integer or float is valid
    // todo(chad): check for overflow?
    if (desired->typeData.kind == NodeTypekind::INT_LITERAL) {
        if (actual->typeData.kind == NodeTypekind::I8
            || actual->typeData.kind == NodeTypekind::I16
            || actual->typeData.kind == NodeTypekind::I32
            || actual->typeData.kind == NodeTypekind::I64
            || actual->typeData.kind == NodeTypekind::F32
            || actual->typeData.kind == NodeTypekind::F64) {

            desired->typeData = actual->typeData;

            if (actual->typeData.kind == NodeTypekind::F32 || actual->typeData.kind == NodeTypekind::F64) {
                desired->typeData.floatTypeData = (double) desired->typeData.intTypeData;
            }

            return true;
        }
        else if (actual->typeData.kind == NodeTypekind::INT_LITERAL) {
            // if they're both literals then they become i64 by default
            desired->typeData.kind = NodeTypekind::I64;
            actual->typeData.kind = NodeTypekind::I64;
            return true;
        }
        else if (actual->typeData.kind == NodeTypekind::FLOAT_LITERAL) {
            // if they're both literals then they become i64 by default
            desired->typeData.kind = NodeTypekind::F64;
            actual->typeData.kind = NodeTypekind::F64;

            actual->typeData.floatTypeData = (double) desired->typeData.intTypeData;

            return true;
        }
        return false;
    }
    if (actual->typeData.kind == NodeTypekind::INT_LITERAL) {
        if (desired->typeData.kind == NodeTypekind::I8
            || desired->typeData.kind == NodeTypekind::I16
            || desired->typeData.kind == NodeTypekind::I32
            || desired->typeData.kind == NodeTypekind::I64) {
            actual->typeData = desired->typeData;
            return true;
        }
        else if (desired->typeData.kind == NodeTypekind::F32 || desired->typeData.kind == NodeTypekind::F64) {
            actual->typeData = desired->typeData;
            return true;
        }

        return false;
    }

    // coercion from float literal to any float is valid
    // todo(chad): check for overflow?
    if (desired->typeData.kind == NodeTypekind::FLOAT_LITERAL) {
        if (actual->typeData.kind == NodeTypekind::F32 || actual->typeData.kind == NodeTypekind::F64) {
            desired->typeData = actual->typeData;
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
            actual->typeData = desired->typeData;
            return true;
        }
        return false;
    }

    if (actual->typeData.kind == NodeTypekind::NONE && desired->typeData.kind == NodeTypekind::NONE) {
        return true;
    }

    if (actual->typeData.kind == NodeTypekind::NONE) {
        if (desired->typeData.kind == NodeTypekind::STRUCT && desired->typeData.structTypeData.params.length == 0) {
            return true;
        }
    }

    if (desired->typeData.kind == NodeTypekind::NONE) {
        if (actual->typeData.kind == NodeTypekind::STRUCT && actual->typeData.structTypeData.params.length == 0) {
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

        auto matchedUnion = maybeMatchUnionToStructLiteral(desired, actual, semantic);
        if (matchedUnion == 1) {
            return false;
        }
        else if (matchedUnion == 2) {
            return true;
        }

        auto encounteredError = assignParams(semantic, actual, desired->typeData.structTypeData.params, actual->typeData.structTypeData.params);
        if (encounteredError) {
            return false;
        }

        for (auto p : actual->typeData.structTypeData.params) {
            semantic->resolveTypes(p);
        }

        if (!encounteredError) {
            if (!actual->typeData.structTypeData.isLiteral && desired->typeData.structTypeData.isLiteral) {
                desired->typeData.structTypeData.coercedType = actual;
            }
            else if (actual->typeData.structTypeData.isLiteral && !desired->typeData.structTypeData.isLiteral) {
                actual->typeData.structTypeData.coercedType = desired;
            }
        }

        for (unsigned long i = 0; i < desired->typeData.structTypeData.params.length; i++) {
            auto desiredParamI = vector_at(desired->typeData.structTypeData.params, i);
            auto actualParamI = vector_at(actual->typeData.structTypeData.params, i);

            Node *newT1 = resolve(desiredParamI->typeInfo);
            Node *newT2 = resolve(actualParamI->typeInfo);

            auto t1Pointiness = 0;
            auto t2Pointiness = 0;

            while (newT1->typeData.kind == NodeTypekind::POINTER) {
                t1Pointiness += 1;
                newT1 = resolve(newT1->typeData.pointerTypeData.underlyingType);
            }
            while (newT2->typeData.kind == NodeTypekind::POINTER) {
                t2Pointiness += 1;
                newT2 = resolve(newT2->typeData.pointerTypeData.underlyingType);
            }

            if (t1Pointiness != t2Pointiness) { return false; }

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
            if (actual->typeData.pointerTypeData.underlyingType->typeData.kind == NodeTypekind::NONE) {
                return true;
            }
            if (desired->typeData.pointerTypeData.underlyingType->typeData.kind == NodeTypekind::NONE) {
                return true;
            }

            return typesMatch(actual->typeData.pointerTypeData.underlyingType, desired->typeData.pointerTypeData.underlyingType, semantic);
        }

        else { return false; }
    }

    return desired->typeData.kind == actual->typeData.kind;
}

Node *defaultValueFor(Semantic *semantic, Node *type) {
    type = resolve(type);
    semantic->resolveTypes(type);

    cpi_assert(type->type == NodeType::TYPE);

    switch (type->typeData.kind) {
        case NodeTypekind::INT_LITERAL:
        case NodeTypekind::I8:
        case NodeTypekind::I16:
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
        case NodeTypekind::BOOLEAN_LITERAL: {
            auto def = new Node();
            def->type = NodeType::BOOLEAN_LITERAL;
            def->boolLiteralData.value = type->boolLiteralData.value;
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
            initStructLiteralData(def);

            for (auto param : type->typeData.structTypeData.params) {
                auto vp = param->paramData.value;
                if (vp == nullptr) {
                    vp = wrapInValueParam(defaultValueFor(semantic, param->typeInfo), param->paramData.name->symbolData.atomId);
                }
                else {
                    vp = wrapInValueParam(vp, param->paramData.name->symbolData.atomId);
                }

                semantic->resolveTypes(vp);
                vector_append(def->structLiteralData.params, vp);
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
            initStructLiteralData(def);
            def->typeInfo = type;
            return def;
        }
        default:
            cpi_assert(false);
    }

    cpi_assert(false);
    return nullptr;
}

// todo(chad): this is SLOW! probably should keep an extra linear list
void addAllFromScopeToScope(Semantic *semantic, Scope *from, Scope *to, bool forStaticIf) {
    for (auto i = 0; i < from->symbols->bucket_count; i++) {
        auto bucket = from->symbols->buckets[i];
        if (bucket != nullptr) {
            auto node = bucket->value;
            auto found = hash_get(to->symbols, bucket->key);
            if (found != nullptr) {
                ostringstream s("");
                s << "redeclaration of symbol '" << atomTable->backwardAtoms[bucket->key] << "', from import";
                semantic->reportError({}, Error{(*found)->region, s.str()});
            }

            if (!forStaticIf
                || node->type == NodeType::ALIAS
                || node->type == NodeType::MODULE
                || node->type == NodeType::TYPE
                || node->type == NodeType::FN_DECL) {
                hash_insert(to->symbols, bucket->key, node);
            }

            while (bucket->next != nullptr) {
                bucket = bucket->next;

                found = hash_get(to->symbols, bucket->key);
                if (found != nullptr) {
                    ostringstream s("");
                    s << "redeclaration of symbol '" << atomTable->backwardAtoms[bucket->key] << "', from import";
                    semantic->reportError({}, Error{(*found)->region, s.str()});
                }

                if (!forStaticIf
                    || node->type == NodeType::ALIAS
                    || node->type == NodeType::MODULE
                    || node->type == NodeType::TYPE
                    || node->type == NodeType::FN_DECL) {
                    hash_insert(to->symbols, bucket->key, node);
                }
            }
        }
    }

    for (auto ts : from->typeScopes) {
        vector_append(to->typeScopes, ts);
    }
}

void Semantic::addStaticIfs(Scope *targetScope, Scope *importInto) {
    if (targetScope->addedStaticIfs) {
        return;
    }
    targetScope->addedStaticIfs = true;

    for (auto ifStmt : targetScope->staticIfs) {
        Scope *ii = importInto;
        if (ii == nullptr) {
            ii = ifStmt->scope;
        }

        this->resolveTypes(ifStmt->ifData.condition);
        ifStmt->ifData.condition->staticValue = constantize(this, ifStmt->ifData.condition);

        cpi_assert(ifStmt->ifData.condition->staticValue->type == NodeType::BOOLEAN_LITERAL);

        if (ifStmt->ifData.condition->staticValue->boolLiteralData.value && ifStmt->ifData.stmts.length > 0) {
            addImports(ifStmt->ifData.trueImports, ifStmt->ifData.trueScopeDecls);

            addAllFromScopeToScope(this, ifStmt->ifData.ifScope, ii, true);
            addStaticIfs(ifStmt->ifData.ifScope, ifStmt->scope);
        }
        else if (!ifStmt->ifData.condition->staticValue->boolLiteralData.value && ifStmt->ifData.elseStmts.length > 0) {
            addImports(ifStmt->ifData.falseImports, ifStmt->ifData.falseScopeDecls);

            addAllFromScopeToScope(this, ifStmt->ifData.elseScope, ii, true);
            addStaticIfs(ifStmt->ifData.elseScope, ifStmt->scope);
        }
    }
}

void Semantic::addImports(vector_t<Node *> imports, vector_t<Node *> scopeDecls) {
    for (auto import : imports) {
        auto importTarget = resolve(import->importData.target);

        if (importTarget == nullptr) {
            reportError({import}, Error{import->region, "Unable to locate import"});
            return;
        }

        if (importTarget->type != NodeType::MODULE) {
            this->resolveTypes(importTarget);
        }
        importTarget = resolve(importTarget);
        cpi_assert(importTarget->type == NodeType::MODULE);

        if (!import->importData.isFile) {
            addAllFromScopeToScope(this, importTarget->scope, import->scope, false);
        }
    }

    for (auto scope : scopeDecls) {
        this->resolveTypes(scope);
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

void resolveModule(Semantic *semantic, Node *node) {
    for (auto stmt : node->moduleData.stmts) {
        semantic->resolveTypes(stmt);
    }
}

void resolveImport(Semantic *semantic, Node *node) {
    semantic->resolveTypes(node->importData.target);
}

void resolveDefer(Semantic *semantic, Node *node) {
    for (auto stmt : node->deferData.stmts) {
        semantic->resolveTypes(stmt);
    }
}

void resolveUnaryNot(Semantic *semantic, Node *node) {
    semantic->resolveTypes(node->nodeData);
    if (node->nodeData->typeInfo->typeData.kind != NodeTypekind::BOOLEAN && node->nodeData->typeInfo->typeData.kind != NodeTypekind::BOOLEAN_LITERAL) {
        semantic->reportError({node, node->nodeData}, Error{node->region, "Can only do unary-not on a boolean"});
    }

    node->typeInfo = node->nodeData->typeInfo;
}

void resolveLink(Semantic *semantic, Node *node) {
    vector_append(semantic->linkLibs, node->linkData.name);
}

void resolveScope(Semantic *semantic, Node *node) {
    semantic->resolveTypes(node->scopeData.targetType);

    vector_append(node->scope->typeScopes, node);
}

void resolveFnDecl(Semantic *semantic, Node *node) {
    auto data = &node->fnDeclData;

    auto index = 0;
    for (auto param : node->fnDeclData.params) {
        param->paramData.index = index;
        index += 1;
    }

    if (data->ctParams.length != 0 && !data->cameFromPolymorph) {
        return;
    }

    if (data->body.length > 0) {
        // todo(chad): add top-level scope for the function body as a member on data?
        semantic->addStaticIfs(vector_at(data->body, 0)->scope);
    }

    auto savedFnDecl = semantic->currentFnDecl;
    semantic->currentFnDecl = node;

    data->tableIndex = fnTableId;
    fnTableId += 1;

    for (auto param : data->ctParams) {
        semantic->resolveTypes(param);
    }

    for (auto param : data->params) {
        semantic->resolveTypes(param);
    }

    if (data->body.length == 0) {
        if (data->returnType == nullptr) {
            data->returnType = new Node(NodeTypekind::NONE);
        }
    }
    else if (data->returnType != nullptr && data->returns.length == 0) {
        semantic->reportError({node}, Error{node->region, "fn has a return type, but there are no return statements!"});
    }
    else if (data->returns.length == 0) {
        data->returnType = new Node(NodeTypekind::NONE);
    }

    if (data->returnType == nullptr) {
        auto firstReturn = vector_at(data->returns, 0);
        semantic->resolveTypes(firstReturn);
        data->returnType = firstReturn->typeInfo;
    }

    cpi_assert(data->returnType != nullptr);
    semantic->resolveTypes(data->returnType);

    node->typeInfo = new Node(NodeTypekind::FN);
    initFnTypeData(node->typeInfo);
    node->typeInfo->typeData.fnTypeData.params = vector_init<Node *>(10);
    for (auto param : data->params) {
        vector_append(node->typeInfo->typeData.fnTypeData.params, param);
    }

    // if the return type is just an int/float literal,
    // lock it down instead of leaving it open
    if (data->returnType->typeData.kind == NodeTypekind::INT_LITERAL) {
        data->returnType->typeData.kind = NodeTypekind::I64;
    }
    if (data->returnType ->typeData.kind == NodeTypekind::FLOAT_LITERAL) {
        data->returnType->typeData.kind = NodeTypekind::F32;
    }

    node->typeInfo->typeData.fnTypeData.returnType = data->returnType;

    semantic->currentFnDecl->fnDeclData.stackSize += typeSize(data->returnType);

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

    // check to see everything worked out ok
    if (data->returnType == nullptr) {
        ostringstream s("");
        s << "could not resolve return type for fn '"
          << atomTable->backwardAtoms[data->name->symbolData.atomId]
          << "'";

        semantic->reportError({node}, Error{node->region, s.str()});
    }

    for (auto stmt : data->body) {
        semantic->resolveTypes(stmt);
    }

    auto localIndex = 0;
    for (auto local : data->locals) {
        auto resolvedLocal = resolve(local);
        if (resolvedLocal != local && resolvedLocal->isLocal) { continue; }

        local->localOffset = semantic->currentFnDecl->fnDeclData.stackSize;

        // todo(chad): DANGEROUS!!! But I don't know of a better way to deal with the fact that we can have non-typechecked things because of static if, etc.
        if (local->typeInfo != nullptr) {
            semantic->currentFnDecl->fnDeclData.stackSize += typeSize(local->typeInfo);
            localIndex += 1;
        }
    }

    auto paramOffset = -8;
    for (auto declParam : node->fnDeclData.params) {
        // we push the params onto the stack in reverse order
        paramOffset -= typeSize(declParam->typeInfo);
        declParam->localOffset = paramOffset;
    }

    semantic->currentFnDecl = savedFnDecl;
}

void resolveReturn(Semantic *semantic, Node *node) {
    for (auto n : node->preStmts) {
        semantic->resolveTypes(n);
    }

    if (node->retData.value != nullptr) {
        semantic->resolveTypes(node->retData.value);
        node->typeInfo = node->retData.value->typeInfo;
    }
    else {
        node->typeInfo = new Node(NodeTypekind::NONE);
    }
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
    node->typeInfo = new Node(NodeTypekind::BOOLEAN_LITERAL);
    node->typeInfo->typeData.boolTypeData = node->boolLiteralData.value;
}

Node *constantize(Semantic *semantic, Node *node) {
    semantic->resolveTypes(node);
    node = resolve(node);

    if (node->type == NodeType::TYPE) {
        return node;
    }

    auto typeKind = resolve(node->typeInfo)->typeData.kind;
    if (typeKind == NodeTypekind::POINTER
        || typeKind == NodeTypekind::STRUCT
        || typeKind == NodeTypekind::FN
        || typeKind == NodeTypekind::SYMBOL
        || typeKind == NodeTypekind::DOT) {
        semantic->reportError({node}, Error{node->region, "type is not constant"});
        return node;
    }

    if (node->type == NodeType::INT_LITERAL || node->type == NodeType::BOOLEAN_LITERAL || node->type == NodeType::FN_DECL) {
        return node;
    }

    if (node->type == NodeType::STRUCT_LITERAL) {
        auto constNode = new Node(node->region.srcInfo, NodeType::STRUCT_LITERAL, node->scope);
        constNode->region = node->region;

        for (auto e : node->structLiteralData.params) {
            vector_append(constNode->structLiteralData.params, wrapInValueParam(constantize(semantic, e->paramData.value), e->paramData.name));
        }

        semantic->resolveTypes(constNode);
        return constNode;
    }

    auto gen = new BytecodeGen();
    gen->isMainFn = true;
    gen->sourceMap.sourceInfo = node->region.srcInfo;
    gen->processFnDecls = true;

    auto copied = semantic->deepCopyRvalue(node, node->scope);
    semantic->resolveTypes(copied);

    if (copied->type != NodeType::FN_CALL || copied->fnCallData.params.length != 0) {
        auto wrappedFn = new Node(node->region.srcInfo, NodeType::FN_DECL, node->scope);

        if (!noIppFlag) {
            semantic->parser->addContextParameterForDecl(wrappedFn->fnDeclData.params, node->scope);
        }

        auto savedCurrentFnDecl = semantic->currentFnDecl;
        semantic->currentFnDecl = wrappedFn;
        auto wrappedRet = new Node(node->region.srcInfo, NodeType::RETURN, node->scope);

        wrappedRet->nodeData = semantic->deepCopyRvalue(node, node->scope);
        semantic->currentFnDecl = savedCurrentFnDecl;

        vector_append(wrappedFn->fnDeclData.body, wrappedRet);
        vector_append(wrappedFn->fnDeclData.returns, wrappedRet);
        semantic->resolveTypes(wrappedFn);

        auto fnCall = new Node(node->region.srcInfo, NodeType::FN_CALL, node->scope);
        fnCall->fnCallData.fn = wrappedFn;

        copied = fnCall;
        semantic->resolveTypes(copied);
    }

    gen->gen(copied);
    gen->instructions.push_back((unsigned char) Instruction::EXIT);
    while (!gen->toProcess.empty()) {
        gen->isMainFn = false;
        gen->processFnDecls = true;
        gen->gen(gen->toProcess.front());
        gen->toProcess.pop();
    }
    gen->fixup();

    for (auto g : gen->generatedNodes) {
        g->gen = false;
        g->bytecode = {};
    }

    auto interp = new Interpreter(semantic->linkLibs);
    interp->instructions = gen->instructions;
    interp->fnTable = gen->fnTable;
    interp->sourceMap = gen->sourceMap;
    interp->externalFnTable = gen->externalFnTable;
    auto instructions = gen->instructions;

    interp->continuing = true;
    interp->instructions = instructions;
    interp->fnTable = gen->fnTable;

//    auto p = new MnemonicPrinter(instructions);
//    p->fnTable = gen->fnTable;
//    cout << p->debugString() << endl << endl << endl;

    interp->interpret();

    auto resolvedTypeInfo = resolve(copied->typeInfo);
    auto isStringLiteral = resolvedTypeInfo->typeData.kind == NodeTypekind::STRUCT
                           && resolvedTypeInfo->typeData.structTypeData.isSecretlyArray
                           && resolvedTypeInfo->typeData.structTypeData.secretArrayElementType->typeData.kind == NodeTypekind::I8;

    auto isBoolean = (resolvedTypeInfo->typeData.kind == NodeTypekind::BOOLEAN || resolvedTypeInfo->typeData.kind == NodeTypekind::BOOLEAN_LITERAL);

    auto staticValue = interp->readFromStack<int64_t>(copied->localOffset);

    auto staticNode = new Node();
    if (isBoolean) {
        staticNode->type = NodeType::BOOLEAN_LITERAL;
        staticNode->boolLiteralData.value = staticValue != 0;
    }
    else if (isStringLiteral) {
        staticNode->type = NodeType::STRING_LITERAL;
        // todo(chad): set read memory and set staticNode->stringLiteralData.value
    }
    else {
        staticNode->type = NodeType::INT_LITERAL;
        staticNode->intLiteralData.value = staticValue;
    }

    semantic->resolveTypes(staticNode);

    node->staticValue = staticNode;
    copied->staticValue = staticNode;

    return staticNode;
}

void resolveArrayIndex(Semantic *semantic, Node *node) {
    semantic->resolveTypes(node->arrayIndexData.target);
    semantic->resolveTypes(node->arrayIndexData.indexValue);

    auto resolvedTargetTypeInfo = resolve(resolve(node->arrayIndexData.target)->typeInfo);

    switch (resolve(node->arrayIndexData.indexValue->typeInfo)->typeData.kind) {
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

    if (resolvedTargetTypeInfo->typeData.kind != NodeTypekind::STRUCT) {
        semantic->reportError({node}, Error{node->region, "cannot index a non-array"});
        return;
    }
    if (!resolvedTargetTypeInfo->typeData.structTypeData.isSecretlyArray) {
        auto resolvedIndexValue = resolve(node->arrayIndexData.indexValue);
        semantic->resolveTypes(resolvedIndexValue);

        resolvedIndexValue = constantize(semantic, resolvedIndexValue);
        semantic->resolveTypes(resolvedIndexValue);

        // todo(chad): bounds checking

        auto oldTypeInfo = resolvedIndexValue->typeInfo;
        auto oldResolved = resolvedIndexValue->resolved;

        auto rewrittenDot = new Node(node->region);
        rewrittenDot->scope = node->scope;
        rewrittenDot->type = NodeType::DOT;
        initDotData(rewrittenDot);
        rewrittenDot->dotData.lhs = node->arrayIndexData.target;
        rewrittenDot->dotData.rhs = resolvedIndexValue;
        semantic->resolveTypes(rewrittenDot);

        // @HACK - this value sometimes gets re-assigned even when it's an int_literal... sigh...
        resolvedIndexValue->typeInfo = oldTypeInfo;
        resolvedIndexValue->resolved = oldResolved;

        node->resolved = rewrittenDot;
        node->typeInfo = rewrittenDot->typeInfo;

        cpi_assert(node->typeInfo != nullptr);

        return;
    }

    node->typeInfo = resolvedTargetTypeInfo->typeData.structTypeData.secretArrayElementType;

    // a[i] <==> ^(a.data + i)
    auto dataSym = new Node();
    dataSym->scope = node->dotData.lhs->scope;
    dataSym->type = NodeType::SYMBOL;
    dataSym->symbolData.atomId = atomTable->insertStr("data");

    auto aCasted = new Node();
    aCasted->scope = node->scope;
    aCasted->type = NodeType::DOT;
    initDotData(aCasted);
    aCasted->dotData.lhs = node->arrayIndexData.target;
    aCasted->dotData.rhs = dataSym;
    semantic->resolveTypes(aCasted);

    auto add = new Node(node->region.srcInfo, NodeType::BINOP, node->scope);
    add->binopData.type = LexerTokenType::ADD;
    add->binopData.lhs = aCasted;
    add->binopData.rhs = node->arrayIndexData.indexValue;
    add->binopData.rhsScale = typeSize(node->typeInfo);

    auto deref = new Node(node->region.srcInfo, NodeType::DEREF, node->scope);
    deref->nodeData = add;

    semantic->addLocal(deref->nodeData);

    node->resolved = deref;

    semantic->resolveTypes(node->resolved);

    node->typeInfo = node->resolved->typeInfo;
    cpi_assert(node->typeInfo != nullptr);
}

void resolveCast(Semantic *semantic, Node *node) {
    semantic->resolveTypes(node->castData.type);
    semantic->resolveTypes(node->castData.value);

    node->typeInfo = node->castData.type;
}

void resolveStringLiteral(Semantic *semantic, Node *node) {
    // typeInfo = []i8;
    node->typeInfo = makeArrayType(new Node(NodeTypekind::I8));

    // "hello" <==> {&{'h', 'e', 'l', 'l', 'o'}, 5};
    auto arrayLiteral = new Node(node->region.srcInfo, NodeType::STRUCT_LITERAL, node->scope);
    arrayLiteral->region = node->region;

    // charArrayLiteral = {'h', 'e', 'l', 'l', 'o'}
    auto charArrayLiteral = new Node(node->region);
    charArrayLiteral->type = NodeType::STRUCT_LITERAL;
    initStructLiteralData(charArrayLiteral);

    for (auto c : *node->stringLiteralData.value) {
        auto charNode = new Node(node->region);
        charNode->type = NodeType::INT_LITERAL;
        charNode->typeInfo = new Node(NodeTypekind::I8);
        charNode->intLiteralData.value = static_cast<int64_t>(c);

        vector_append(charArrayLiteral->structLiteralData.params, wrapInValueParam(charNode, ""));
    }

    Node *heapifiedCharArrayLiteral = nullptr;
    if (node->stringLiteralData.allocFn != nullptr) {
        // heap(allocFn, {'h', 'e', 'l', 'l', 'o'})

        semantic->resolveTypes(node->stringLiteralData.allocFn);
        // todo(chad): check that the type is ok here

        auto basicSym = new Node(node->region.srcInfo, NodeType::SYMBOL, node->scope);
        basicSym->symbolData.atomId = atomTable->insertStr("basic");

        auto heapFn = new Node(node->region.srcInfo, NodeType::SYMBOL, node->scope);
        heapFn->symbolData.atomId = atomTable->insertStr("heap");

        auto basicDotHeap = new Node(node->region.srcInfo, NodeType::DOT, node->scope);
        basicDotHeap->dotData.lhs = basicSym;
        basicDotHeap->dotData.rhs = heapFn;

        heapifiedCharArrayLiteral = new Node(node->region.srcInfo, NodeType::FN_CALL, node->scope);
        heapifiedCharArrayLiteral->fnCallData.fn = basicDotHeap;
        heapifiedCharArrayLiteral->fnCallData.hasRuntimeParams = true;

        vector_append(heapifiedCharArrayLiteral->fnCallData.params, wrapInValueParam(node->stringLiteralData.allocFn, nullptr));
        vector_append(heapifiedCharArrayLiteral->fnCallData.params, wrapInValueParam(charArrayLiteral, nullptr));
    }
    else {
        // {'h', 'e', 'l', 'l', 'o'}

        heapifiedCharArrayLiteral = new Node(node->region);
        heapifiedCharArrayLiteral->type = NodeType::ADDRESS_OF;
        heapifiedCharArrayLiteral->nodeData = charArrayLiteral;
    }

    semantic->addLocal(heapifiedCharArrayLiteral->nodeData);

    // countNode = 5
    auto countNode = new Node(node->region);
    countNode->type = NodeType::INT_LITERAL;
    countNode->typeInfo = new Node(NodeTypekind::I64);
    countNode->intLiteralData.value = static_cast<int64_t>(node->stringLiteralData.value->size());

    // arrayLiteral = {heap({'h', 'e', 'l', 'l', 'o'}), 5}
    vector_append(arrayLiteral->structLiteralData.params, wrapInValueParam(heapifiedCharArrayLiteral, "data"));
    vector_append(arrayLiteral->structLiteralData.params, wrapInValueParam(countNode, "count"));

    semantic->resolveTypes(arrayLiteral);

    cpi_assert(arrayLiteral->typeInfo->typeData.kind == NodeTypekind::STRUCT);
    arrayLiteral->typeInfo->typeData.structTypeData.isSecretlyArray = true;

    arrayLiteral->typeInfo->typeData.structTypeData.secretArrayElementType = new Node(NodeTypekind::I8);

    node->stringLiteralData.arrayLiteralRepresentation = arrayLiteral;
}

void resolveNilLiteral(Semantic *semantic, Node *node) {
    node->typeInfo = new Node(NodeTypekind::POINTER);
    node->typeInfo->typeData.pointerTypeData.underlyingType = new Node(NodeTypekind::NONE);
}

void resolveSymbol(Semantic *semantic, Node *node) {
    node->resolved = resolve(node->scope->find(node->symbolData.atomId));

    if (node->resolved == nullptr) {
        ostringstream s("");
        s << "undeclared identifier " << atomTable->backwardAtoms[node->symbolData.atomId];
        semantic->reportError({node}, Error{node->region, s.str()});
        return;
    }

    semantic->resolveTypes(node->resolved);
    node->typeInfo = node->resolved->typeInfo;
}

bool assignParams(Semantic *semantic, Node *errorReportTarget, const vector_t<Node *> &declParams, vector_t<Node *> &givenParams) {
    vector_t<bool> openParams = vector_init<bool>(declParams.length + 1);
    vector_t<Node *> newParams = vector_init<Node *>(declParams.length + 1);

    for (unsigned long i = 0; i < declParams.length; i++) {
        vector_append(openParams, true);
        vector_append(newParams, (Node *) nullptr);
    }

    auto encounteredError = false;

    for (auto passedParam : givenParams) {
        auto valueAndNameNotNull = passedParam->type == NodeType::VALUE_PARAM && passedParam->paramData.name != nullptr;
        auto declAndNameNotNull = passedParam->type == NodeType::DECL_PARAM && passedParam->paramData.name != nullptr;

        if (valueAndNameNotNull || declAndNameNotNull) {
            // look up that name in the declaration
            auto found = false;
            for (unsigned long j = 0; j < declParams.length; j++) {
                auto declParam = vector_at(declParams, j);
                cpi_assert(declParam->type == NodeType::DECL_PARAM);

                int64_t passedParamAtomId = passedParam->paramData.name->symbolData.atomId;

                if (declParam->paramData.name->symbolData.atomId == passedParamAtomId) {
                    found = true;
                    auto openParam = vector_at(openParams, j);
                    if (!openParam) {
                        encounteredError = true;
                        semantic->reportError({errorReportTarget},
                                              Error{errorReportTarget->region, "passed a parameter twice"});
                    }

                    if (typesMatch(declParam->typeInfo, passedParam->typeInfo, semantic)) {
                        passedParam->typeInfo = declParam->paramData.type;
                        vector_set_at(newParams, j, passedParam);
                        vector_set_at(openParams, j, false);
                    }
                    else {
                        encounteredError = true;
                    }
                }
            }
            if (!found) {
                encounteredError = true;
                semantic->reportError({errorReportTarget}, Error{errorReportTarget->region, "unable to find one of the parameters"});
            }
        }
        else {
            // put it in next open slot
            unsigned long j = 0;
            while (j < newParams.length && !(vector_at(openParams, j))) { j += 1; }
            if (j >= newParams.length) {
                encounteredError = true;
                semantic->reportError({errorReportTarget}, Error{errorReportTarget->region, "too many parameters passed!"});
            }
            else {
                auto declParam = vector_at(declParams, j);

                if (declParam->type == NodeType::DECL_PARAM) {
                    passedParam->typeInfo = declParam->paramData.type;
                    passedParam->paramData.name = declParam->paramData.name;
                } else if (declParam->type == NodeType::VALUE_PARAM) {
                    passedParam->typeInfo = declParam->paramData.value->typeInfo;
                    passedParam->paramData.name = declParam->paramData.name;
                } else {
                    cpi_assert(false);
                }

                vector_set_at(newParams, j, passedParam);
                vector_set_at(openParams, j, false);
            }
        }
    }

    for (unsigned long i = 0; i < newParams.length; i++) {
        auto declParam = vector_at(declParams, i);

        // if the decl parameter is unassigned but there is a default value, then fill in that param with the default
        bool openParamsI = vector_at(openParams, i);
        if (declParam->paramData.value != nullptr && openParamsI) {
            vector_set_at(openParams, i, false);
            declParam->paramData.value->typeInfo = declParam->paramData.type;
            vector_set_at(newParams, i, wrapInValueParam(declParam->paramData.value, declParam->paramData.name));
        }
    }

    for (unsigned long i = 0; i < declParams.length; i++) {
        auto openParamsI = vector_at(openParams, i);
        if (openParamsI || vector_at(newParams, i) == nullptr) {
            encounteredError = true;

            ostringstream s("");
            s << "unassigned parameter: " << atomTable->backwardAtoms[vector_at(declParams, i)->paramData.name->symbolData.atomId];
            auto sstr = s.str();
            semantic->reportError({errorReportTarget}, Error{errorReportTarget->region, sstr});
        }
    }

    givenParams = newParams;

    return encounteredError;
}

void maybeStructDefault(Semantic *semantic, Node *rhs, Node *lhsType) {
    while (rhs->type == NodeType::ADDRESS_OF || rhs->type == NodeType::DEREF) {
        rhs = rhs->nodeData;
    }

    if (rhs->type == NodeType::STRUCT_LITERAL
        && lhsType->typeData.kind == NodeTypekind::STRUCT
        && !lhsType->typeData.structTypeData.isSecretlyEnum
        && !lhsType->typeData.structTypeData.isSecretlyArray) {
        assignParams(semantic, rhs, lhsType->typeData.structTypeData.params, rhs->structLiteralData.params);
    }
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
    if (shouldCheckTypeMatch && !typesMatch(node->declData.type, node->declData.initialValue->typeInfo, semantic)) {
        ostringstream s("");
        s << "Type mismatch! wanted " << node->declData.type->typeData
          << ", got " << resolve(node->declData.initialValue->typeInfo)->typeData;
        semantic->reportError({node, node->declData.initialValue, node->declData.type},
                              Error{node->region, s.str()});
    }
    else if (node->declData.type == nullptr) {
        node->declData.type = node->declData.initialValue->typeInfo;
    }
    else if (node->declData.initialValue == nullptr) {
        node->declData.initialValue = defaultValueFor(semantic, resolve(node->declData.type));
    }

    if (shouldCheckTypeMatch) {
        maybeStructDefault(semantic, node->declData.initialValue, resolve(node->declData.type));
    }

    auto resolvedDeclDataType = resolve(node->declData.type);
    node->typeInfo = makeTypeConcrete(node->declData.type);

    if (node->declData.initialValue == nullptr) {
        return;
    }

    auto matchedUnion = maybeMatchUnionToStructLiteral(resolvedDeclDataType, node->declData.initialValue->typeInfo, semantic);
    if (matchedUnion == 1) {
        semantic->reportError({}, Error{node->region, "error assigning struct literal to union - unmatched field name"});
    }
}

void resolveAlias(Semantic *semantic, Node *node) {
    resolveDecl(semantic, node);
    node->resolved = constantize(semantic, resolve(node->declData.initialValue));
    semantic->resolveTypes(node->resolved);
    node->typeInfo = node->resolved->typeInfo;
}

void resolveType(Semantic *semantic, Node *node) {
    switch (node->typeData.kind) {
        case NodeTypekind::NONE:
        case NodeTypekind::BOOLEAN:
        case NodeTypekind::BOOLEAN_LITERAL:
        case NodeTypekind::EXPOSED_AST:
        case NodeTypekind::INT_LITERAL:
        case NodeTypekind::FLOAT_LITERAL:
        case NodeTypekind::I8:
        case NodeTypekind::I16:
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
                semantic->resolveTypes(param);
                auto size = typeSize(param->paramData.type);
                auto align = typeAlign(param->paramData.type);

                if (align > 0) { total += total % align; } // alignment

                param->localOffset = total;
                param->paramData.index = localIndex;

                total += size;

                localIndex += 1;
            }
        } break;
        case NodeTypekind::SYMBOL: {
            node->resolved = node->scope->find(node->typeData.symbolTypeData.atomId);

            if (node->resolved == nullptr) {
                ostringstream s("");
                s << "undeclared identifier " << atomTable->backwardAtoms[node->typeData.symbolTypeData.atomId];
                semantic->reportError({node}, Error{node->region, s.str()});
                return;
            }

            auto resolved = resolve(node);

            semantic->resolveTypes(resolved);
        } break;
        case NodeTypekind::POINTER: {
            semantic->resolveTypes(node->typeData.pointerTypeData.underlyingType);
        } break;
        case NodeTypekind::DOT: {
            semantic->resolveTypes(node->typeData.dotTypeData);
            node->typeData = resolve(node->typeData.dotTypeData)->typeData;
        } break;
    }
}

Node *makeTemporary(Semantic *semantic, Node *n) {
    auto node = new Node(n->region);
    node->typeInfo = n->typeInfo;
    semantic->addLocal(node);
    return node;
}

void rewritePipe(Semantic *semantic, Node *node) {
    auto resolvedLhs = resolve(node->binopData.lhs);
    auto resolvedRhs = resolve(node->binopData.rhs);

    semantic->resolveTypes(resolvedLhs);

    auto fnCallNode = new Node(node->region.srcInfo, NodeType::FN_CALL, node->scope);
    fnCallNode->region = node->region;
    fnCallNode->fnCallData.hasRuntimeParams = true;
    vector_append(fnCallNode->fnCallData.params, wrapInValueParam(resolvedLhs, nullptr));

    if (resolvedRhs->type == NodeType::DOT) {
        semantic->resolveTypes(resolvedRhs);
        resolvedRhs = resolve(resolvedRhs);
    }

    if (resolvedRhs->type == NodeType::SYMBOL || resolvedRhs->type == NodeType::FN_DECL) {
        // rhs is symbol or fn
        fnCallNode->fnCallData.fn = resolvedRhs;
    }
    else if (resolvedRhs->type == NodeType::FN_CALL) {
        // rhs is fn call
        fnCallNode->fnCallData.fn = resolvedRhs->fnCallData.fn;
        for (auto param : resolvedRhs->fnCallData.params) {
            vector_append(fnCallNode->fnCallData.params, param);
        }
        for (auto param : resolvedRhs->fnCallData.ctParams) {
            vector_append(fnCallNode->fnCallData.ctParams, param);
        }
    }
    else {
        // error
        cpi_assert(false);
    }

    semantic->resolveTypes(fnCallNode);

    node->resolved = fnCallNode;
    node->typeInfo = fnCallNode->typeInfo;
}

void resolveBinop(Semantic *semantic, Node *node) {
    if (node->binopData.type == LexerTokenType::VERTICAL_BAR) {
        rewritePipe(semantic, node);
        return;
    }

    semantic->resolveTypes(node->binopData.lhs);
    semantic->resolveTypes(node->binopData.rhs);

    node->binopData.lhsTemporary = makeTemporary(semantic, node->binopData.lhs);
    node->binopData.rhsTemporary = makeTemporary(semantic, node->binopData.rhs);

    auto resolvedLhsType = resolve(node->binopData.lhs->typeInfo);
    auto resolvedRhsType = resolve(node->binopData.rhs->typeInfo);

    if (node->binopData.lhs->type == NodeType::DOT) {
        auto newLocalStorage = new Node(node->region);
        newLocalStorage->type = NodeType::TYPE;
        newLocalStorage->typeInfo = resolvedLhsType;

        semantic->addLocal(newLocalStorage);
    }
    if (node->binopData.rhs->type == NodeType::DOT) {
        auto newLocalStorage = new Node(node->region);
        newLocalStorage->type = NodeType::TYPE;
        newLocalStorage->typeInfo = resolvedRhsType;

        semantic->addLocal(newLocalStorage);
    }

    if (resolvedLhsType->typeData.kind == NodeTypekind::POINTER
        && (resolvedRhsType->typeData.kind == NodeTypekind::INT_LITERAL
            || resolvedRhsType->typeData.kind == NodeTypekind::I64)) {
        node->typeInfo = resolvedLhsType;
        node->binopData.rhsScale = typeSize(resolvedLhsType->typeData.pointerTypeData.underlyingType);
        return;
    }

    if (resolvedRhsType->typeData.kind == NodeTypekind::POINTER
        && (resolvedLhsType->typeData.kind == NodeTypekind::INT_LITERAL
            || resolvedLhsType->typeData.kind == NodeTypekind::I64)) {
        node->typeInfo = resolvedRhsType;
        node->binopData.rhsScale = typeSize(resolvedRhsType->typeData.pointerTypeData.underlyingType);
        return;
    }

    if (!typesMatch(resolvedLhsType, resolvedRhsType, semantic)) {
        semantic->reportError({node, node->binopData.lhs, node->binopData.rhs},
                              Error{node->region, "type mismatch - both sides of binary operation need to be the same type"});
    }

    if (isBooleanBinop(node->binopData.type)) {
        node->typeInfo = new Node(NodeTypekind::BOOLEAN);
    } else {
        node->typeInfo = resolvedLhsType;
    }
}

Node *Semantic::deepCopyScopedStmt(Node *node, Scope *scope) {
    Node *copied = nullptr;

    if (node->type == NodeType::END_SCOPE) {
        copied = new Node(node->region.srcInfo, NodeType::END_SCOPE, node->scope);
        copied->region = node->region;
    }
    else {
        auto copyingLexer = new Lexer(*node->region.srcInfo.fileName, true);
        copyingLexer->lastLoc = node->region.start;
        copyingLexer->loc = node->region.start;
        copyingLexer->popFront();
        copyingLexer->popFront();

        auto copyingParser = new Parser(copyingLexer);
        copyingParser->isCopying = true;
        copyingParser->scopes.pop();
        copyingParser->scopes.push(scope);
        copyingParser->currentFnDecl = currentFnDecl;
        copied = copyingParser->parseScopedStmt();
    }

    for (auto s : node->preStmts) {
        vector_append(copied->preStmts, deepCopyScopedStmt(s, s->scope));
    }
    for (auto s : node->postStmts) {
        vector_append(copied->postStmts, deepCopyScopedStmt(s, s->scope));
    }

    return copied;
}

Node *Semantic::deepCopyRvalue(Node *node, Scope *scope) {
    auto copyingLexer = new Lexer(node->region.srcInfo, node);

    auto copyingParser = new Parser(copyingLexer);
    copyingParser->isCopying = true;
    copyingParser->scopes.pop();
    copyingParser->scopes.push(scope);
    copyingParser->currentFnDecl = currentFnDecl;

    return copyingParser->parseRvalue();
}

Node *resolveSymbolWithScopeType(Semantic *semantic, Node *fnSymbol, Node *firstParam, Scope *scope) {
    semantic->resolveTypes(firstParam);

    auto resolvedScopeType = resolve(firstParam->typeInfo);
    while (resolvedScopeType->typeData.kind == NodeTypekind::POINTER) {
        resolvedScopeType = resolve(resolvedScopeType->typeData.pointerTypeData.underlyingType);
    }

    while (scope != nullptr) {
        for (auto ts : scope->typeScopes) {
            auto resolvedTarget = resolve(ts->scopeData.targetType);

            if (typesMatch(resolvedTarget, resolvedScopeType, semantic)) {
                auto found = ts->scopeData.scope->find(fnSymbol->symbolData.atomId);
                if (found != nullptr) {
                    return found;
                }
            }
        }
        scope = scope->parent;
    }

    return nullptr;
}

void resolveFnCall(Semantic *semantic, Node *node) {
    node->sourceMapStatement = true;
    semantic->addLocal(node);

    Node *resolvedFn = nullptr;

    // if the fn is a symbol and there's at least 1 runtime param
    if (resolvedFn == nullptr && node->fnCallData.fn->type == NodeType::SYMBOL && node->fnCallData.params.length > 0) {
        // try to find it normally
        resolvedFn = resolve(node->scope->find(node->fnCallData.fn->symbolData.atomId));
        if (resolvedFn != nullptr) {
            node->fnCallData.fn->resolved = resolvedFn;
        }

        auto firstParam = vector_at(node->fnCallData.params, 0);

        // if that fails, look in any scopes that we can find
        if (resolvedFn == nullptr) {
            resolvedFn = resolveSymbolWithScopeType(semantic, node->fnCallData.fn, firstParam, node->fnCallData.fn->scope);
            if (resolvedFn != nullptr) {
                node->fnCallData.fn->resolved = resolvedFn;
            }
        }

        // if that fails, look in any scopes in the scope where the type was defined
        if (resolvedFn == nullptr) {
            auto fpType = resolve(firstParam->typeInfo);
            while (fpType->typeData.kind == NodeTypekind::POINTER) {
                fpType = resolve(fpType->typeData.pointerTypeData.underlyingType);
            }

            resolvedFn = resolveSymbolWithScopeType(semantic, node->fnCallData.fn, firstParam, fpType->scope);
            if (resolvedFn != nullptr) {
                node->fnCallData.fn->resolved = resolvedFn;
            }
        }
    }

    // if we haven't found anything, then just do the normal thing
    if (resolvedFn == nullptr) {
        resolvedFn = node->fnCallData.fn;
    }

    semantic->resolveTypes(resolvedFn);
    resolvedFn = resolve(resolvedFn);

    auto isPoly = resolvedFn->type == NodeType::FN_DECL && resolvedFn->fnDeclData.ctParams.length != 0;
    Node *polyResolvedFn = nullptr;

    if (!noIppFlag) {
        bool shouldAddContextParam = false;
        if (resolvedFn->type != NodeType::FN_DECL) {
            shouldAddContextParam = true;
        }
        else if (resolvedFn->fnDeclData.name != nullptr && !resolvedFn->fnDeclData.isExternal) {
            auto initContextAtom = atomTable->insertStr("initContext");

            if (resolvedFn->fnDeclData.name->type == NodeType::DOT) {
                auto fnNameAtom = resolvedFn->fnDeclData.name->dotData.rhs->symbolData.atomId;
                shouldAddContextParam = fnNameAtom != initContextAtom;
            }
            else if (resolvedFn->fnDeclData.name->type == NodeType::SYMBOL) {
                auto fnNameAtom = resolvedFn->fnDeclData.name->symbolData.atomId;
                shouldAddContextParam = fnNameAtom != semantic->parser->mainAtom && fnNameAtom != initContextAtom;
            }
        }
        else if (resolvedFn->fnDeclData.name != nullptr && resolvedFn->fnDeclData.isExternal) {
            shouldAddContextParam = false;
        }
        else {
            shouldAddContextParam = true;
        }

        if (shouldAddContextParam) {
            addContextParameterForCall(semantic, node);
        }
    }

    if (isPoly) {
        // make a new function
        // todo(chad): memoize this based on the ctParams
        polyResolvedFn = semantic->deepCopyScopedStmt(resolvedFn, resolvedFn->scope);
        cpi_assert(polyResolvedFn->type == NodeType::FN_DECL);
        polyResolvedFn->fnDeclData.cameFromPolymorph = true;

        node->fnCallData.fn->resolved = polyResolvedFn;
        resolvedFn = polyResolvedFn;
    }

    // assign runtime params
    if (node->fnCallData.hasRuntimeParams) {
        vector_t<Node *> declParams;

        if (isPoly) {
            cpi_assert(resolvedFn->type == NodeType::FN_DECL);

            declParams = resolvedFn->fnDeclData.params;
        } else {
            auto resolvedFnType = resolve(resolvedFn->typeInfo)->typeData;
            cpi_assert(resolvedFnType.kind == NodeTypekind::FN);

            declParams = resolvedFnType.fnTypeData.params;
        }

        auto encounteredError = assignParams(semantic, node, declParams, node->fnCallData.params);
        for (auto p : node->fnCallData.params) {
            semantic->resolveTypes(p);
        }

        if (encounteredError) {
            cpi_assert(false);
        }

        if (isPoly) {
            // 'link' each decl param to its runtime param
            for (unsigned long p = 0; p < declParams.length; p++) {
                vector_at(declParams, p)->paramData.polyLink = vector_at(node->fnCallData.params, p);
            }
        }
    }

    // assign compile-time params
    if (isPoly) {
        auto encounteredError = assignParams(semantic, node, resolvedFn->fnDeclData.ctParams, node->fnCallData.ctParams);
        if (encounteredError) {
            cpi_assert(false);
        }

        // We want to resolve all semantic parameters, but we DON'T want to add any locals
        auto savedFnDecl = semantic->currentFnDecl;
        semantic->currentFnDecl = nullptr;
        for (unsigned long i = 0; i < resolvedFn->fnCallData.ctParams.length; i++) {
            auto paramI = vector_at(resolvedFn->fnDeclData.ctParams, i);
            if (paramI->paramData.isAutoPolyParam) {
                semantic->resolveTypes(vector_at(node->fnCallData.ctParams, i));
            }
        }
        for (unsigned long i = 0; i < resolvedFn->fnCallData.ctParams.length; i++) {
            auto paramI = vector_at(resolvedFn->fnDeclData.ctParams, i);
            semantic->resolveTypes(paramI);
        }
        semantic->currentFnDecl = savedFnDecl;

        auto ctDeclParams = resolvedFn->fnDeclData.ctParams;
        auto ctGivenParams = node->fnCallData.ctParams;

        cpi_assert(ctDeclParams.length == ctGivenParams.length);

        // Make sure the ctDeclParams resolve to their compile-time values
        for (unsigned long i = 0; i < ctGivenParams.length; i++) {
            const auto& ctParam = vector_at(ctGivenParams, i);
            const auto& ctDeclParam = vector_at(ctDeclParams, i);

            Node *ctValue;
            if (ctParam->type == NodeType::VALUE_PARAM) {
                ctValue = ctParam->paramData.value;
            } else {
                ctValue = ctParam;
            }
            ctValue = resolve(ctValue);

            auto declParamType = resolve(ctDeclParam->paramData.type);
            if (declParamType != nullptr && declParamType->typeData.kind == NodeTypekind::EXPOSED_AST) {
                ctDeclParam->staticValue = ctValue;
            }
            else {
                ctDeclParam->staticValue = constantize(semantic, ctValue);
            }
        }
    }

    // un-assign runtime params
    if (node->fnCallData.hasRuntimeParams) {
        vector_t<Node *> declParams;

        if (isPoly) {
            cpi_assert(resolvedFn->type == NodeType::FN_DECL);
            declParams = resolvedFn->fnDeclData.params;
        } else {
            auto resolvedFnType = resolve(resolvedFn->typeInfo)->typeData;
            cpi_assert(resolvedFnType.kind == NodeTypekind::FN);
            declParams = resolvedFnType.fnTypeData.params;
        }

        if (isPoly) {
            // un-link
            for (unsigned long p = 0; p < declParams.length; p++) {
                vector_at(declParams, p)->paramData.polyLink = nullptr;
            }

            // @Hack!!!
            for (auto d : declParams) {
                d->semantic = false;
            }
            for (auto d : declParams) {
                semantic->resolveTypes(d);
            }
        }
    }

    if (isPoly) {
        auto ctDeclParams = resolvedFn->fnDeclData.ctParams;
        auto ctGivenParams = node->fnCallData.ctParams;

        // put ourselves in the context of the newly resolved fn in order to do the decl params
        auto savedCurrentFnDecl = semantic->currentFnDecl;
        semantic->currentFnDecl = polyResolvedFn;

        for (unsigned long i = 0; i < ctGivenParams.length; i++) {
            auto declParam = vector_at(ctDeclParams, i);
            auto givenParam = vector_at(ctGivenParams, i);

            if (!declParam->paramData.isAutoPolyParam) {
                continue;
            }

            semantic->resolveTypes(declParam);
            semantic->resolveTypes(givenParam);

            // todo(chad): just skipping everything if the typeInfo is null seems really dangerous...
            // probably want to find another way to categorize these based on some polymorphic quality/flag
            if (declParam->typeInfo != nullptr) {
                if (declParam->typeInfo->typeData.kind == NodeTypekind::EXPOSED_AST) {
                    // if declParam is an exposed 'Any' type, convert the givenParam to that
                    givenParam->typeInfo = new Node(NodeTypekind::EXPOSED_AST);
                    givenParam->typeInfo->staticValue = givenParam;
                }
                else {
                    if (!typesMatch(declParam->typeInfo, givenParam->paramData.value->typeInfo, semantic)) {
                        // todo if match exposed to real type then set resolved on exposed to real and pass match
                        // unless already set then error?
                        semantic->reportError({node, declParam, givenParam}, Error{node->region, "static type mismatch!"});
                    }
                }
            }
        }

        for (unsigned long i = 0; i < ctGivenParams.length; i++) {
            auto declParam = vector_at(ctDeclParams, i);
            auto givenParam = vector_at(ctGivenParams, i);

            if (declParam->paramData.isAutoPolyParam) {
                continue;
            }

            semantic->resolveTypes(declParam);
            semantic->resolveTypes(givenParam);

            // todo(chad): just skipping everything if the typeInfo is null seems really dangerous...
            // probably want to find another way to categorize these based on some polymorphic quality/flag
            if (declParam->typeInfo != nullptr) {
                if (declParam->typeInfo->typeData.kind == NodeTypekind::EXPOSED_AST) {
                    // if declParam is an exposed 'Any' type, convert the givenParam to that
                    givenParam->typeInfo = new Node(NodeTypekind::EXPOSED_AST);
                    givenParam->typeInfo->staticValue = givenParam;
                }
                else {
                    if (!typesMatch(declParam->typeInfo, givenParam->paramData.value->typeInfo, semantic)) {
                        // todo if match exposed to real type then set resolved on exposed to real and pass match
                        // unless already set then error?
                        semantic->reportError({node, declParam, givenParam}, Error{node->region, "static type mismatch!"});
                    }
                }
            }
        }

        semantic->currentFnDecl = savedCurrentFnDecl;
    }

    semantic->resolveTypes(resolvedFn);

    if (node->fnCallData.hasRuntimeParams) {
        auto resolvedFnType = resolve(resolvedFn->typeInfo)->typeData;
        cpi_assert(resolvedFnType.kind == NodeTypekind::FN);

        auto declParams = resolvedFnType.fnTypeData.params;
        auto givenParams = node->fnCallData.params;

        for (unsigned long i = 0; i < givenParams.length; i++) {
            auto declParam = vector_at(declParams, i);
            auto givenParam = vector_at(givenParams, i);
            semantic->resolveTypes(givenParam);
            if (!typesMatch(declParam->typeInfo, givenParam->typeInfo, semantic)) {
                semantic->reportError({node, declParam, givenParam}, Error{node->region, "type mismatch!"});
            }

            maybeStructDefault(semantic, givenParam->paramData.value, resolve(declParam->paramData.type));
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
        assigningToUnion = typeData.kind == NodeTypekind::STRUCT && typeData.structTypeData.isSecretlyEnum;
    }

    // don't set the tag when assigning to the tag parameter
    if (assigningToUnion) {
        assigningToUnion = node->dotData.rhs->symbolData.atomId != atomTable->insertStr("tag");
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
        param0->symbolData.atomId = atomTable->insertStr("tag");
        param0->resolved = vector_at(typeData.structTypeData.params, 0);
        cpi_assert(vector_at(typeData.structTypeData.params, 0)->type == NodeType::DECL_PARAM);
        param0->typeInfo = vector_at(typeData.structTypeData.params, 0)->paramData.type;

        auto tagDot = new Node();
        tagDot->scope = originalAssignment->scope;
        tagDot->type = NodeType::DOT;
        initDotData(tagDot);
        tagDot->dotData.lhs = mostLhs;
        tagDot->dotData.rhs = param0;
        semantic->resolveTypes(tagDot);

        auto foundParam = secondmostLhs->dotData.resolved;
        uint64_t paramIndex = (uint64_t) foundParam->paramData.index;

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
        initAssignData(secretAss);
        secretAss->assignData.lhs = tagDot;
        secretAss->assignData.rhs = constParamIndex;

        vector_append(originalAssignment->postStmts, secretAss);

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

    maybeStructDefault(semantic, node->assignData.rhs, resolve(node->assignData.lhs->typeInfo));

    semantic->lvalueAssignmentContext = true;
    possiblyResolveAssignToUnion(semantic, node, node->assignData.lhs);
    semantic->lvalueAssignmentContext = false;

    auto resolvedDeclDataType = resolve(node->assignData.lhs->typeInfo);

    auto matchedUnion = maybeMatchUnionToStructLiteral(resolvedDeclDataType, node->assignData.rhs->typeInfo, semantic);
    if (matchedUnion == 1) {
        semantic->reportError({}, Error{node->region, "error assigning struct literal to union - unmatched field name"});
    }
}

void resolveDeclParam(Semantic *semantic, Node *node) {
    cpi_assert(!(node->paramData.type == nullptr && node->paramData.value == nullptr));
    semantic->resolveTypes(node->paramData.value);

    if (node->paramData.type == nullptr) {
        node->paramData.type = node->paramData.value->typeInfo;
    }

    semantic->resolveTypes(node->paramData.type);
    if (node->paramData.polyLink != nullptr) {
        semantic->resolveTypes(node->paramData.polyLink);
        node->typeInfo = node->paramData.polyLink->typeInfo;
    } else {
        node->typeInfo = node->paramData.type;
    }

    if (node->paramData.type != nullptr && node->paramData.value != nullptr) {
        if (!typesMatch(node->paramData.type, node->paramData.value->typeInfo, semantic)) {
            semantic->reportError({node}, Error{node->region, "Type mismatch decl param"});
        }

        maybeStructDefault(semantic, node->paramData.value, resolve(node->paramData.type));
    }

    makeTypeConcrete(node->typeInfo);
}

void resolveValueParam(Semantic *semantic, Node *node) {
    semantic->resolveTypes(node->paramData.value);
    node->typeInfo = node->paramData.value->typeInfo;
}

void resolveDeref(Semantic *semantic, Node *node) {
    semantic->resolveTypes(node->nodeData);
    auto pointerType = resolve(node->nodeData->typeInfo);

    if (pointerType->typeData.kind != NodeTypekind::POINTER) {
        semantic->reportError({node}, Error{node->region, "Cannot dereference a non-pointer"});
        return;
    }

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
        node->resolved = resolve(node->nodeData)->nodeData;
    }
}

Node *findParam(Semantic *semantic, Node *node) {
    if (node->dotData.resolved) {
        return node->dotData.resolved;
    }

    auto resolvedTypeInfo = resolve(node->dotData.lhs)->typeInfo;
    cpi_assert(resolvedTypeInfo != nullptr);

    auto typeData = &resolve(resolvedTypeInfo)->typeData;
    while (typeData->kind == NodeTypekind::POINTER) {
        typeData = &resolve(typeData->pointerTypeData.underlyingType)->typeData;
    }
    if (typeData->kind != NodeTypekind::STRUCT) {
        semantic->reportError({node}, Error{node->region, "cannot perform dot operation on this!"});
    }
    auto structData = typeData->structTypeData;

    cpi_assert(node->dotData.rhs->type == NodeType::SYMBOL || node->dotData.rhs->type == NodeType::INT_LITERAL);

    Node *foundParam = nullptr;
    if (node->dotData.rhs->type == NodeType::INT_LITERAL) {
        foundParam = vector_at(structData.params, node->dotData.rhs->intLiteralData.value);
    }
    else {
        auto sizeSoFar = 0;
        for (const auto &param : structData.params) {
            auto paramSize = typeSize(param->paramData.type);
            auto paramAlign = typeAlign(param->paramData.type);

            // alignment
            if (sizeSoFar > 0 && paramAlign > 0) {
                sizeSoFar += sizeSoFar % paramAlign;
            }

            param->localOffset = sizeSoFar;

            sizeSoFar += paramSize;
        }

        for (auto param : structData.params) {
            if (param->paramData.name != nullptr) {
                if (param->paramData.name->symbolData.atomId == node->dotData.rhs->symbolData.atomId) {
                    foundParam = param;
                    break;
                }
            }
        }
    }

    node->dotData.resolved = foundParam;

    if (foundParam != nullptr && resolve(node->dotData.lhs)->type == NodeType::STRUCT_LITERAL) {
        cpi_assert(foundParam->type == NodeType::DECL_PARAM);
        auto foundValue = vector_at(resolve(node->dotData.lhs)->structLiteralData.params,
                                    (unsigned long) foundParam->paramData.index)->paramData.value;
        node->resolved = foundValue;
        node->dotData.resolved = foundValue;
    }

    return foundParam;
}

void createTagCheck(Semantic *semantic, Node *node) {
    // tagcheck(a.b)
    auto tagCheck = new Node();
    tagCheck->scope = node->scope;
    tagCheck->type = NodeType::TAGCHECK;
    tagCheck->nodeData = node;
    semantic->resolveTypes(tagCheck);

    // false
    auto falseConst = new Node();
    falseConst->scope = node->scope;
    falseConst->type = NodeType::BOOLEAN_LITERAL;
    falseConst->boolLiteralData.value = false;

    // tagcheck(a.b) == false
    auto eqFalse = new Node();
    eqFalse->scope = node->scope;
    eqFalse->type = NodeType::BINOP;
    initBinopData(eqFalse);
    eqFalse->binopData.type = LexerTokenType::EQ_EQ;
    eqFalse->binopData.lhs = tagCheck;
    eqFalse->binopData.rhs = falseConst;
    semantic->addLocal(eqFalse);
    semantic->resolveTypes(eqFalse);

    // panic();
    auto panicStmt = new Node();
    panicStmt->scope = node->scope;
    panicStmt->type = NodeType::PANIC;

    // if tagcheck(a.b) == false { panic(); }
    auto ifCheck = new Node();
    ifCheck->scope = node->scope;
    ifCheck->type = NodeType::IF;
    initIfData(ifCheck);
    ifCheck->ifData.condition = eqFalse;
    vector_append(ifCheck->ifData.stmts, panicStmt);

    vector_append(node->preStmts, ifCheck);
}

void resolveModuleDot(Semantic *semantic, Node *node) {
    auto found = resolve(node->dotData.lhs)->scope->find(node->dotData.rhs->symbolData.atomId);
    semantic->resolveTypes(found);

    if (found == nullptr) {
        semantic->reportError({node, found}, Error{node->region, "Could not resolved dot"});
        return;
    }

    node->resolved = found;
    node->typeInfo = found->typeInfo;
}

void resolveDot(Semantic *semantic, Node *node, Node *lhs, Node *rhs) {
    semantic->resolveTypes(lhs);

    auto resolvedLhs = resolve(lhs);
    semantic->resolveTypes(resolvedLhs);

    if (resolvedLhs->type == NodeType::MODULE) {
        resolveModuleDot(semantic, node);
        return;
    }

    semantic->addLocal(node->dotData.lhs);

    Node *resolvedLhsTypeInfo;

    // follow the lhs all the way through pointers, symbols, etc. and assert it's a struct
    if (!semantic->lvalueAssignmentContext) {
        resolvedLhsTypeInfo = resolve(resolvedLhs->typeInfo);
        while (resolvedLhsTypeInfo->typeData.kind == NodeTypekind::POINTER) {
            resolvedLhsTypeInfo = resolve(resolvedLhsTypeInfo->typeData.pointerTypeData.underlyingType);
        }
        cpi_assert(resolvedLhsTypeInfo->typeData.kind == NodeTypekind::STRUCT);
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

        if (node->resolved != nullptr) {
            semantic->resolveTypes(node->resolved);
            node->typeInfo = node->resolved->typeInfo;
            return;
        }

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
        local->typeData.pointerTypeData.underlyingType = nullptr;
        local->typeInfo = local;
        semantic->addLocal(local);

        node->dotData.autoDerefStorage = local;
    }

    if (!semantic->lvalueAssignmentContext) {
        auto isUnionAccess = resolvedLhsTypeInfo->typeData.structTypeData.isSecretlyEnum;

        // todo(chad): keep the atomId for "tag" handy so we don't have to look it up all the time...
        auto isTagAccess = node->dotData.rhs->symbolData.atomId == atomTable->insertStr("tag");

        if (isUnionAccess && !isTagAccess) {
            createTagCheck(semantic, node);
        }
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
    initStructTypeData(node->typeInfo);
    node->typeInfo->typeData.structTypeData.isLiteral = true;

    auto paramIndex = 0;
    for (auto param : node->structLiteralData.params) {
        semantic->resolveTypes(param);

        auto wrapped = wrapInDeclParam(param->typeInfo, param->paramData.name, paramIndex);
        vector_append(node->typeInfo->typeData.structTypeData.params, wrapped);

        param->typeInfo = param->paramData.value->typeInfo;

        param->paramData.index = paramIndex;
        paramIndex += 1;
    }
}

void resolveIf(Semantic *semantic, Node *node) {
    semantic->resolveTypes(node->ifData.condition);

    auto resolvedTypeInfo = resolve(node->ifData.condition->typeInfo);
    auto resolvedKind = resolvedTypeInfo->typeData.kind;
    if (resolvedKind != NodeTypekind::BOOLEAN && resolvedKind != NodeTypekind::BOOLEAN_LITERAL) {
        semantic->reportError({node, node->ifData.condition}, Error{node->ifData.condition->region, "Condition for 'if' must be a boolean!"});
    }

    auto isStatic = false;
    bool staticCondition;
    if (node->ifData.isStatic) {
        isStatic = true;
        cpi_assert(node->ifData.condition->staticValue->type == NodeType::BOOLEAN_LITERAL);
        staticCondition = node->ifData.condition->staticValue->boolLiteralData.value;
    }
    else if (resolvedTypeInfo->typeData.kind == NodeTypekind::BOOLEAN_LITERAL && resolve(node->ifData.condition)->type == NodeType::DECL) {
        isStatic = true;
        staticCondition = resolvedTypeInfo->typeData.boolTypeData;
    }

    bool shouldResolveIfStmts = true;
    bool shouldResolveElseStmts = true;
    if (isStatic) {
        if (staticCondition) {
            shouldResolveElseStmts = false;
            node->ifData.elseStmts = {};
        }
        else {
            shouldResolveIfStmts = false;
            node->ifData.stmts = {};
        }
    }

    if (shouldResolveIfStmts) {
        for (const auto& stmt : node->ifData.stmts) {
            semantic->resolveTypes(stmt);
        }
    }

    if (shouldResolveElseStmts) {
        for (const auto& stmt : node->ifData.elseStmts) {
            semantic->resolveTypes(stmt);
        }
    }
}

void resolveWhile(Semantic *semantic, Node *node) {
    semantic->resolveTypes(node->whileData.condition);

    if (node->whileData.condition->typeInfo->typeData.kind != NodeTypekind::BOOLEAN && node->whileData.condition->typeInfo->typeData.kind != NodeTypekind::BOOLEAN_LITERAL) {
        semantic->reportError({node, node->whileData.condition}, Error{node->whileData.condition->region, "Condition for 'while' must be a boolean!"});
    }

    for (const auto& stmt : node->whileData.stmts) {
        semantic->resolveTypes(stmt);
    }
}

void resolveUnaryNeg(Semantic *semantic, Node *node) {
    semantic->resolveTypes(node->unaryNegData.target);
    semantic->resolveTypes(node->unaryNegData.rewritten);
    node->typeInfo = node->nodeData->typeInfo;
}

void resolveArrayLiteral(Semantic *semantic, Node *node) {
    for (auto elem : node->arrayLiteralData.elements) {
        semantic->resolveTypes(elem);
    }

    if (node->arrayLiteralData.elementType == nullptr) {
        if (node->arrayLiteralData.elements.length == 0) {
            semantic->reportError({node}, Error{
                    node->region,
                    "No type given for array literal and no elements from which to infer it"
            });
            return;
        }
        node->arrayLiteralData.elementType = vector_at(node->arrayLiteralData.elements, 0)->typeInfo;
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

    // create struct literal representation
    auto elemsStruct = new Node(node->region.srcInfo, NodeType::STRUCT_LITERAL, node->scope);
    for (auto elem : node->arrayLiteralData.elements) {
        vector_append(elemsStruct->structLiteralData.params, wrapInValueParam(elem, ""));
    }

    Node *heapified = nullptr;
    if (node->arrayLiteralData.allocFn != nullptr) {
        // heap(allocFn, elemsStruct)

        semantic->resolveTypes(node->arrayLiteralData.allocFn);
        // todo(chad): check that the type is ok here

        auto basicSym = new Node(node->region.srcInfo, NodeType::SYMBOL, node->scope);
        basicSym->symbolData.atomId = atomTable->insertStr("basic");

        auto heapFn = new Node(node->region.srcInfo, NodeType::SYMBOL, node->scope);
        heapFn->symbolData.atomId = atomTable->insertStr("heap");

        auto basicDotHeap = new Node(node->region.srcInfo, NodeType::DOT, node->scope);
        basicDotHeap->dotData.lhs = basicSym;
        basicDotHeap->dotData.rhs = heapFn;

        heapified = new Node(node->region.srcInfo, NodeType::FN_CALL, node->scope);
        heapified->fnCallData.fn = basicDotHeap;
        heapified->fnCallData.hasRuntimeParams = true;

        vector_append(heapified->fnCallData.params, wrapInValueParam(node->arrayLiteralData.allocFn, nullptr));
        vector_append(heapified->fnCallData.params, wrapInValueParam(elemsStruct, nullptr));
    }
    else {
        // &elemsStruct

        heapified = new Node(node->region.srcInfo, NodeType::ADDRESS_OF, node->scope);
        heapified->nodeData = elemsStruct;
    }

    semantic->addLocal(elemsStruct);
    semantic->addLocal(heapified);

    Node *typeOfElem = nullptr;
    if (node->arrayLiteralData.elementType != nullptr) {
        typeOfElem = node->arrayLiteralData.elementType;
    }
    else if (node->arrayLiteralData.elements.length != 0) {
        typeOfElem = new Node(node->region.srcInfo, NodeType::TYPEOF, node->scope);
        typeOfElem->nodeData = vector_at(node->arrayLiteralData.elements, 0);
    }

    auto pointerToTypeOfElem = new Node(NodeTypekind::POINTER);
    pointerToTypeOfElem->typeData.pointerTypeData.underlyingType = typeOfElem;

    auto castedHeapified = new Node(node->region.srcInfo, NodeType::CAST, node->scope);
    castedHeapified->castData.type = pointerToTypeOfElem;
    castedHeapified->castData.value = heapified;

    semantic->addLocal(castedHeapified);
    semantic->addLocal(heapified);

    auto countNode = new Node(node->region.srcInfo, NodeType::INT_LITERAL, node->scope);
    countNode->intLiteralData.value = static_cast<int64_t>(elemsStruct->structLiteralData.params.length);
    countNode->typeInfo = new Node(NodeTypekind::I64);

    node->arrayLiteralData.structLiteralRepresentation = new Node(node->region.srcInfo, NodeType::STRUCT_LITERAL, node->scope);
    vector_append(node->arrayLiteralData.structLiteralRepresentation->structLiteralData.params, wrapInValueParam(castedHeapified, "data"));
    vector_append(node->arrayLiteralData.structLiteralRepresentation->structLiteralData.params, wrapInValueParam(countNode, "count"));

    node->resolved = node->arrayLiteralData.structLiteralRepresentation;

    semantic->resolveTypes(node->resolved);
    node->resolved->typeInfo->typeData.structTypeData.isSecretlyArray = true;
    node->resolved->typeInfo->typeData.structTypeData.secretArrayElementType = node->arrayLiteralData.elementType;

    node->typeInfo = node->resolved->typeInfo;
}

void resolveStaticFor(Semantic *semantic, Node *node) {
    semantic->resolveTypes(node->forData.target);

    auto resolvedTarget = node->forData.target;
    if (resolvedTarget->type == NodeType::ARRAY_LITERAL) {
        // todo(chad): we need to move away from using 'resolved' for array literals...
    }
    else if (resolvedTarget->type == NodeType::FIELDSOF) {
        resolvedTarget = resolvedTarget->resolved;
    }
    else {
        resolvedTarget = resolve(resolvedTarget);
    }

    auto resolvedTargetType = resolve(resolvedTarget->typeInfo);

    // if the target is not an array literal, error
    vector_t<Node *> staticElements = vector_init<Node *>(10);
    if (resolvedTarget->type == NodeType::ARRAY_LITERAL) {
        staticElements = resolvedTarget->arrayLiteralData.elements;
    }
    else if (resolvedTargetType->typeData.kind == NodeTypekind::STRUCT
            && !resolvedTargetType->typeData.structTypeData.isSecretlyArray
            && !resolvedTargetType->typeData.structTypeData.isSecretlyEnum) {
        auto params = resolvedTargetType->typeData.structTypeData.params;

        for (unsigned long staticIdx = 0; staticIdx < params.length; staticIdx += 1) {
            auto intLiteral = new Node();
            intLiteral->type = NodeType::INT_LITERAL;
            intLiteral->typeInfo = new Node(NodeTypekind::I64);
            intLiteral->intLiteralData.value = (int64_t) staticIdx;

            auto elemDot = new Node();
            elemDot->scope = resolvedTarget->scope;
            elemDot->type = NodeType::DOT;
            initDotData(elemDot);
            elemDot->dotData.lhs = resolvedTarget;
            elemDot->dotData.rhs = intLiteral;
            semantic->resolveTypes(elemDot);

            vector_append(staticElements, elemDot);
        }
    }
    else {
        semantic->reportError({node, node->forData.target, resolvedTarget}, Error{node->forData.target->region, "Can only #for over an array literal or a struct"});
        return;
    }

    int64_t staticIdx = 0;
    for (auto staticElem : staticElements) {
        // todo(chad): test scopes here

        // new declaration for elem
        hash_insert(node->scope->symbols, node->forData.element_alias->symbolData.atomId, staticElem);

        Node *indexLiteral = nullptr;
        if (node->forData.iterator_alias != nullptr) {
            indexLiteral = new Node();
            indexLiteral->type = NodeType::INT_LITERAL;
            indexLiteral->typeInfo = new Node(NodeTypekind::I64);
            indexLiteral->intLiteralData.value = staticIdx;
            hash_insert(node->scope->symbols, node->forData.iterator_alias->symbolData.atomId, indexLiteral);
        }

        auto subScope = new Scope(node->scope);

        for (auto stmt : node->forData.stmts) {
            auto newStmt = semantic->deepCopyScopedStmt(stmt, subScope);

            semantic->resolveTypes(newStmt);
            vector_append(node->forData.staticStmts, newStmt);
        }

        hash_erase(node->scope->symbols, node->forData.element_alias->symbolData.atomId);
        if (node->forData.iterator_alias != nullptr) {
            hash_erase(node->scope->symbols, node->forData.iterator_alias->symbolData.atomId);
        }
        staticIdx += 1;
    }
}

void resolveFor(Semantic *semantic, Node *node) {
    if (node->forData.isStatic) {
        resolveStaticFor(semantic, node);
        return;
    }

    semantic->resolveTypes(node->forData.target);
    auto resolvedTypeInfo = resolve(node->forData.target->typeInfo);

    if (!(resolvedTypeInfo->typeData.kind == NodeTypekind::STRUCT && resolvedTypeInfo->typeData.structTypeData.isSecretlyArray)) {
        semantic->reportError({node, node->forData.target}, Error{
                node->forData.target->region,
                "'for' target must be an array"
        });
    }

    // create a new declaration with the alias as the lvalue, and the array type as the type
    auto elementDecl = new Node();
    elementDecl->type = NodeType::DECL;
    initDeclData(elementDecl);
    elementDecl->scope = node->scope;
    elementDecl->region = node->forData.element_alias->region;
    elementDecl->declData.type = resolvedTypeInfo->typeData.structTypeData.secretArrayElementType;
    elementDecl->declData.lhs = node->forData.element_alias;
    semantic->addLocal(elementDecl);
    semantic->resolveTypes(elementDecl);

    // insert this new declaration into the scope of the 'for' statement
    hash_insert(node->scope->symbols, node->forData.element_alias->symbolData.atomId, elementDecl);

    // 0
    auto zero = new Node();
    zero->type = NodeType::INT_LITERAL;
    zero->typeInfo = new Node(NodeTypekind::I64);
    zero->intLiteralData.value = 0;

    // indexDecl: i64 = 0
    auto indexDecl = new Node();
    indexDecl->type = NodeType::DECL;
    initDeclData(indexDecl);
    indexDecl->scope = node->scope;
    if (node->forData.iterator_alias != nullptr) {
        indexDecl->region = node->forData.iterator_alias->region;
    }
    indexDecl->declData.type = new Node(NodeTypekind::I64);
    indexDecl->declData.lhs = node->forData.iterator_alias;
    indexDecl->declData.initialValue = zero;
    semantic->addLocal(indexDecl);
    semantic->resolveTypes(indexDecl);

    auto index = indexDecl;
    if (node->forData.iterator_alias != nullptr) {
        node->forData.iterator_alias->resolved = indexDecl;
        hash_insert(node->scope->symbols, node->forData.iterator_alias->symbolData.atomId, indexDecl);

        index = node->forData.iterator_alias;
    }

    // arrayDecl
    auto arrayDecl = new Node(node->region.srcInfo, NodeType::DECL, node->scope);
    arrayDecl->declData.type = resolvedTypeInfo;
    arrayDecl->declData.initialValue = node->forData.target;
    semantic->addLocal(arrayDecl);

    for (auto& stmt : node->forData.stmts) {
        semantic->resolveTypes(stmt);
    }

    auto countSymbol = new Node();
    countSymbol->scope = node->forData.target->scope;
    countSymbol->type = NodeType::SYMBOL;
    countSymbol->symbolData.atomId = atomTable->insertStr("count");

    auto arrayDotCount = new Node();
    arrayDotCount->scope = node->scope;
    arrayDotCount->type = NodeType::DOT;
    initDotData(arrayDotCount);
    arrayDotCount->dotData.lhs = arrayDecl;
    arrayDotCount->dotData.rhs = countSymbol;

    auto whileConditionBinop = new Node();
    whileConditionBinop->type = NodeType::BINOP;
    initBinopData(whileConditionBinop);
    whileConditionBinop->binopData.type = LexerTokenType::LT;
    whileConditionBinop->binopData.lhs = index;
    whileConditionBinop->binopData.rhs = arrayDotCount;
    semantic->addLocal(whileConditionBinop);

    // one
    auto one = new Node();
    one->type = NodeType::INT_LITERAL;
    one->typeInfo = new Node(NodeTypekind::I64);
    one->intLiteralData.value = 1;

    // indexDecl + 1
    auto incrementIndexBinop = new Node(node->region.srcInfo, NodeType::BINOP, node->scope);
    incrementIndexBinop->type = NodeType::BINOP;
    initBinopData(incrementIndexBinop);
    incrementIndexBinop->binopData.type = LexerTokenType::ADD;
    incrementIndexBinop->binopData.lhs = index;
    incrementIndexBinop->binopData.rhs = one;
    semantic->addLocal(incrementIndexBinop);

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
    vector_append(while_->whileData.stmts, elementAssign);

    // stmts
    for (auto stmt : node->forData.stmts) {
        vector_append(while_->whileData.stmts, stmt);
    }

    // increment
    vector_append(while_->whileData.stmts, incr);

    vector_append(node->forData.rewritten, elementDecl);
    vector_append(node->forData.rewritten, arrayDecl);
    vector_append(node->forData.rewritten, index);
    vector_append(node->forData.rewritten, while_);

    for (auto stmt : node->forData.rewritten) {
        semantic->resolveTypes(stmt);
    }
}

void resolveTagCheck(Semantic *semantic, Node *node) {
    if (node->tagCheck) { return; }
    node->tagCheck = true;

    semantic->resolveTypes(node->nodeData);
    auto resolvedNode = resolve(node->nodeData);

    if (resolvedNode->type != NodeType::DOT) {
        semantic->reportError({node, resolvedNode}, Error{
                node->region,
                "Expected dot for tagcheck"
        });
        return;
    }

    semantic->resolveTypes(node->nodeData);

    auto resolvedType = resolve(resolvedNode->dotData.lhs->typeInfo);
    while (resolvedType->typeData.kind == NodeTypekind::POINTER) {
        resolvedType = resolve(resolvedType->typeData.pointerTypeData.underlyingType);
    }

    cpi_assert(resolvedType->typeData.kind == NodeTypekind::STRUCT);
    if (!resolvedType->typeData.structTypeData.isSecretlyEnum) {
        semantic->reportError({node, node->nodeData}, Error{
                node->region,
                "Can only do a tagcheck on a union"
        });
    }

    auto resolved = resolvedNode->dotData.resolved;
    cpi_assert(resolved->type == NodeType::DECL_PARAM);

    // eqeq
    auto typeData = resolve(resolvedNode->dotData.lhs->typeInfo)->typeData;
    while (typeData.kind == NodeTypekind::POINTER) {
        typeData = resolve(typeData.pointerTypeData.underlyingType)->typeData;
    }

    // the resolved parameter is the 0th parameter of the type data
    auto param0 = new Node();
    param0->scope = resolvedNode->dotData.lhs->scope;
    param0->type = NodeType::SYMBOL;
    param0->symbolData.atomId = atomTable->insertStr("tag");
    param0->resolved = vector_at(typeData.structTypeData.params, 0);
    cpi_assert(vector_at(typeData.structTypeData.params, 0)->type == NodeType::DECL_PARAM);
    param0->typeInfo = vector_at(typeData.structTypeData.params, 0)->paramData.type;

    auto tagDot = new Node();
    tagDot->scope = node->scope;
    tagDot->type = NodeType::DOT;
    initDotData(tagDot);
    tagDot->dotData.lhs = resolvedNode->dotData.lhs;
    tagDot->dotData.rhs = param0;
    semantic->resolveTypes(tagDot);

    auto paramIndex = resolved->paramData.index;

    // assign to 0th parameter the value of paramIndex
    auto constParamIndex = new Node();
    constParamIndex->scope = node->scope;
    constParamIndex->type = NodeType::INT_LITERAL;
    constParamIndex->intLiteralData.value = paramIndex;

    auto eqeq = new Node();
    eqeq->scope = node->scope;
    eqeq->type = NodeType::BINOP;
    initBinopData(eqeq);
    eqeq->binopData.type = LexerTokenType::EQ_EQ;
    eqeq->binopData.lhs = tagDot;
    eqeq->binopData.rhs = constParamIndex;
    semantic->addLocal(eqeq);
    semantic->resolveTypes(eqeq);

    node->resolved = eqeq;
    node->typeInfo = node->resolved->typeInfo;
}

void resolvePuts(Semantic *semantic, Node *node) {
    semantic->resolveTypes(node->nodeData);
    auto resolvedType = resolve(resolve(node->nodeData)->typeInfo);

    semantic->addLocal(resolve(node->nodeData));

    if (resolvedType->typeData.kind == NodeTypekind::STRUCT
        && resolvedType->typeData.structTypeData.isSecretlyArray
        && resolve(resolvedType->typeData.structTypeData.secretArrayElementType)->typeData.kind == NodeTypekind::I8) {
        return;
    }

    semantic->reportError({node}, Error{node->region, "Expected string for call to puts..."});
}

void resolveRun(Semantic *semantic, Node *node) {
    semantic->resolveTypes(node->nodeData);

    node->typeInfo = node->nodeData->typeInfo;

    cpi_assert(node->nodeData->type == NodeType::FN_CALL);
    auto ctFn = node->nodeData->fnCallData.fn;

    auto ctSemantic = new Semantic();
    ctSemantic->resolveTypes(ctFn);

    auto gen = new BytecodeGen();
    gen->isMainFn = true;
    gen->sourceMap.sourceInfo = node->region.srcInfo;
    gen->processFnDecls = true;

    // wrap in a function that simply returns the value -- that way we get args and stuff
    auto wrappedFn = new Node(node->region.srcInfo, NodeType::FN_DECL, node->scope);
    auto wrappedRet = new Node(node->region.srcInfo, NodeType::RETURN, node->scope);
    wrappedRet->nodeData = node->nodeData;
    vector_append(wrappedFn->fnDeclData.body, wrappedRet);
    vector_append(wrappedFn->fnDeclData.returns, wrappedRet);
    ctSemantic->resolveTypes(wrappedFn);
    auto fnCall = new Node(node->region.srcInfo, NodeType::FN_CALL, node->scope);
    fnCall->fnCallData.fn = wrappedFn;
    // done wrapping

    gen->gen(fnCall);
    gen->instructions.push_back((unsigned char) Instruction::EXIT);
    while (!gen->toProcess.empty()) {
        gen->isMainFn = false;
        gen->processFnDecls = true;
        gen->gen(gen->toProcess.front());
        gen->toProcess.pop();
    }
    gen->fixup();

    auto interp = new Interpreter(semantic->linkLibs);
    interp->instructions = gen->instructions;
    interp->fnTable = gen->fnTable;
    interp->sourceMap = gen->sourceMap;
    interp->externalFnTable = gen->externalFnTable;
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
            cpi_assert(false && "unsupported typekind for CTE");
        } break;
    }
}

void resolveTypeof(Semantic *semantic, Node *node) {
    semantic->resolveTypes(node->nodeData);
    node->typeInfo = new Node(NodeTypekind::EXPOSED_AST);
    node->staticValue = node->nodeData->typeInfo;
}

void resolveReturnTypeof(Semantic *semantic, Node *node) {
    semantic->resolveTypes(node->nodeData);

    node->typeInfo = new Node(NodeTypekind::EXPOSED_AST);

    cpi_assert(node->nodeData->typeInfo->typeData.kind == NodeTypekind::FN);

    node->staticValue = node->nodeData->typeInfo->typeData.fnTypeData.returnType;
}

void resolveIsKind(Semantic *semantic, Node *node) {
    semantic->resolveTypes(node->isKindData.type);

    auto resolvedType = resolve(node->isKindData.type);
    cpi_assert(resolvedType->type == NodeType::TYPE);

    auto matches = false;
    switch (resolvedType->typeData.kind) {
        case NodeTypekind::NONE: {
            matches = node->isKindData.tokenType == LexerTokenType::NONE;
        } break;
        case NodeTypekind::I8: {
            matches = node->isKindData.tokenType == LexerTokenType::I8;
        } break;
        case NodeTypekind::I16: {
            matches = node->isKindData.tokenType == LexerTokenType::I16;
        } break;
        case NodeTypekind::I32: {
            matches = node->isKindData.tokenType == LexerTokenType::I32;
        } break;
        case NodeTypekind::INT_LITERAL:
        case NodeTypekind::I64: {
            matches = node->isKindData.tokenType == LexerTokenType::I64;
        } break;
        case NodeTypekind::F32: {
            matches = node->isKindData.tokenType == LexerTokenType::F32;
        } break;
        case NodeTypekind::FLOAT_LITERAL:
        case NodeTypekind::F64: {
            matches = node->isKindData.tokenType == LexerTokenType::F64;
        } break;
        case NodeTypekind::FN: {
            matches = node->isKindData.tokenType == LexerTokenType::FN;
        } break;
        case NodeTypekind::BOOLEAN:
        case NodeTypekind::BOOLEAN_LITERAL: {
            matches = node->isKindData.tokenType == LexerTokenType::BOOLEAN;
        } break;
        case NodeTypekind::STRUCT: {
            if (resolvedType->typeData.structTypeData.isSecretlyArray) {
                matches = node->isKindData.tokenType == LexerTokenType::LSQUARE;
            }
            else if (resolvedType->typeData.structTypeData.isSecretlyEnum) {
                matches = node->isKindData.tokenType == LexerTokenType::ENUM;
            }
            else {
                matches = node->isKindData.tokenType == LexerTokenType::STRUCT;
            }
        } break;
        case NodeTypekind::POINTER: {
            matches = node->isKindData.tokenType == LexerTokenType::MUL;
        } break;
        default: {}
    }

    auto resolved = new Node();
    resolved->scope = node->scope;
    resolved->type = NodeType::BOOLEAN_LITERAL;
    resolved->boolLiteralData.value = matches;
    semantic->resolveTypes(resolved);

    node->resolved = resolved;
    node->staticValue = resolved;

    node->typeInfo = node->resolved->typeInfo;
}

void resolveSizeof(Semantic *semantic, Node *node) {
    semantic->resolveTypes(node->nodeData);

    node->typeInfo = new Node(NodeTypekind::I64);
}

void resolveFieldsof(Semantic *semantic, Node *node) {
    semantic->resolveTypes(node->nodeData);
    auto resolvedNodeData = resolve(node->nodeData);

    auto canDo = true;
    auto isEnum = false;
    vector_t<Node *> params = vector_init<Node *>(10);
    if (resolvedNodeData->typeData.kind == NodeTypekind::STRUCT) {
        if (resolvedNodeData->typeData.structTypeData.isSecretlyArray) {
            canDo = false;
        }
        isEnum = resolvedNodeData->typeData.structTypeData.isSecretlyEnum;

        params = resolvedNodeData->typeData.structTypeData.params;
    }
    else if (resolvedNodeData->typeData.kind == NodeTypekind::FN) {
        params = resolvedNodeData->typeData.fnTypeData.params;
    }
    else {
        canDo = false;
    }

    if (!canDo) {
        semantic->reportError({node}, Error{node->region, "Can only do #fieldsof on a struct/enum/fn type"});
        return;
    }

    auto resolved = new Node();
    resolved->type = NodeType::ARRAY_LITERAL;
    initArrayLiteralData(resolved);

    for (auto param : params) {
        cpi_assert(param->type == NodeType::DECL_PARAM);

        if (param->paramData.index == 0 && isEnum) {
            // skip the 'tag' field
            continue;
        }

        auto indexLit = new Node();
        indexLit->scope = node->scope;
        indexLit->type = NodeType::INT_LITERAL;
        indexLit->intLiteralData.value = param->paramData.index;

        auto nameLit = new Node();
        nameLit->scope = node->scope;
        nameLit->type = NodeType::STRING_LITERAL;
        nameLit->stringLiteralData.value = new string(
                param->paramData.name == nullptr ? "" : atomTable->backwardAtoms[param->paramData.name->symbolData.atomId]);

        auto fieldLit = new Node();
        fieldLit->scope = node->scope;
        fieldLit->type = NodeType::STRUCT_LITERAL;
        initStructLiteralData(fieldLit);
        fieldLit->structLiteralData.params = vector_init<Node *>(2);
        vector_append(fieldLit->structLiteralData.params, wrapInValueParam(indexLit, "index"));
        vector_append(fieldLit->structLiteralData.params, wrapInValueParam(nameLit, "name"));

        vector_append(resolved->arrayLiteralData.elements, fieldLit);
    }

    semantic->resolveTypes(resolved);
    node->resolved = resolved;
    node->typeInfo = node->resolved->typeInfo;
}

void Semantic::resolveTypes(Node *node) {
    if (node == nullptr) { return; }

    if (node->semantic) { return; }
    node->semantic = true;

    for (auto stmt : node->preStmts) {
        resolveTypes(stmt);
    }
    for (auto stmt : node->postStmts) {
        resolveTypes(stmt);
    }

    switch (node->type) {
        case NodeType::FN_DECL: {
            resolveFnDecl(this, node);
        } break;
        case NodeType::RETURN: {
            resolveReturn(this, node);
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
        case NodeType::RETURNTYPEOF: {
            resolveReturnTypeof(this, node);
        } break;
        case NodeType::ISKIND: {
            resolveIsKind(this, node);
        } break;
        case NodeType::SIZEOF: {
            resolveSizeof(this, node);
        } break;
        case NodeType::FIELDSOF: {
            resolveFieldsof(this, node);
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
        case NodeType::ARRAY_LITERAL: {
            resolveArrayLiteral(this, node);
        } break;
        case NodeType::FOR: {
            resolveFor(this, node);
        } break;
        case NodeType::TAGCHECK: {
            resolveTagCheck(this, node);
        } break;
        case NodeType::PUTS: {
            resolvePuts(this, node);
        } break;
        case NodeType::MODULE: {
            resolveModule(this, node);
        } break;
        case NodeType::IMPORT: {
            resolveImport(this, node);
        } break;
        case NodeType::DEFER: {
            resolveDefer(this, node);
        } break;
        case NodeType::ALIAS: {
            resolveAlias(this, node);
        } break;
        case NodeType::UNARY_NOT: {
            resolveUnaryNot(this, node);
        } break;
        case NodeType::LINK: {
            resolveLink(this, node);
        } break;
        case NodeType::SCOPE: {
            resolveScope(this, node);
        } break;
        case NodeType::END_SCOPE: break;
        default: cpi_assert(false);
    }
}

void Semantic::addLocal(Node *local) {
    if (local->isLocal) { return; }
    local->isLocal = true;

    if (currentFnDecl) {
        vector_append(currentFnDecl->fnDeclData.locals, local);
    }
}
