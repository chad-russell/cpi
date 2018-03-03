#include "llvmgen.h"
#include "util.h"
#include "node.h"

void debugValue(llvm::Value *val) {
    val->print(llvm::errs());
}

llvm::Value *LlvmGen::store(llvm::Value *val, llvm::Value *ptr) {
    assert(val);
    assert(ptr);

    auto ptrToValType = llvm::PointerType::get(val->getType(), 0);
//    assert(((llvm::StructType *) val->getType())->isLayoutIdentical((llvm::StructType *) ptr->getType()->getContainedType(0)));

    return builder.CreateStore(val, builder.CreateBitCast(ptr, ptrToValType));
//    return builder.CreateStore(val, ptr);
}

bool needsStorage(Node *node) {
    // otherwise if it's constant (doesn't have own local by default), then we should store it
    return hasNoLocalByDefault(node);
}

void LlvmGen::storeIfNeeded(Node *node) {
    if (needsStorage(node) && node->llvmData && node->llvmLocal) {
        store((llvm::Value *) node->llvmData, (llvm::Value *) node->llvmLocal);
    }
}

LlvmGen::LlvmGen() : builder(context), module(llvm::make_unique<llvm::Module>("module", context)) {
    TheFPM = llvm::make_unique<llvm::legacy::FunctionPassManager>(module.get());

    TheFPM->add(llvm::createInstructionCombiningPass());
    TheFPM->add(llvm::createReassociatePass());
    TheFPM->add(llvm::createGVNPass());
    TheFPM->add(llvm::createCFGSimplificationPass());
    TheFPM->add(llvm::createPromoteMemoryToRegisterPass());

    TheFPM->doInitialization();

    llvm::FunctionType *panicType = llvm::FunctionType::get(builder.getVoidTy(), { builder.getInt8Ty()->getPointerTo() }, false);
    panicFunc = module->getOrInsertFunction("panic", panicType);
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
            if (node->typeData.pointerTypeData.underlyingType == nullptr) {
                return llvm::PointerType::getVoidTy(context);
            }

            return llvm::PointerType::get(typeFor(node->typeData.pointerTypeData.underlyingType), 0);
        }
        case NodeTypekind::STRUCT: {
            if (node->llvmData) { return (llvm::Type *) node->llvmData; }

            auto ty = llvm::StructType::create(context);
            node->llvmData = ty;

            vector<llvm::Type *> elementTypes;
            for (auto param : node->typeData.structTypeData.params) {
                elementTypes.push_back(typeFor(param->typeInfo));
            }
            ty->setBody(elementTypes, true);

            return ty;
        }
        case NodeTypekind::SYMBOL: {
            return typeFor(resolve(node));
        }
        case NodeTypekind::BOOLEAN: {
            return llvm::IntegerType::getInt1Ty(context);
        }
        case NodeTypekind::NONE: {
            return llvm::Type::getVoidTy(context);
        }
        case NodeTypekind::ARRAY: {
            llvm::ArrayRef<llvm::Type *> elements = {
                    // data pointer
                    llvm::PointerType::get(typeFor(node->typeData.arrayTypeData.elementType), 0),
                    // count
                    builder.getInt32Ty()
            };

            return llvm::StructType::get(context, elements, true);
        }
        default: assert(false);
    }
}

