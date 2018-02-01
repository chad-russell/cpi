#include "semantic.h"

#include <sstream>

int64_t typeSize(Node *type) {
    assert(type->type == NodeType::TYPE);

    switch (type->typeData.kind) {
        case NodeTypekind::INT_LITERAL:
        case NodeTypekind::I32: {
            return 4;
        }
        case NodeTypekind::I64: {
            return 8;
        }
        default: assert(false);
    }
}

bool typesMatch(Node *desired, Node *actual) {
    assert(desired->type == NodeType::TYPE);
    assert(actual->type == NodeType::TYPE);

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
    auto data = node->fnDeclData;
    auto savedFnDecl = semantic->currentFnDecl;
    semantic->currentFnDecl = data;

    for (auto param : data.params) {
        semantic->resolveTypes(param);
    }

    if (data.returnType != nullptr) {
        if (data.returns.size() == 0) {
            semantic->reportError({node},
                                  Error{node->region, "fn has a return type, but there are no return statements!"});
        }
    }
    else if (data.returns.size() == 0) {
        data.returnType = new Node(NodeTypekind::NONE);
    }
    else {
        auto firstReturn = data.returns[0];
        semantic->resolveTypes(firstReturn);
        data.returnType = firstReturn->typeInfo;
    }

    node->typeInfo = new Node(NodeTypekind::FN);
    node->typeInfo->typeData.fnTypeData.params = {};
    for (auto param : data.params) {
        node->typeInfo->typeData.fnTypeData.params.push_back(param);
    }

    // if the return type is just an int/float literal,
    // lock it down instead of leaving it open
    if (data.returnType != nullptr
        && data.returnType->typeData.kind == NodeTypekind::INT_LITERAL) {
        data.returnType->typeData.kind = NodeTypekind::I32;
    }
    if (data.returnType != nullptr
        && data.returnType ->typeData.kind == NodeTypekind::FLOAT_LITERAL) {
        data.returnType->typeData.kind = NodeTypekind::F32;
    }

    node->typeInfo->typeData.fnTypeData.returnType = data.returnType;

    semantic->currentFnDecl.stackSize += typeSize(data.returnType);

    for (auto ret : data.returns) {
        semantic->resolveTypes(ret);

         if (ret->typeInfo != nullptr) {
             if (!typesMatch(ret->typeInfo, data.returnType)) {
                 ostringstream s("");
                 s << "Type mismatch; function return type is "
                   << data.returnType->typeData.kind
                   << ", but this is "
                   << ret->typeInfo->typeData.kind;

                 semantic->reportError({ret, ret->typeInfo, data.returnType},
                 Error{ret->retData.value->region, s.str()});
             }
         }
         else {
             if (data.returnType == nullptr) {
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

    for (auto stmt : data.body) {
        semantic->resolveTypes(stmt);
    }

    // check to see everything worked out ok
    // if (data.return_type == NULL) {
    //     report_semantic_error_fmt(ctx, vector_init_ptrsize_items(&node, NULL), data.name,
    //                                 "could not resolve return type for fn '%region'",
    //                                 &data.name);
    // }

    for (auto local : data.locals) {
        local->localOffset = semantic->currentFnDecl.stackSize;
        semantic->currentFnDecl.stackSize += typeSize(local->typeInfo);
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

    assert(node->resolved->type == NodeType::DECL);
    semantic->resolveTypes(node->resolved);
    node->typeInfo = node->resolved->typeInfo;
}

void resolveDecl(Semantic *semantic, Node *node) {
    semantic->resolveTypes(node->declData.type);
    semantic->resolveTypes(node->declData.initialValue);

    node->typeInfo = node->declData.type;
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
        default: assert(false);
    }
}