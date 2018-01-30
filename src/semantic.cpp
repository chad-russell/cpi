#include "semantic.h"

void resolveFnDecl(Semantic *semantic, Node *node) {
    auto data = node->fnDeclData;
    auto savedFnDecl = semantic->currentFnDecl;
    semantic->currentFnDecl = data;

    for (auto param : data.params) {
        semantic->resolveTypes(param);
    }

    if (data.returnType != nullptr) {
        if (data.returns.size() == 0) {
            // report_semantic_error_fmt(ctx, vector_init_ptrsize_items(&node, NULL),
            //                             node->region,
            //                             "fn has a return type, but there are no return statements!");
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

    // for (size_t i = 0; i < data.returns->length; i++) {
    //     struct node_t *ret = (struct node_t *) vector_at_deref(data.returns, i);

    //     resolve_types(ctx, ret);

    //     if (ret->type_info != NULL) {
    //         if (!types_match(ctx, ret->type_info, data.return_type)) {
    //             report_semantic_error_fmt(ctx,
    //                                         vector_init_ptrsize_items(&ret, &ret->type_info, &data.return_type, NULL),
    //                                         ret->data.ret_data.node_data->region,
    //                                         "Type mismatch. Type of return statement is %type_data, buf function return type is %type_data",
    //                                         &ret->type_info->data.type_data,
    //                                         &data.return_type->data.type_data);
    //         }
    //     }
    //     else {
    //         if (data.return_type == NULL) {
    //             struct vector_t *notes = vector_init(1, sizeof(struct note_t));
    //             struct note_t note = note_t_init(ret->data.ret_data.node_data->region, "because we cannot find the type of this returned value");
    //             vector_append(notes, &note);
    //             report_semantic_error_with_notes_fmt(ctx, vector_init_ptrsize_items(&node, &ret, NULL), node->region, notes, "could not resolve return type for fn");
    //         }
    //         else {
    //             report_semantic_error_fmt(ctx, vector_init_ptrsize_items(&node, &ret, &ret->data.ret_data.node_data, NULL),
    //                                         ret->data.ret_data.node_data->region,
    //                                         "could not resolve type of return statement"
    //             );
    //         }
    //     }
    // }

    for (auto stmt : data.body) {
        semantic->resolveTypes(stmt);
    }

    // check to see everything worked out ok
    // if (data.return_type == NULL) {
    //     report_semantic_error_fmt(ctx, vector_init_ptrsize_items(&node, NULL), data.name,
    //                                 "could not resolve return type for fn '%region'",
    //                                 &data.name);
    // }

    semantic->currentFnDecl = savedFnDecl;
}

void resolveRet(Semantic *semantic, Node *node) {
    semantic->resolveTypes(node->nodeData);
}

void Semantic::resolveTypes(Node *node) {
    if (node == nullptr) { return; }

    switch (node->type) {
        case NodeType::FN_DECL: {
            resolveFnDecl(this, node);
        } break;
        case NodeType::RET: {
            resolveRet(this, node);
        } break;
        case NodeType::INT_LITERAL: {
            node->typeInfo = new Node(NodeTypekind::INT_LITERAL);
        } break;
        case NodeType::FLOAT_LITERAL: {
            node->typeInfo = new Node(NodeTypekind::FLOAT_LITERAL);
        } break;
        default: assert(false);
    }
}