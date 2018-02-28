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
        case NodeTypekind::STRUCT: {
            vector<llvm::Type *> elementTypes;
            for (auto param : node->typeData.structTypeData.params) {
                elementTypes.push_back(typeFor(param->typeInfo));
            }

            return llvm::StructType::get(context, elementTypes, false);
        }
        case NodeTypekind::SYMBOL: {
            return typeFor(resolve(node));
        }
        default: assert(false);
    }
}

llvm::Value *LlvmGen::rvalueFor(Node *node) {
    auto resolved = resolve(node);

    if (resolved->isLocal) {
        return builder.CreateLoad((llvm::Value *) resolved->llvmLocal);
    }
    else if (resolved->type == NodeType::DOT && resolve(resolved->dotData.lhs)->isLocal) {
        return builder.CreateLoad((llvm::Value *) resolved->llvmData);
    }

    return (llvm::Value *) resolved->llvmData;
}

llvm::Value *LlvmGen::store(llvm::Value *val, llvm::Value *ptr) {
    return builder.CreateStore(val, ptr);
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

            // DO_OPTIMIZE
            TheFPM->run(*F);
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

            store(rvalueFor(data.initialValue), (llvm::Value *) node->llvmData);
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
                args.push_back(rvalueFor(param->valueParamData.value));
            }

            auto resolvedFn = resolve(node->fnCallData.fn);
            gen(resolvedFn);

            auto callInst = builder.CreateCall(rvalueFor(resolvedFn), args);

            store((llvm::Value *) callInst, (llvm::Value *) node->llvmData);
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

            store(value, (llvm::Value *) node->llvmData);
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
                store((llvm::Value *) resolved->llvmData, (llvm::Value *) resolved->llvmLocal);
            }

            node->llvmData = resolved->llvmLocal;

            if (node->isLocal) {
                store((llvm::Value *) node->llvmData, (llvm::Value *) node->llvmLocal);
            }
        } break;
        case NodeType::DEREF: {
            auto resolved = node->derefData.target;
            gen(resolved);

            node->llvmData = builder.CreateLoad(rvalueFor(resolved));

            if (node->isLocal) {
                store((llvm::Value *) node->llvmData, (llvm::Value *) node->llvmLocal);
            }
        } break;
        case NodeType::ASSIGN: {
            auto resolvedLhs = resolve(node->assignData.lhs);
            auto resolvedRhs = resolve(node->assignData.rhs);

            gen(resolvedRhs);

            auto resolvedDecl = resolve(resolvedLhs);
            if (resolvedDecl->type == NodeType::DECL) {
                gen(resolvedLhs);
                node->llvmData = store(rvalueFor(resolvedRhs), (llvm::Value *) resolvedLhs->llvmLocal);
            } else if (resolvedDecl->type == NodeType::DEREF) {
                // store rvalue into its slot
                store((llvm::Value *) resolvedRhs->llvmData, (llvm::Value *) resolvedRhs->llvmLocal);

                auto storageTargetNode = resolve(node->assignData.lhs->derefData.target);
                gen(storageTargetNode);

                auto storageTarget = (llvm::Value *) storageTargetNode->llvmLocal;

                node->llvmData = store(rvalueFor(resolvedRhs), builder.CreateLoad(storageTarget));
            } else if (resolvedDecl->type == NodeType::DOT) {
                // store rvalue into its slot
                store((llvm::Value *) resolvedRhs->llvmData, (llvm::Value *) resolvedRhs->llvmLocal);

                auto foundParam = resolvedDecl->dotData.resolved;
                auto paramIndex = (uint64_t) foundParam->declParamData.index;

                gen(resolvedDecl->dotData.lhs);

                assert(resolve(resolvedDecl->dotData.lhs)->isLocal);
                auto gep = builder.CreateGEP((llvm::Value *) resolvedDecl->dotData.lhs->llvmData, {
                        llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), 0),
                        llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), paramIndex)
                });

                store(rvalueFor(resolvedRhs), gep);
                node->llvmData = gep;
            } else {
                assert(false);
            }
        } break;
        case NodeType::DOT: {
            auto foundParam = node->dotData.resolved;
            auto paramIndex = (uint64_t) foundParam->declParamData.index;

            gen(node->dotData.lhs);

            if (resolve(node->dotData.lhs)->isLocal) {
                auto gep = builder.CreateGEP((llvm::Value *) node->dotData.lhs->llvmData, {
                        llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), 0),
                        llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), paramIndex)
                });
                node->llvmData = gep;

                if (node->isLocal) {
                    store(builder.CreateLoad(gep), (llvm::Value *) node->llvmLocal);
                }
            } else {
//                ((llvm::Value *) node->dotData.lhs->llvmData)->print(llvm::errs());
                vector<unsigned int> values = { (unsigned int) paramIndex };
                node->llvmData = builder.CreateExtractValue((llvm::Value *) node->dotData.lhs->llvmData, values);

                if (node->isLocal) {
                    store((llvm::Value *) node->llvmData, (llvm::Value *) node->llvmLocal);
                }
            }
        } break;
        default: assert(false);
    }
}
