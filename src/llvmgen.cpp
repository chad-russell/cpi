#include "llvmgen.h"
#include "util.h"
#include "node.h"

LlvmGen::LlvmGen() : builder(context), module(llvm::make_unique<llvm::Module>("module", context)) {
    TheFPM = llvm::make_unique<llvm::legacy::FunctionPassManager>(module.get());

    TheFPM->add(llvm::createInstructionCombiningPass());
    TheFPM->add(llvm::createReassociatePass());
    TheFPM->add(llvm::createGVNPass());
    TheFPM->add(llvm::createCFGSimplificationPass());
    TheFPM->add(llvm::createPromoteMemoryToRegisterPass());

    TheFPM->doInitialization();
}

llvm::Type *LlvmGen::typeFor(Node *node) {
    assert(node->type == NodeType::TYPE);
    switch (node->typeData.kind) {
        case NodeTypekind::I32: {
            return llvm::Type::getInt32Ty(context);
        }
        case NodeTypekind::INT_LITERAL:
        case NodeTypekind::I64: {
            return llvm::Type::getInt64Ty(context);
        }
        case NodeTypekind::FN: {
            auto resultType = typeFor(node->typeData.fnTypeData.returnType);

            vector<llvm::Type *> paramTypes;
            for (auto param : node->typeData.fnTypeData.params) {
                paramTypes.push_back(typeFor(param->typeInfo));
            }

            auto fnType = llvm::FunctionType::get(resultType, paramTypes, false);
            return llvm::PointerType::get(fnType, 0);
        }
        case NodeTypekind::POINTER: {
            return llvm::PointerType::get(typeFor(node->typeData.pointerTypeData.underlyingType), 0);
        }
        default: assert(false);
    }
}

llvm::Value *LlvmGen::rvalueFor(Node *node) {
    auto resolved = resolve(node);

    if (resolved->isLocal) {
        return builder.CreateLoad((llvm::Value *) resolved->llvmLocal);
    }

    return (llvm::Value *) resolved->llvmData;
}

void LlvmGen::setLocal(Node *a, Node *b) {
    assert(a->isLocal);

    if (b->isLocal) {
        a->llvmLocal = b->llvmLocal;
    } else {
        builder.CreateStore((llvm::Value *) b->llvmData, (llvm::Value *) a->llvmLocal);
    }
}

void LlvmGen::gen(Node *node) {
    if (node->llvmGen) { return; }
    node->llvmGen = true;

    switch (node->type) {
        case NodeType::FN_DECL: {
            std::vector<llvm::Type*> paramTypes = {};
            for (const auto& param : node->fnDeclData.params) {
                assert(param->type == NodeType::DECL_PARAM);
                paramTypes.push_back(typeFor(param->declParamData.type));
            }

            auto returnType = typeFor(node->fnDeclData.returnType);

            auto FT = llvm::FunctionType::get(returnType, paramTypes, false);
            auto *F = llvm::Function::Create(FT,
                                           llvm::Function::ExternalLinkage,
                                           node->fnDeclData.name ? AtomTable::current->backwardAtoms[node->fnDeclData.name->symbolData.atomId] : "anon",
                                           module.get());

            node->llvmData = F;

            auto currentBB = builder.GetInsertBlock();
            auto currentIP = builder.GetInsertPoint();

            auto BB = llvm::BasicBlock::Create(context, "entry", F);
            builder.SetInsertPoint(BB);

            auto savedFnDecl = currentFnDecl;
            currentFnDecl = node;

            // alloca all locals
            for (auto local : node->fnDeclData.locals) {
                auto resolvedLocal = resolve(local);
                if (resolvedLocal != local && resolvedLocal->isLocal) { continue; }

                // todo(chad): get the name based on node type, include that in the alloca for convenience
                resolvedLocal->llvmData = builder.CreateAlloca(typeFor(resolvedLocal->typeInfo));
                resolvedLocal->llvmLocal = resolvedLocal->llvmData;
            }

            for (const auto& stmt : node->fnDeclData.body) {
                gen(stmt);
            }

            if (currentBB) {
                builder.SetInsertPoint(currentBB, currentIP);
            }

            currentFnDecl = savedFnDecl;

//            TheFPM->run(*F);
        } break;
        case NodeType::RET: {
            gen(node->retData.value);
            builder.CreateRet(rvalueFor(node->retData.value));
        } break;
        case NodeType::INT_LITERAL: {
            node->llvmData = llvm::ConstantInt::get(typeFor(node->typeInfo), (uint64_t) node->intLiteralData.value);
        } break;
        case NodeType::DECL: {
            auto data = node->declData;
            if (data.initialValue == nullptr) { return; }

            gen(data.initialValue);

            auto resolvedInitialValue = resolve(data.initialValue);
            gen(resolvedInitialValue);

            builder.CreateStore((llvm::Value *) data.initialValue->llvmData, (llvm::Value *) node->llvmData);
        } break;
        case NodeType::SYMBOL: {
            auto resolved = resolve(node);
            gen(resolved);

            node->llvmData = resolved->llvmData;
        } break;
        case NodeType::FN_CALL: {
            if (!node->fnCallData.hasRuntimeParams) { break; }

            vector<llvm::Value *> args;
            for (auto param : node->fnCallData.params) {
                gen(param);
                args.push_back((llvm::Value *) param->llvmData);
            }

            auto resolvedFn = resolve(node->fnCallData.fn);
            gen(resolvedFn);

            auto callInst = builder.CreateCall(rvalueFor(resolvedFn), args);

            builder.CreateStore((llvm::Value *) callInst, (llvm::Value *) node->llvmData);
        } break;
        case NodeType::VALUE_PARAM: {
            gen(node->valueParamData.value);
            node->llvmData = node->valueParamData.value->llvmData;
        } break;
        case NodeType::BINOP: {
            gen(node->binopData.lhs);
            gen(node->binopData.rhs);

            llvm::Value *value = nullptr;
            switch (node->binopData.type) {
                case LexerTokenType::ADD: {
                    value = builder.CreateAdd(rvalueFor(node->binopData.lhs), rvalueFor(node->binopData.rhs));
                } break;
                default: assert(false);
            }

            builder.CreateStore(value, (llvm::Value *) node->llvmData);
        } break;
        case NodeType::DECL_PARAM: {
            auto resolvedParam = currentFnDecl->fnDeclData.params[node->declParamData.index];
            auto fn = static_cast<llvm::Function *>(currentFnDecl->llvmData);
            node->llvmData = &fn->arg_begin()[resolvedParam->declParamData.index];
        } break;
        case NodeType::ADDRESS_OF: {
            auto resolved = resolve(node->nodeData);

            gen(resolved);

            if (isConstant(resolved)) {
                builder.CreateStore((llvm::Value *) resolved->llvmData, (llvm::Value *) resolved->llvmLocal);
            }

            node->llvmData = resolved->llvmLocal;

            if (node->isLocal) {
                builder.CreateStore((llvm::Value *) node->llvmData, (llvm::Value *) node->llvmLocal);
            }
        } break;
        case NodeType::DEREF: {
            auto resolved = node->derefData.target;
            gen(resolved);

            node->llvmData = builder.CreateLoad(rvalueFor(resolved));

            if (node->isLocal) {
                builder.CreateStore((llvm::Value *) node->llvmData, (llvm::Value *) node->llvmLocal);
            }
        } break;
        default: assert(false);
    }
}