llvm::Value *LlvmGen::rvalueFor(Node *node) {
    auto resolved = resolve(node);

    if (resolved->isLocal) {
        return builder.CreateLoad((llvm::Value *) resolved->llvmLocal);
    }
    else if (resolved->type == NodeType::DOT && resolved->dotData.lhs->isLocal) {
        return builder.CreateLoad((llvm::Value *) resolved->llvmData);
    }

    return (llvm::Value *) resolved->llvmData;
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

                Node *nodeTypeToAlloca = nullptr;
                if (resolvedLocal->typeInfo->type == NodeType::TYPE) {
                    nodeTypeToAlloca = resolvedLocal->typeInfo;
                } else if (resolvedLocal->typeInfo->type == NodeType::TYPEOF) {
                    // todo(chad): this should probably be disallowed, and we should instead find a way to always have typeInfo be NodeType::TYPE
                    assert(resolvedLocal->typeInfo->staticValue);
                    nodeTypeToAlloca = resolvedLocal->typeInfo->staticValue;
                } else {
                    assert(false);
                }
                auto typeToAlloca = typeFor(nodeTypeToAlloca);

                auto shouldCreateAlloca = nodeTypeToAlloca->typeData.kind != NodeTypekind::NONE && !resolvedLocal->isAutoDerefStorage;
                if (shouldCreateAlloca) {
                    if (resolvedLocal->type == NodeType::DECL) {
                        assert(resolvedLocal->declData.lvalue->type == NodeType::SYMBOL);
                        auto atomId = resolvedLocal->declData.lvalue->symbolData.atomId;

                        resolvedLocal->llvmLocal = builder.CreateAlloca(typeToAlloca, nullptr, AtomTable::current->backwardAtoms[atomId]);
                    } else {
                        resolvedLocal->llvmLocal = builder.CreateAlloca(typeToAlloca, nullptr, "local");
                    }
                }
            }

            auto didTerminate = false;
            for (const auto& stmt : node->fnDeclData.body) {
                if (stmt->type == NodeType::RET || stmt->type == NodeType::PANIC) { didTerminate = true; }
                gen(stmt);
            }
            if (!didTerminate) {
                builder.CreateRetVoid();
            }

            if (currentBB) {
                builder.SetInsertPoint(currentBB, currentIP);
            }

            currentFnDecl = savedFnDecl;

            verifyFunction(*F, &llvm::errs());

            // DO_OPTIMIZE
//            TheFPM->run(*F);
        } break;
        case NodeType::RET: {
            gen(node->retData.value);
            builder.CreateRet(rvalueFor(node->retData.value));
        } break;
        case NodeType::INT_LITERAL: {
            node->llvmData = llvm::ConstantInt::get(typeFor(node->typeInfo), (uint64_t) node->intLiteralData.value);
        } break;
        case NodeType::BOOLEAN_LITERAL: {
            node->llvmData = llvm::ConstantInt::get(typeFor(node->typeInfo), (uint64_t) node->boolLiteralData.value ? 1 : 0);
        } break;
        case NodeType::DECL: {
            auto data = node->declData;
            if (data.initialValue == nullptr) { return; }

            gen(data.initialValue);

            auto resolvedInitialValue = resolve(data.initialValue);
            gen(resolvedInitialValue);

            store(rvalueFor(data.initialValue), (llvm::Value *) node->llvmLocal);
        } break;
        case NodeType::SYMBOL: {
            auto resolved = resolve(node);
            gen(resolved);

            node->llvmData = resolved->llvmData;
            node->llvmLocal = resolved->llvmLocal;

            if (node->isLocal && resolved->type != NodeType::DECL) {
                store((llvm::Value *) node->llvmData, (llvm::Value *) node->llvmLocal);
            }

            node->isLocal = resolved->isLocal;
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

            node->llvmData = builder.CreateCall(rvalueFor(resolvedFn), args);

            if (node->isLocal && node->typeInfo->typeData.kind != NodeTypekind::NONE) {
                store((llvm::Value *) node->llvmData, (llvm::Value *) node->llvmLocal);
            }
        } break;
        case NodeType::VALUE_PARAM: {
            gen(node->valueParamData.value);

            node->llvmLocal = node->valueParamData.value->llvmLocal;
            node->llvmData = node->valueParamData.value->llvmData;
            node->isLocal = node->valueParamData.value->isLocal;
        } break;
        case NodeType::BINOP: {
            gen(node->binopData.lhs);
            gen(node->binopData.rhs);

            storeIfNeeded(node->binopData.lhs);
            storeIfNeeded(node->binopData.rhs);

            auto lhsValue = rvalueFor(node->binopData.lhs);
            auto rhsValue = rvalueFor(node->binopData.rhs);

            llvm::Type *pointerType = nullptr;

            if (node->binopData.lhs->typeInfo->typeData.kind == NodeTypekind::POINTER) {
                pointerType = lhsValue->getType();
                lhsValue = builder.CreatePtrToInt(lhsValue, builder.getInt64Ty());
            }

            if (node->binopData.rhs->typeInfo->typeData.kind == NodeTypekind::POINTER) {
                pointerType = rhsValue->getType();
                rhsValue = builder.CreatePtrToInt(rhsValue, builder.getInt64Ty());
            }

            llvm::Value *value = nullptr;
            switch (node->binopData.type) {
                case LexerTokenType::ADD: {
                    value = builder.CreateAdd(lhsValue, rhsValue);
                } break;
                case LexerTokenType::DIV: {
                    value = builder.CreateSDiv(lhsValue, rhsValue);
                } break;
                case LexerTokenType::SUB: {
                    value = builder.CreateSub(lhsValue, rhsValue);
                } break;
                case LexerTokenType::MUL: {
                    value = builder.CreateMul(lhsValue, rhsValue);
                } break;
                case LexerTokenType::EQ_EQ: {
                    value = builder.CreateICmpEQ(lhsValue, rhsValue);
                } break;
                default: assert(false);
            }

            if (pointerType != nullptr) {
                value = builder.CreateIntToPtr(value, pointerType);
            }

            store(value, (llvm::Value *) node->llvmLocal);
        } break;
        case NodeType::DECL_PARAM: {
            auto resolvedParam = currentFnDecl->fnDeclData.params[node->declParamData.index];
            auto fn = static_cast<llvm::Function *>(currentFnDecl->llvmData);
            node->llvmData = &fn->arg_begin()[resolvedParam->declParamData.index];

            if (node->isLocal) {
                store((llvm::Value *) node->llvmData, (llvm::Value *) node->llvmLocal);
            }
        } break;
        case NodeType::ADDRESS_OF: {
            auto resolved = resolve(node->nodeData);

            gen(resolved);

            storeIfNeeded(resolved);

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
                storeIfNeeded(resolvedRhs);

                auto storageTargetNode = resolve(resolvedDecl->derefData.target);
                gen(storageTargetNode);

                auto storageTarget = (llvm::Value *) storageTargetNode->llvmLocal;
                if (storageTargetNode->type == NodeType::DECL_PARAM) {
                    storageTarget = (llvm::Value *) storageTargetNode->llvmData;
                    node->llvmData = store(rvalueFor(resolvedRhs), storageTarget);
                } else {
                    node->llvmData = store(rvalueFor(resolvedRhs), builder.CreateLoad(storageTarget));
                }
            } else if (resolvedDecl->type == NodeType::DOT) {
                // store rvalue into its slot
                storeIfNeeded(resolvedRhs);

                auto foundParam = resolvedDecl->dotData.resolved;
                auto paramIndex = (uint64_t) foundParam->declParamData.index;

                gen(resolvedDecl->dotData.lhs);

                auto gepTarget = (llvm::Value *) resolvedDecl->dotData.lhs->llvmLocal;

                vector<llvm::Value *> geps;
                geps.push_back(builder.getInt32(0));

                auto resolvedTypeInfo = resolve(resolve(resolvedDecl->dotData.lhs)->typeInfo);
                auto pointerCount = 0;
                while (resolvedTypeInfo->typeData.kind == NodeTypekind::POINTER) {
                    resolvedTypeInfo = resolve(resolvedTypeInfo->typeData.pointerTypeData.underlyingType);
                    pointerCount += 1;
                }
                for (auto i = 0; i < pointerCount; i++) {
                    gepTarget = builder.CreateLoad(gepTarget);
                }

                geps.push_back(builder.getInt32((uint32_t) paramIndex));
                auto gep = builder.CreateGEP(gepTarget, geps);
                store(rvalueFor(resolvedRhs), gep);
                node->llvmData = gep;
            } else {
                assert(false);
            }
        } break;
        case NodeType::DOT: {
            auto foundParam = node->dotData.resolved;

            uint64_t paramIndex;
            if (foundParam->type == NodeType::DECL_PARAM) {
                paramIndex = (uint64_t) foundParam->declParamData.index;
            } else if (foundParam->type == NodeType::VALUE_PARAM) {
                paramIndex = (uint64_t) foundParam->valueParamData.index;
            } else {
                assert(false);
            }

            gen(node->dotData.lhs);

            vector<llvm::Value *> geps;
            geps.push_back(builder.getInt32(0));

            auto resolvedLhs = resolve(node->dotData.lhs);

            llvm::Value *gepTarget;
            if (node->dotData.lhs->isLocal) {
                gepTarget = (llvm::Value *) resolvedLhs->llvmLocal;
            } else {
                gepTarget = (llvm::Value *) resolvedLhs->llvmData;
            }

            auto resolvedTypeInfo = resolve(resolvedLhs->typeInfo);
            auto pointerCount = 0;
            while (resolvedTypeInfo->typeData.kind == NodeTypekind::POINTER) {
                resolvedTypeInfo = resolve(resolvedTypeInfo->typeData.pointerTypeData.underlyingType);
                pointerCount += 1;
            }
            for (auto i = 0; i < pointerCount; i++) {
                gepTarget = builder.CreateLoad(gepTarget);
            }

            geps.push_back(builder.getInt32(paramIndex));

            if (node->dotData.lhs->isLocal) {
                auto gep = builder.CreateGEP(gepTarget, geps);
                node->llvmData = gep;

                if (node->isLocal) {
                    node->llvmLocal = gep;
                }
            } else {
                vector<unsigned int> values = { (unsigned int) paramIndex };
                node->llvmData = builder.CreateExtractValue(gepTarget, values);

                if (node->isLocal) {
                    store((llvm::Value *) node->llvmData, (llvm::Value *) node->llvmLocal);
                }
            }
        } break;
        case NodeType::TYPE: {
            // nothing to do
        } break;
        case NodeType::IF: {
            auto thenBlock = llvm::BasicBlock::Create(context, "then", (llvm::Function *) currentFnDecl->llvmData);
            auto elseBlock = llvm::BasicBlock::Create(context, "else", (llvm::Function *) currentFnDecl->llvmData);
            auto mergeBlock = llvm::BasicBlock::Create(context, "if_cont", (llvm::Function *) currentFnDecl->llvmData);

            gen(node->ifData.condition);

            builder.CreateCondBr(rvalueFor(node->ifData.condition), thenBlock, elseBlock);

            builder.SetInsertPoint(thenBlock);
            auto didTerminate = false;
            for (auto stmt : node->ifData.stmts) {
                if (stmt->type == NodeType::RET || stmt->type == NodeType::PANIC) { didTerminate = true; }
                gen(stmt);
            }
            if (!didTerminate) {
                builder.CreateBr(mergeBlock);
            }

            builder.SetInsertPoint(elseBlock);
            didTerminate = false;
            for (auto stmt : node->ifData.elseStmts) {
                if (stmt->type == NodeType::RET || stmt->type == NodeType::PANIC) { didTerminate = true; }
                gen(stmt);
            }
            if (!didTerminate) {
                builder.CreateBr(mergeBlock);
            }

            builder.SetInsertPoint(mergeBlock);
        } break;
        case NodeType::PANIC: {
            builder.CreateCall(panicFunc, { builder.CreateGlobalStringPtr("assertion failed!!!") });
            builder.CreateRet(builder.getInt64(0));
        } break;
        case NodeType::STRUCT_LITERAL: {
            auto structType = (llvm::StructType *) typeFor(node->typeInfo);
            auto blankSlate = (llvm::Value *) llvm::ConstantStruct::get(structType);

            unsigned int idx = 0;
            for (auto param : node->structLiteralData.params) {
                gen(param);

                auto valueToInsert = rvalueFor(param);
                assert(valueToInsert);

                blankSlate = builder.CreateInsertValue(blankSlate, valueToInsert, idx);

                idx += 1;
            }

            node->llvmData = blankSlate;

            if (node->isLocal) {
                store((llvm::Value *) node->llvmData, (llvm::Value *) node->llvmLocal);
            }
        } break;
        case NodeType::CAST: {
            gen(node->castData.value);

            if (node->castData.isCastFromArrayToDataPtr) {
                auto arrayStruct = rvalueFor(node->castData.value);

                node->llvmData = builder.CreateExtractValue(arrayStruct, 0);
                node->llvmLocal = nullptr;
                node->isLocal = false;
            }
            else {
                node->llvmData = (llvm::Value *) node->castData.value->llvmData;
                if (node->llvmData && resolve(node->castData.type)->typeData.kind == NodeTypekind::POINTER) {
                    node->llvmData = builder.CreateBitCast((llvm::Value *) node->llvmData, typeFor(node->castData.type));
                }

                node->llvmLocal = (llvm::Value *) node->castData.value->llvmLocal;
                if (node->llvmLocal && resolve(node->castData.type)->typeData.kind == NodeTypekind::POINTER) {
                    node->llvmLocal = builder.CreateBitCast((llvm::Value *) node->llvmLocal, typeFor(node->castData.type)->getPointerTo(0));
                }
                node->isLocal = node->castData.value->isLocal;
            }
        } break;
        case NodeType::ARRAY_INDEX: {
            gen(node->resolved);

            node->llvmData = node->resolved->llvmData;
            node->llvmLocal = node->resolved->llvmLocal;
            node->isLocal = node->resolved->isLocal;
        } break;
        default: assert(false);
    }
}
