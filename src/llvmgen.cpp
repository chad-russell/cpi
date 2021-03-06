#include "llvmgen.h"
#include "util.h"
#include "node.h"

#include "llvm/Support/TargetRegistry.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetOptions.h"

#include <unistd.h>
#include <sstream>

void debugValue(void *val) {
    ((llvm::Value *) val)->print(llvm::errs());
}

void debugType(void *ty) {
    ((llvm::Type *) ty)->print(llvm::errs());
}

llvm::Value *LlvmGen::store(llvm::Value *val, llvm::Value *ptr) {
    cpi_assert(val);
    cpi_assert(ptr);

    auto ptrToValType = llvm::PointerType::get(val->getType(), 0);
//    cpi_assert(((llvm::StructType *) val->getType())->isLayoutIdentical((llvm::StructType *) ptr->getType()->getContainedType(0)));

    return builder.CreateStore(val, builder.CreateBitCast(ptr, ptrToValType));
//    return builder.CreateStore(val, ptr);
}

// todo(chad): why this wrapper????
bool needsStorage(Node *node) {
    return hasNoLocalByDefault(node);
}

void LlvmGen::storeIfNeeded(Node *node) {
    if (needsStorage(node) && node->llvmData && node->llvmLocal) {
        store((llvm::Value *) node->llvmData, (llvm::Value *) node->llvmLocal);
    }
}

LlvmGen::LlvmGen(const char *fileName) : builder(context), module(llvm::make_unique<llvm::Module>("module", context)) {
    llvm::InitializeAllTargetInfos();
    llvm::InitializeAllTargets();
    llvm::InitializeAllTargetMCs();
    llvm::InitializeAllAsmParsers();
    llvm::InitializeAllAsmPrinters();

//    voidTy = builder.getVoidTy();
    voidTy = llvm::StructType::get(context, {});

    auto CPU = "generic";
    auto Features = "";

    llvm::TargetOptions opt;
    auto RM = llvm::Optional<llvm::Reloc::Model>();
    auto TargetTriple = llvm::sys::getDefaultTargetTriple();

    module->setTargetTriple(TargetTriple);
    module->addModuleFlag(llvm::Module::Warning, "Dwarf Version", 2);

    std::string Error;
    auto Target = llvm::TargetRegistry::lookupTarget(TargetTriple, Error);
    targetMachine = Target->createTargetMachine(TargetTriple, CPU, Features, opt, RM);
    module->setDataLayout(targetMachine->createDataLayout());

    TheFPM = llvm::make_unique<llvm::legacy::FunctionPassManager>(module.get());

    TheFPM->add(llvm::createInstructionCombiningPass());
    TheFPM->add(llvm::createReassociatePass());
    TheFPM->add(llvm::createGVNPass());
    TheFPM->add(llvm::createCFGSimplificationPass());
    TheFPM->add(llvm::createPromoteMemoryToRegisterPass());

    TheFPM->doInitialization();

    llvm::FunctionType *panicType = llvm::FunctionType::get(voidTy, { builder.getInt8Ty()->getPointerTo() }, false);
    panicFunc = module->getOrInsertFunction("panic", panicType);

    llvm::FunctionType *printfType = llvm::FunctionType::get(builder.getInt32Ty(), { builder.getInt8Ty()->getPointerTo() }, true);
    printfFunc = module->getOrInsertFunction("printf", printfType);
    
    auto fullPath = realpath(fileName, nullptr);
    auto checkPos = 0;
    auto lastSepPos = 0;
    while (fullPath[checkPos] != '\0') {
        if (fullPath[checkPos] == '/') {
            lastSepPos = checkPos;
        }
        checkPos += 1;
    }

    string fullPathString(fullPath);
    auto fileNameWithExtension = fullPathString.substr((unsigned long) lastSepPos + 1, fullPathString.length());
    auto fileBasePath = fullPathString.substr(0, (unsigned long) lastSepPos);
}

void LlvmGen::finalize() {
    verifyModule(*module, &llvm::errs());

    // DO_OPTIMIZE
    for (auto fn : allFns) {
        TheFPM->run(*fn);
    }
}

llvm::Type *LlvmGen::typeFor(Node *node) {
    node = resolve(node);

    cpi_assert(node->type == NodeType::TYPE);
    switch (node->typeData.kind) {
        case NodeTypekind::BOOLEAN_LITERAL: {
            return builder.getInt1Ty();
        }
        case NodeTypekind::U8:
        case NodeTypekind::I8: {
            return builder.getInt8Ty();
        }
        case NodeTypekind::U16:
        case NodeTypekind::I16: {
            return builder.getInt16Ty();
        }
        case NodeTypekind::U32:
        case NodeTypekind::I32: {
            return builder.getInt32Ty();
        }
        case NodeTypekind::INT_LITERAL:
        case NodeTypekind::U64:
        case NodeTypekind::I64: {
            return builder.getInt64Ty();
        }
        case NodeTypekind::FLOAT_LITERAL:
        case NodeTypekind::F32: {
            return builder.getFloatTy();
        }
        case NodeTypekind::F64: {
            return builder.getDoubleTy();
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
                return builder.getInt8PtrTy(0);
            }

            auto underlyingTy = typeFor(node->typeData.pointerTypeData.underlyingType);
            if (underlyingTy == voidTy) {
                return builder.getInt8PtrTy(0);
            }

            return underlyingTy->getPointerTo(0);
        }
        case NodeTypekind::STRUCT: {
            if (node->llvmData) { return (llvm::Type *) node->llvmData; }

            if (node->typeData.structTypeData.coercedType != nullptr) {
                return typeFor(node->typeData.structTypeData.coercedType);
            }

            if (node->typeData.structTypeData.isSecretlyArray) {
                vector<llvm::Type *> elementTypes;
                elementTypes.push_back(typeFor(node->typeData.structTypeData.secretArrayElementType)->getPointerTo(0));
                elementTypes.push_back(builder.getInt64Ty());

                return llvm::StructType::get(context, elementTypes);
            }

            else if (node->typeData.structTypeData.isSecretlyUnion) {
                vector<llvm::Type *> elementTypes;

                // tag
                if (node->typeData.structTypeData.unionTagType != nullptr) {
                    elementTypes.push_back(typeFor(node->typeData.structTypeData.unionTagType));
                }
                else {
                    elementTypes.push_back(builder.getInt64Ty());
                }

                // value size
                uint64_t dataSizeInBytes = 0;
                for (auto param : node->typeData.structTypeData.params) {
                    if (resolve(param->typeInfo)->typeData.kind != NodeTypekind::NONE) {
                        // todo(chad): this might not always be accurate -- should write a routine to consult with llvm...
                        auto potentiallyLarger = (uint64_t) typeSize(param->typeInfo);
                        if (potentiallyLarger > dataSizeInBytes) {
                            dataSizeInBytes = static_cast<uint64_t>(potentiallyLarger);
                        }
                    }
                }

                // value
                elementTypes.push_back(llvm::ArrayType::get(builder.getInt8Ty(), dataSizeInBytes));

                return llvm::StructType::get(context, elementTypes);
            }

            else if (node->llvmData) {
                return (llvm::Type *) node->llvmData;
            }

            auto ty = llvm::StructType::create(context);
            node->llvmData = ty;

            vector<llvm::Type *> elementTypes;
            for (auto param : node->typeData.structTypeData.params) {
                elementTypes.push_back(typeFor(param->typeInfo));
            }
            ty->setBody(elementTypes);

            return ty;
        }
        case NodeTypekind::SYMBOL: {
            return typeFor(resolve(node));
        }
        case NodeTypekind::BOOLEAN: {
            return llvm::IntegerType::getInt1Ty(context);
        }
        case NodeTypekind::NONE: {
            return llvm::StructType::get(context, false);
        }
        case NodeTypekind::ENUM: {
            return typeFor(node->typeData.enumTypeData.type);
        }
        default: cpi_assert(false);
    }

    return nullptr;
}

llvm::Value *LlvmGen::rvalueFor(Node *node) {
    auto resolved = resolve(node);

    if (resolved->typeInfo->typeData.kind == NodeTypekind::NONE) { return llvm::ConstantStruct::get(llvm::StructType::get(context, false), {}); }

    if(resolved->isLocal && resolved->llvmLocal) {
        return builder.CreateLoad((llvm::Value *) resolved->llvmLocal);
    }
    if (resolved->type == NodeType::DOT && resolved->dotData.lhs->isLocal) {
        return builder.CreateLoad((llvm::Value *) resolved->llvmData);
    }

    return (llvm::Value *) resolved->llvmData;
}

void LlvmGen::gen(Node *node) {
    if (node->llvmGen
        && node->type != NodeType::STRING_LITERAL
        && node->type != NodeType::ARRAY_LITERAL
        && node->type != NodeType::STRUCT_LITERAL) {
        return;
    }

    node->llvmGen = true;

    for (auto stmt : node->preStmts) {
        gen(stmt);
    }

    if (node->skipAllButPostStmts) {
        for (auto stmt : node->postStmts) {
            gen(stmt);
        }

        return;
    }

    switch (node->type) {
        case NodeType::FN_DECL: {
            if (node->fnDeclData.ctParams.length != 0 && !node->fnDeclData.cameFromPolymorph) {
                break;
            }

            auto declOnly = node->fnDeclData.body.length == 0;

            // debug info
            auto fnName = node->fnDeclData.name ? atomTable->backwardAtoms[node->fnDeclData.name->symbolData.atomId] : "anon";

            auto savedScope = currentScope;
            auto savedScopeName = currentScopeName;

            std::vector<llvm::Type*> paramTypes = {};
            for (const auto& param : node->fnDeclData.params) {
                cpi_assert(param->type == NodeType::DECL_PARAM);
                paramTypes.push_back(typeFor(param->paramData.type));
            }

            auto returnType = typeFor(node->fnDeclData.returnType);

            auto FT = llvm::FunctionType::get(returnType, paramTypes, false);

            auto *F = llvm::Function::Create(FT, llvm::Function::ExternalLinkage, fnName, module.get());

            F->setCallingConv(llvm::CallingConv::C);
            node->llvmData = F;

            // if it's just a declaration, then we're done
            if (declOnly) {
                return;
            }

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

                // todo(chad): DANGEROUS!!! But I don't know of a better way to deal with the fact that we can have non-typechecked things because of static if, etc.
                if (resolvedLocal->typeInfo == nullptr) { continue; }

                Node *nodeTypeToAlloca = nullptr;
                auto resolvedTypeInfo = resolve(resolvedLocal->typeInfo);
                if (resolvedTypeInfo->type == NodeType::TYPE) {
                    nodeTypeToAlloca = resolve(resolvedLocal->typeInfo);
                } else if (resolvedTypeInfo->type == NodeType::TYPEOF) {
                    // todo(chad): this should probably be disallowed, and we should instead find a way to always have typeInfo resolve to a NodeType::TYPE
                    cpi_assert(resolvedLocal->typeInfo->staticValue != nullptr);
                    nodeTypeToAlloca = resolvedLocal->typeInfo->staticValue;
                } else {
                    cpi_assert(false);
                }

                cpi_assert(nodeTypeToAlloca->type == NodeType::TYPE);

                auto typeToAlloca = typeFor(nodeTypeToAlloca);
                auto shouldCreateAlloca = nodeTypeToAlloca->typeData.kind != NodeTypekind::NONE
                                          && resolvedLocal->type != NodeType::ALIAS;

                if (shouldCreateAlloca) {
                    if (resolvedLocal->type == NodeType::DECL) {
                        if (resolvedLocal->declData.lhs != nullptr) {
                            cpi_assert(resolvedLocal->declData.lhs->type == NodeType::SYMBOL);
                            auto atomId = resolvedLocal->declData.lhs->symbolData.atomId;

                            auto localName = atomTable->backwardAtoms[atomId];
                            resolvedLocal->llvmLocal = builder.CreateAlloca(typeToAlloca, nullptr, localName);
                        } else {
                            resolvedLocal->llvmLocal = builder.CreateAlloca(typeToAlloca, nullptr, "foreach_index");
                        }
                    } else {
                        ostringstream oss("");
                        oss << "local" << resolvedLocal->id << "_";

                        resolvedLocal->llvmLocal = builder.CreateAlloca(typeToAlloca, nullptr, oss.str());
                    }

                    local->llvmLocal = resolvedLocal->llvmLocal;

                    // store any param locals
                    // todo(chad): is this a hacky way to see if the local is a param?
                    for (auto param: node->fnDeclData.params) {
                        if (param == local) {
                            gen(param);
                            store((llvm::Value *) param->llvmData, (llvm::Value *) param->llvmLocal);
                        }
                    }
                }
            }

            auto didTerminate = false;
            for (const auto& stmt : node->fnDeclData.body) {
                if (stmt->type == NodeType::RETURN) { didTerminate = true; }
                gen(stmt);
            }
            if (!didTerminate) {
                builder.CreateRet(llvm::ConstantStruct::get(llvm::StructType::get(context, {}), {}));
            }

            if (currentBB) {
                builder.SetInsertPoint(currentBB, currentIP);
            }

            currentScope = savedScope;
            currentScopeName = savedScopeName;
            currentFnDecl = savedFnDecl;

            allFns.push_back(F);
        } break;
        case NodeType::RETURN: {
            if (node->retData.value != nullptr) {
                gen(node->retData.value);

                auto rvalue = rvalueFor(node->retData.value);

                auto retType = rvalue->getType();
                auto realRetType = typeFor(currentFnDecl->fnDeclData.returnType);

                // todo(chad): @Hack there doesn't seem to be another way to cast things...
                auto realRet = builder.CreateAlloca(realRetType, nullptr, "realRet");
                builder.CreateStore(rvalue, builder.CreateBitCast(realRet, retType->getPointerTo(0)));

                builder.CreateRet(builder.CreateLoad(realRet));
            } else {
                // return {}
                builder.CreateRet(llvm::ConstantStruct::get((llvm::StructType *) voidTy));
            }
        } break;
        case NodeType::INT_LITERAL: {
            if (node->typeInfo->typeData.kind == NodeTypekind::FLOAT_LITERAL
                || node->typeInfo->typeData.kind == NodeTypekind::F32
                || node->typeInfo->typeData.kind == NodeTypekind::F64) {
                node->llvmData = llvm::ConstantFP::get(typeFor(node->typeInfo), (float) node->intLiteralData.value);
            }
            else {
                node->llvmData = llvm::ConstantInt::get(typeFor(node->typeInfo), (uint64_t) node->intLiteralData.value);
            }

            if (node->isLocal && node->llvmLocal) {
                store((llvm::Value *) node->llvmData, (llvm::Value *) node->llvmLocal);
            }
        } break;
        case NodeType::FLOAT_LITERAL: {
            node->llvmData = llvm::ConstantFP::get(typeFor(node->typeInfo), (float) node->floatLiteralData.value);

            if (node->isLocal) {
                store((llvm::Value *) node->llvmData, (llvm::Value *) node->llvmLocal);
            }
        } break;
        case NodeType::NIL_LITERAL: {
            node->llvmData = llvm::ConstantPointerNull::get(builder.getInt8PtrTy(0));

            if (node->isLocal) {
                store((llvm::Value *) node->llvmData, (llvm::Value *) node->llvmLocal);
            }
        } break;
        case NodeType::BOOLEAN_LITERAL: {
            node->llvmData = llvm::ConstantInt::get(typeFor(node->typeInfo), (uint64_t) node->boolLiteralData.value ? 1 : 0);

            if (node->isLocal) {
                store((llvm::Value *) node->llvmData, (llvm::Value *) node->llvmLocal);
            }
        } break;
        case NodeType::DECL: {
            auto data = node->declData;

            if (node->staticValue != nullptr) {
                gen(node->staticValue);
                node->llvmData = node->staticValue->llvmData;
            }
            else if (data.initialValue == nullptr) {
                cpi_assert(node->llvmLocal != nullptr);
            }
            else {
                gen(data.initialValue);

                auto resolvedInitialValue = resolve(data.initialValue);
                gen(resolvedInitialValue);

                if (data.initialValue->typeInfo->typeData.kind != NodeTypekind::NONE) {
                    store(rvalueFor(data.initialValue), (llvm::Value *) node->llvmLocal);
                }
            }
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
            auto argIdx = 0;
            for (auto param : node->fnCallData.params) {
                gen(param);

                auto passedParamType = rvalueFor(param->paramData.value)->getType();
                auto declParamType = typeFor(resolve(vector_at(resolve(node->fnCallData.fn)->typeInfo->typeData.fnTypeData.params, argIdx)->typeInfo));

                // todo(chad): @Hack there doesn't seem to be another way to cast things...
                auto realParam = builder.CreateAlloca(declParamType, nullptr, "realParam");
                auto realParamCasted = builder.CreateBitCast(realParam, passedParamType->getPointerTo(0));
                builder.CreateStore(rvalueFor(param->paramData.value), realParamCasted);

                args.push_back(builder.CreateLoad(realParam));

                argIdx += 1;
            }

            auto resolvedFn = resolve(node->fnCallData.fn);
            gen(resolvedFn);

            auto resolvedFnLlvmValue = rvalueFor(resolvedFn);
            node->llvmData = builder.CreateCall(resolvedFnLlvmValue, args);

            if (node->isLocal && node->typeInfo->typeData.kind != NodeTypekind::NONE) {
                store((llvm::Value *) node->llvmData, (llvm::Value *) node->llvmLocal);
            }
        } break;
        case NodeType::VALUE_PARAM: {
            gen(node->paramData.value);

            node->llvmLocal = node->paramData.value->llvmLocal;
            node->llvmData = node->paramData.value->llvmData;
            node->isLocal = node->paramData.value->isLocal;
        } break;
        case NodeType::BINOP: {
            auto resolved = resolve(node);
            if (resolved != node) {
                gen(resolved);
                return;
            }

            if (node->binopData.type == LexerTokenType::VERTICAL_BAR) {
                gen(node->resolved);
            }
            else if (node->binopData.type == LexerTokenType::AND) {
                // a and b ====> { result := false; if a { if b { result = true; } }

                // initially store false
                store(builder.getInt1(0), (llvm::Value *) node->llvmLocal);

                auto thenBlock = llvm::BasicBlock::Create(context, "then", (llvm::Function *) currentFnDecl->llvmData);
                auto mergeBlock = llvm::BasicBlock::Create(context, "if_cont", (llvm::Function *) currentFnDecl->llvmData);

                gen(node->binopData.lhs);

                builder.CreateCondBr(rvalueFor(node->binopData.lhs), thenBlock, mergeBlock);

                builder.SetInsertPoint(thenBlock);

                // if stmts
                {
                    auto thenBlock2 = llvm::BasicBlock::Create(context, "then_2", (llvm::Function *) currentFnDecl->llvmData);
                    auto mergeBlock2 = llvm::BasicBlock::Create(context, "if_cont_2", (llvm::Function *) currentFnDecl->llvmData);

                    gen(node->binopData.rhs);

                    builder.CreateCondBr(rvalueFor(node->binopData.rhs), thenBlock2, mergeBlock2);

                    builder.SetInsertPoint(thenBlock2);

                    store(builder.getInt1(1), (llvm::Value *) node->llvmLocal);

                    builder.CreateBr(mergeBlock2);
                    builder.SetInsertPoint(mergeBlock2);
                }

                builder.CreateBr(mergeBlock);
                builder.SetInsertPoint(mergeBlock);
            }
            else if (node->binopData.type == LexerTokenType::OR) {
                // a or b ====> { result := false; if a { result = true; } else if b { result = true; }

                // initially store false
                store(builder.getInt1(false), (llvm::Value *) node->llvmLocal);

                auto thenBlock = llvm::BasicBlock::Create(context, "then", (llvm::Function *) currentFnDecl->llvmData);
                auto elseBlock = llvm::BasicBlock::Create(context, "else", (llvm::Function *) currentFnDecl->llvmData);
                auto mergeBlock = llvm::BasicBlock::Create(context, "if_cont", (llvm::Function *) currentFnDecl->llvmData);

                gen(node->binopData.lhs);

                builder.CreateCondBr(rvalueFor(node->binopData.lhs), thenBlock, elseBlock);

                builder.SetInsertPoint(thenBlock);
                {
                    // set to true
                    store(builder.getInt1(true), (llvm::Value *) node->llvmLocal);
                }
                builder.CreateBr(mergeBlock);

                builder.SetInsertPoint(elseBlock);
                {
                    auto thenBlock2 = llvm::BasicBlock::Create(context, "then", (llvm::Function *) currentFnDecl->llvmData);
                    auto mergeBlock2 = llvm::BasicBlock::Create(context, "if_cont", (llvm::Function *) currentFnDecl->llvmData);

                    gen(node->binopData.rhs);

                    builder.CreateCondBr(rvalueFor(node->binopData.rhs), thenBlock2, mergeBlock2);

                    builder.SetInsertPoint(thenBlock2);

                    // if stmts
                    {
                        store(builder.getInt1(true), (llvm::Value *) node->llvmLocal);
                    }

                    builder.CreateBr(mergeBlock2);
                    builder.SetInsertPoint(mergeBlock2);
                }
                builder.CreateBr(mergeBlock);

                builder.SetInsertPoint(mergeBlock);
            }
            else {
                gen(node->binopData.lhs);
                gen(node->binopData.rhs);

                auto lhsValue = rvalueFor(node->binopData.lhs);
                auto rhsValue = rvalueFor(node->binopData.rhs);

                if (node->binopData.lhs->typeInfo->typeData.kind == NodeTypekind::POINTER
                    && node->binopData.rhs->typeInfo->typeData.kind == NodeTypekind::POINTER) {
                    auto lhsIntValue = builder.CreatePtrToInt(lhsValue, builder.getInt64Ty());
                    auto rhsIntValue = builder.CreatePtrToInt(rhsValue, builder.getInt64Ty());

                    switch (node->binopData.type) {
                        case LexerTokenType::EQ_EQ: {
                            node->llvmData = builder.CreateICmpEQ(lhsIntValue, rhsIntValue);
                        } break;
                        case LexerTokenType::NE: {
                            node->llvmData = builder.CreateICmpNE(lhsIntValue, rhsIntValue);
                        } break;
                        case LexerTokenType::GT: {
                            node->llvmData = builder.CreateICmpSGT(lhsIntValue, rhsIntValue);
                        } break;
                        case LexerTokenType::GE: {
                            node->llvmData = builder.CreateICmpSGE(lhsIntValue, rhsIntValue);
                        } break;
                        case LexerTokenType::LT: {
                            node->llvmData = builder.CreateICmpSLT(lhsIntValue, rhsIntValue);
                        } break;
                        case LexerTokenType::LE: {
                            node->llvmData = builder.CreateICmpSLE(lhsIntValue, rhsIntValue);
                        } break;
                        case LexerTokenType::SUB: {
                            node->llvmData = builder.CreateSub(lhsIntValue, rhsIntValue);
                        } break;
                        case LexerTokenType::ADD: {
                            node->llvmData = builder.CreateAdd(lhsIntValue, rhsIntValue);
                        } break;
                        default: cpi_assert(false);
                    }

                    if (node->isLocal) {
                        store((llvm::Value *) node->llvmData, (llvm::Value *) node->llvmLocal);
                    }
                }
                else if (node->binopData.lhs->typeInfo->typeData.kind == NodeTypekind::POINTER) {
                    auto value = builder.CreateGEP(lhsValue, rhsValue, "parith");
                    store(value, (llvm::Value *) node->llvmLocal);
                }
                else if (node->binopData.rhs->typeInfo->typeData.kind == NodeTypekind::POINTER) {
                    auto value = builder.CreateGEP(rhsValue, lhsValue, "parith");
                    store(value, (llvm::Value *) node->llvmLocal);
                }
                else {
                    llvm::Value *value = nullptr;

                    auto isFloat = node->binopData.lhs->typeInfo->typeData.kind == NodeTypekind::FLOAT_LITERAL
                                   || node->binopData.lhs->typeInfo->typeData.kind == NodeTypekind::F32
                                   || node->binopData.lhs->typeInfo->typeData.kind == NodeTypekind::F64;

                    if (isFloat) {
                        switch (node->binopData.type) {
                            case LexerTokenType::ADD: {
                                value = builder.CreateFAdd(lhsValue, rhsValue);
                            } break;
                            case LexerTokenType::DIV: {
                                value = builder.CreateFDiv(lhsValue, rhsValue);
                            } break;
                            case LexerTokenType::SUB: {
                                value = builder.CreateFSub(lhsValue, rhsValue);
                            } break;
                            case LexerTokenType::MUL: {
                                value = builder.CreateFMul(lhsValue, rhsValue);
                            } break;
                            case LexerTokenType::EQ_EQ: {
                                value = builder.CreateFCmpOEQ(lhsValue, rhsValue);
                            } break;
                            case LexerTokenType::NE: {
                                value = builder.CreateFCmpONE(lhsValue, rhsValue);
                            } break;
                            case LexerTokenType::BITAND: {
                                value = builder.CreateAnd(lhsValue, rhsValue);
                            } break;
                            case LexerTokenType::BITOR: {
                                value = builder.CreateOr(lhsValue, rhsValue);
                            } break;
                            case LexerTokenType::BITXOR: {
                                value = builder.CreateXor(lhsValue, rhsValue);
                            } break;
                            case LexerTokenType::MOD: {
                                value = builder.CreateFRem(lhsValue, rhsValue);
                            } break;
                            case LexerTokenType::LT: {
                                value = builder.CreateFCmpOLT(lhsValue, rhsValue);
                            } break;
                            case LexerTokenType::LE: {
                                value = builder.CreateFCmpOLE(lhsValue, rhsValue);
                            } break;
                            case LexerTokenType::GT: {
                                value = builder.CreateFCmpOGT(lhsValue, rhsValue);
                            } break;
                            case LexerTokenType::GE: {
                                value = builder.CreateFCmpOGE(lhsValue, rhsValue);
                            } break;
                            default: cpi_assert(false);
                        }
                    }
                    else {
                        auto kind = node->binopData.lhs->typeInfo->typeData.kind;
                        auto isUnsigned = kind == NodeTypekind::U8
                                          || kind == NodeTypekind::U16
                                          || kind == NodeTypekind::U32
                                          || kind == NodeTypekind::U64;

                        switch (node->binopData.type) {
                            case LexerTokenType::ADD: {
                                value = builder.CreateAdd(lhsValue, rhsValue);
                            } break;
                            case LexerTokenType::DIV: {
                                if (isUnsigned) {
                                    value = builder.CreateUDiv(lhsValue, rhsValue);
                                }
                                else {
                                    value = builder.CreateSDiv(lhsValue, rhsValue);
                                }
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
                            case LexerTokenType::NE: {
                                value = builder.CreateICmpNE(lhsValue, rhsValue);
                            } break;
                            case LexerTokenType::LT: {
                                if (isUnsigned) {
                                    value = builder.CreateICmpULT(lhsValue, rhsValue);
                                }
                                else {
                                    value = builder.CreateICmpSLT(lhsValue, rhsValue);
                                }
                            } break;
                            case LexerTokenType::GT: {
                                if (isUnsigned) {
                                    value = builder.CreateICmpUGT(lhsValue, rhsValue);
                                }
                                else {
                                    value = builder.CreateICmpSGT(lhsValue, rhsValue);
                                }
                            } break;
                            case LexerTokenType::GE: {
                                if (isUnsigned) {
                                    value = builder.CreateICmpUGE(lhsValue, rhsValue);
                                }
                                else {
                                    value = builder.CreateICmpSGE(lhsValue, rhsValue);
                                }
                            } break;
                            case LexerTokenType::LE: {
                                if (isUnsigned) {
                                    value = builder.CreateICmpULE(lhsValue, rhsValue);
                                }
                                else {
                                    value = builder.CreateICmpSLE(lhsValue, rhsValue);
                                }
                            } break;
                            case LexerTokenType::BITAND: {
                                value = builder.CreateAnd(lhsValue, rhsValue);
                            } break;
                            case LexerTokenType::BITOR: {
                                value = builder.CreateOr(lhsValue, rhsValue);
                            } break;
                            case LexerTokenType::BITXOR: {
                                value = builder.CreateXor(lhsValue, rhsValue);
                            } break;
                            case LexerTokenType::BITSHL: {
                                value = builder.CreateShl(lhsValue, rhsValue);
                            } break;
                            case LexerTokenType::BITSHR: {
                                value = builder.CreateAShr(lhsValue, rhsValue);
                            } break;
                            case LexerTokenType::MOD: {
                                if (isUnsigned) {
                                    value = builder.CreateURem(lhsValue, rhsValue);
                                }
                                else {
                                    value = builder.CreateSRem(lhsValue, rhsValue);
                                }
                            } break;
                            default: cpi_assert(false);
                        }
                    }

                    node->llvmData = value;

                    if (node->isLocal && node->llvmLocal) {
                        store(value, (llvm::Value *) node->llvmLocal);
                    }
                }
            }
        } break;
        case NodeType::DECL_PARAM: {
            auto resolvedParam = vector_at(currentFnDecl->fnDeclData.params, node->paramData.index);
            auto fn = static_cast<llvm::Function *>(currentFnDecl->llvmData);
            node->llvmData = &fn->arg_begin()[resolvedParam->paramData.index];

            if (node->isLocal && node->llvmLocal) {
                store((llvm::Value *) node->llvmData, (llvm::Value *) node->llvmLocal);
            }
        } break;
        case NodeType::ADDRESS_OF: {
            if (node->nodeData->type == NodeType::ARRAY_INDEX) {
                auto resolved = resolve(node->nodeData);
                gen(resolved);

                storeIfNeeded(resolved);

                node->llvmData = resolved->llvmData;
                node->llvmLocal = resolved->llvmData;
                node->isLocal = resolved->isLocal;
            }
            else {
                auto resolved = resolve(node->nodeData);
                gen(resolved);

                storeIfNeeded(resolved);
                node->llvmData = resolved->llvmLocal;

                if (node->isLocal && node->llvmLocal) {
                    store((llvm::Value *) node->llvmData, (llvm::Value *) node->llvmLocal);
                }
            }
        } break;
        case NodeType::DEREF: {
            auto resolved = node->nodeData;
            gen(resolved);

            node->llvmData = builder.CreateLoad(rvalueFor(resolved));

            if (node->isLocal && node->llvmLocal) {
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

                auto storageTargetNode = resolve(resolvedDecl->nodeData);
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
                auto paramIndex = (uint64_t) foundParam->paramData.index;

                if (resolve(resolvedDecl->dotData.lhs->typeInfo)->typeData.structTypeData.isSecretlyUnion && paramIndex != 0) {
                    paramIndex = 1;
                }

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

                if (resolvedRhs->typeInfo->typeData.kind != NodeTypekind::NONE) {
                    store(rvalueFor(resolvedRhs), gep);
                }

                node->llvmData = gep;
            } else {
                cpi_assert(false);
            }
        } break;
        case NodeType::DOT: {
            if (node->resolved != nullptr) {
                gen(resolve(node));

                node->llvmData = node->resolved->llvmData;
                break;
            }

            auto foundParam = node->dotData.resolved;

            cpi_assert(foundParam->type == NodeType::DECL_PARAM || foundParam->type == NodeType::VALUE_PARAM);
            uint32_t paramIndex = static_cast<uint32_t>(foundParam->paramData.index);

            auto tagAtom = atomTable->insertStr("tag");
            auto resolvedTypeInfo = resolve(node->dotData.lhs->typeInfo);
            while (resolvedTypeInfo->typeData.kind == NodeTypekind::POINTER) {
                resolvedTypeInfo = resolve(resolvedTypeInfo->typeData.pointerTypeData.underlyingType);
            }

            auto isSecretlyUnion = false;
            if (resolvedTypeInfo->typeData.structTypeData.isSecretlyUnion) {
                isSecretlyUnion = true;

                int64_t tag = foundParam->paramData.name->symbolData.atomId;

                paramIndex = tag == tagAtom ? 0 : 1;
            }

            gen(node->dotData.lhs);

            vector<llvm::Value *> geps;
            geps.push_back(builder.getInt32(0));

            auto resolvedLhs = resolve(node->dotData.lhs);

            llvm::Value *gepTarget = nullptr;
            if (node->dotData.lhs->isLocal && node->dotData.lhs->llvmLocal) {
                gepTarget = (llvm::Value *) resolvedLhs->llvmLocal;
            } else {
                gepTarget = (llvm::Value *) resolvedLhs->llvmData;
            }

            resolvedTypeInfo = resolve(resolvedLhs->typeInfo);

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

                if (isSecretlyUnion && paramIndex == 1) {
                    // bitcast gepTarget to the correct thing...
                    if (resolve(foundParam->typeInfo)->typeData.kind == NodeTypekind::NONE) {
                        gep = builder.CreateBitCast(gep, builder.getInt8PtrTy(0));
                    }
                    else {
                        gep = builder.CreateBitCast(gep, typeFor(foundParam->typeInfo)->getPointerTo(0));
                    }
                }

                node->llvmData = gep;

                if (node->isLocal) {
                    node->llvmLocal = gep;
                }
            } else {
                vector<unsigned int> values = { (unsigned int) paramIndex };
                node->llvmData = builder.CreateExtractValue(gepTarget, values);

                if (isSecretlyUnion && paramIndex == 1) {
                    // bitcast gepTarget to the correct thing...
                    llvm::Type *realType = resolve(foundParam->typeInfo)->typeData.kind == NodeTypekind::NONE
                                           ? (llvm::Type *) builder.getInt8Ty()
                                           : typeFor(foundParam->typeInfo);

                    auto dumbcast = builder.CreateAlloca(realType, nullptr, "dumbcast");
                    builder.CreateStore((llvm::Value *) node->llvmData, builder.CreateBitCast(dumbcast, ((llvm::Value *) node->llvmData)->getType()->getPointerTo(0)));

                    node->llvmLocal = dumbcast;
                    node->llvmData = builder.CreateLoad(dumbcast);
                }
                else if (node->isLocal && node->llvmLocal) {
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

            auto resolvedCondition = resolve(node->ifData.condition);
            gen(resolvedCondition);

            builder.CreateCondBr(rvalueFor(resolvedCondition), thenBlock, elseBlock);

            builder.SetInsertPoint(thenBlock);
            auto didTerminate = false;
            for (auto stmt : node->ifData.stmts) {
                if (stmt->type == NodeType::RETURN) { didTerminate = true; }
                gen(stmt);
            }
            if (!didTerminate) {
                builder.CreateBr(mergeBlock);
            }

            builder.SetInsertPoint(elseBlock);
            didTerminate = false;
            for (auto stmt : node->ifData.elseStmts) {
                if (stmt->type == NodeType::RETURN) { didTerminate = true; }
                gen(stmt);
            }
            if (!didTerminate) {
                builder.CreateBr(mergeBlock);
            }

            builder.SetInsertPoint(mergeBlock);
        } break;
        case NodeType::WHILE: {
            auto condBlock = llvm::BasicBlock::Create(context, "cond", (llvm::Function *) currentFnDecl->llvmData);
            auto thenBlock = llvm::BasicBlock::Create(context, "then", (llvm::Function *) currentFnDecl->llvmData);
            auto mergeBlock = llvm::BasicBlock::Create(context, "if_cont", (llvm::Function *) currentFnDecl->llvmData);

            builder.CreateBr(condBlock);

            builder.SetInsertPoint(condBlock);
            gen(node->whileData.condition);
            builder.CreateCondBr(rvalueFor(node->whileData.condition), thenBlock, mergeBlock);

            builder.SetInsertPoint(thenBlock);
            auto didTerminate = false;
            for (auto stmt : node->whileData.stmts) {
                if (stmt->type == NodeType::RETURN) { didTerminate = true; }
                gen(stmt);
            }
            if (!didTerminate) {
                builder.CreateBr(condBlock);
            }

            builder.SetInsertPoint(mergeBlock);
        } break;
        case NodeType::PANIC: {
            builder.CreateCall(panicFunc, { builder.CreateGlobalStringPtr("assertion failed!!!") });
        } break;
        case NodeType::STRUCT_LITERAL: {
            if (node->typeInfo->typeData.structTypeData.coercedType != nullptr && node->typeInfo->typeData.structTypeData.coercedType->typeData.structTypeData.isSecretlyUnion) {
                // we need to store the tag and the value.
                auto tagIndex = vector_at(node->typeInfo->typeData.structTypeData.params, 0)->paramData.index;

                cpi_assert(node->structLiteralData.params.length == 1);
                auto value = vector_at(node->structLiteralData.params, 0);
                cpi_assert(value->type == NodeType::VALUE_PARAM);

                gen(value->paramData.value);
                auto paramValue = rvalueFor(value->paramData.value);

                auto structType = llvm::StructType::get(builder.getContext(),
                                                        { builder.getInt64Ty(), paramValue->getType() },
                                                        true);
                auto blankSlate = (llvm::Value *) llvm::ConstantStruct::get(structType);

                blankSlate = builder.CreateInsertValue(blankSlate, builder.getInt64(tagIndex), 0);

                auto blankSlateIdxType = structType->getTypeAtIndex(1);
                auto castedValue = builder.CreateBitCast(paramValue, blankSlateIdxType);
                blankSlate = builder.CreateInsertValue(blankSlate, castedValue, 1);

                node->llvmData = blankSlate;

                if (node->isLocal && node->llvmLocal) {
                    store((llvm::Value *) node->llvmData, (llvm::Value *) node->llvmLocal);
                }
            }
            else {
                std::vector<llvm::Value *> values;
                std::vector<llvm::Type *> types;

                unsigned int idx = 0;
                for (auto param : node->structLiteralData.params) {
                    gen(param);

                    auto valueToInsert = rvalueFor(param->paramData.value);
                    auto computedType = typeFor(param->paramData.value->typeInfo);
                    cpi_assert(valueToInsert);

                    if (node->typeInfo->typeData.structTypeData.isSecretlyArray && idx == 0) {
                        valueToInsert = builder.CreateBitCast(valueToInsert,
                                                              typeFor(node->typeInfo->typeData.structTypeData.secretArrayElementType)->getPointerTo(0));
                        computedType = typeFor(node->typeInfo->typeData.structTypeData.secretArrayElementType)->getPointerTo(0);
                    }

                    values.push_back(valueToInsert);
                    types.push_back(valueToInsert->getType());

                    idx += 1;
                }

                auto structType = llvm::StructType::get(builder.getContext(), types);
                auto blankSlate = (llvm::Value *) llvm::ConstantStruct::get(structType);

                idx = 0;
                for (auto value : values) {
                    auto blankSlateIdxType = structType->getTypeAtIndex(idx);

                    auto castedValue = builder.CreateBitCast(value, blankSlateIdxType);
                    blankSlate = builder.CreateInsertValue(blankSlate, castedValue, idx);

                    idx += 1;
                }

                node->llvmData = blankSlate;

                if (node->isLocal && node->llvmLocal) {
                    store((llvm::Value *) node->llvmData, (llvm::Value *) node->llvmLocal);
                }
            }
        } break;
        case NodeType::ARRAY_LITERAL: {
            gen(node->arrayLiteralData.structLiteralRepresentation);

            node->llvmData = node->arrayLiteralData.structLiteralRepresentation->llvmData;
            node->llvmLocal = node->arrayLiteralData.structLiteralRepresentation->llvmLocal;

            if (node->isLocal && node->llvmLocal) {
                store((llvm::Value *) node->llvmData, (llvm::Value *) node->llvmLocal);
            }
        } break;
        case NodeType::CAST: {
            auto resolvedValue = resolve(node->castData.value);

            gen(resolvedValue);
            storeIfNeeded(resolvedValue);

            auto toType = resolve(node->typeInfo);
            auto fromType = resolve(resolvedValue->typeInfo);

            if (isNumericType(fromType) && isNumericType(toType)) {
                auto llvmFromValue = rvalueFor(resolvedValue);
                llvm::Type *llvmToType;

                switch (toType->typeData.kind) {
                    case NodeTypekind::U8:
                    case NodeTypekind::I8: {
                        llvmToType = builder.getInt8Ty();
                    } break;
                    case NodeTypekind::U16:
                    case NodeTypekind::I16: {
                        llvmToType = builder.getInt16Ty();
                    } break;
                    case NodeTypekind::U32:
                    case NodeTypekind::I32: {
                        llvmToType = builder.getInt32Ty();
                    } break;
                    case NodeTypekind::INT_LITERAL:
                    case NodeTypekind::U64:
                    case NodeTypekind::I64: {
                        llvmToType = builder.getInt64Ty();
                    } break;
                    case NodeTypekind::FLOAT_LITERAL:
                    case NodeTypekind::F32: {
                        llvmToType = builder.getFloatTy();
                    } break;
                    case NodeTypekind::F64: {
                        llvmToType = builder.getDoubleTy();
                    } break;
                    default: cpi_assert(false);
                }

                auto kind = node->typeInfo->typeData.kind;
                auto isUnsigned = kind == NodeTypekind::U8
                                  || kind == NodeTypekind::U16
                                  || kind == NodeTypekind::U32
                                  || kind == NodeTypekind::U64;

                if (isFloatType(fromType)) {
                    if (isFloatType(toType)) {
                        node->llvmData = builder.CreateFPCast(llvmFromValue, llvmToType);
                    }
                    else {
                        if (isUnsigned) {
                            node->llvmData = builder.CreateFPToUI(llvmFromValue, llvmToType);
                        }
                        else {
                            node->llvmData = builder.CreateFPToSI(llvmFromValue, llvmToType);
                        }
                    }
                }
                else {
                    if (isFloatType(toType)) {
                        if (isUnsigned) {
                            node->llvmData = builder.CreateUIToFP(llvmFromValue, llvmToType);
                        }
                        else {
                            node->llvmData = builder.CreateSIToFP(llvmFromValue, llvmToType);
                        }
                    }
                    else {
                        node->llvmData = builder.CreateIntCast(llvmFromValue, llvmToType, !isUnsigned);
                    }
                }
            }
            else if (fromType->typeData.kind == NodeTypekind::POINTER && toType->typeData.kind == NodeTypekind::I64) {
                // ptr to int
                node->llvmData = builder.CreatePtrToInt(rvalueFor(resolvedValue), typeFor(node->typeInfo));
            }
            else if (fromType->typeData.kind == NodeTypekind::I64 && toType->typeData.kind == NodeTypekind::POINTER) {
                // int to ptr
                node->llvmData = builder.CreateIntToPtr(rvalueFor(resolvedValue), typeFor(node->typeInfo));
            }
            else {
                node->llvmData = rvalueFor(resolvedValue);
                if (node->llvmData && resolve(node->typeInfo)->typeData.kind == NodeTypekind::POINTER) {
                    node->llvmData = builder.CreateBitCast(rvalueFor(resolvedValue), typeFor(node->typeInfo));
                }
            }

            if (node->llvmData && node->llvmLocal) {
                store((llvm::Value *) node->llvmData, (llvm::Value *) node->llvmLocal);
            }
        } break;
        case NodeType::ARRAY_INDEX: {
            gen(node->resolved);

            node->llvmData = node->resolved->llvmData;
            node->llvmLocal = node->resolved->llvmLocal;
            node->isLocal = node->resolved->isLocal;
        } break;
        case NodeType::STRING_LITERAL: {
            gen(node->stringLiteralData.arrayLiteralRepresentation);
            node->llvmData = node->stringLiteralData.arrayLiteralRepresentation->llvmData;

            if (node->isLocal && node->llvmLocal) {
                store((llvm::Value *) node->llvmData, (llvm::Value *) node->llvmLocal);
            }
        } break;
        case NodeType::UNARY_NEG: {
            gen(node->unaryNegData.target);

            if (node->typeInfo->typeData.kind == NodeTypekind::INT_LITERAL
                || node->typeInfo->typeData.kind == NodeTypekind::I8
                || node->typeInfo->typeData.kind == NodeTypekind::U8
                || node->typeInfo->typeData.kind == NodeTypekind::I16
                || node->typeInfo->typeData.kind == NodeTypekind::U16
                || node->typeInfo->typeData.kind == NodeTypekind::I32
                || node->typeInfo->typeData.kind == NodeTypekind::U32
                || node->typeInfo->typeData.kind == NodeTypekind::I64
                || node->typeInfo->typeData.kind == NodeTypekind::U64) {
                node->llvmData = builder.CreateNeg(rvalueFor(node->unaryNegData.target));
            } else if (node->typeInfo->typeData.kind == NodeTypekind::FLOAT_LITERAL
                       || node->typeInfo->typeData.kind == NodeTypekind::F32
                       || node->typeInfo->typeData.kind == NodeTypekind::F64) {
                node->llvmData = builder.CreateFNeg(rvalueFor(node->unaryNegData.target));
            } else {
                cpi_assert(false);
            }
        } break;
        case NodeType::UNARY_NOT: {
            auto notData = resolve(node->nodeData);

            gen(notData);

            node->llvmData = builder.CreateNot(rvalueFor(notData));
            store((llvm::Value *) node->llvmData, (llvm::Value *) node->llvmLocal);
        } break;
        case NodeType::UNARY_BITNOT: {
            auto notData = resolve(node->nodeData);

            gen(notData);

            node->llvmData = builder.CreateNot(rvalueFor(notData));
            store((llvm::Value *) node->llvmData, (llvm::Value *) node->llvmLocal);
        } break;
        case NodeType::SIZEOF: {
            auto sizeInBits = module->getDataLayout().getTypeSizeInBits(typeFor(node->nodeData));
            auto sizeInBytes = sizeInBits / 8;
            node->llvmData = llvm::ConstantInt::get(builder.getInt64Ty(), sizeInBytes);
        } break;
        case NodeType::FOR: {
            if (node->forData.isStatic) {
                for (auto n : node->forData.staticStmts) {
                    gen(n);
                }
            }
            else {
                for (auto n : node->forData.rewritten) {
                    gen(n);
                }
            }
        } break;
        case NodeType::PUTS: {
            gen(node->nodeData);

            auto firstChar = builder.CreateExtractValue(rvalueFor(node->nodeData), { 0 });
            auto count = builder.CreateExtractValue(rvalueFor(node->nodeData), { 1 });

            auto formatStr = builder.CreateGlobalStringPtr("%.*s", "printfFmtStr");
            builder.CreateCall(printfFunc, { formatStr, count, firstChar });
        } break;
        case NodeType::ISKIND:
        case NodeType::TAGCHECK: {
            cpi_assert(node->resolved != nullptr);
            gen(node->resolved);

            node->llvmData = node->resolved->llvmData;
            if (node->isLocal && node->llvmLocal) {
                store((llvm::Value *) node->llvmData, (llvm::Value *) node->llvmLocal);
            }
        } break;
        case NodeType::FIELDSOF: {
            cpi_assert(node->resolved != nullptr);
            gen(node->resolved);

            node->llvmData = node->resolved->llvmData;
            if (node->isLocal && node->llvmLocal) {
                store((llvm::Value *) node->llvmData, (llvm::Value *) node->llvmLocal);
            }
        } break;
        case NodeType::ALIAS: {
            gen(node->nodeData);
            node->llvmData = node->nodeData->llvmData;
        } break;
        case NodeType::ATTROF: {
            auto resolved = resolve(node);
            gen(resolved);
            node->llvmData = resolved->llvmData;

            if (node->isLocal) {
                store((llvm::Value *) node->llvmData, (llvm::Value *) node->llvmLocal);
            }
        } break;
        case NodeType::HASATTR: {
            auto resolved = resolve(node);
            gen(resolved);
            node->llvmData = resolved->llvmData;

            if (node->isLocal) {
                store((llvm::Value *) node->llvmData, (llvm::Value *) node->llvmLocal);
            }
        } break;
        case NodeType::CREATE_CONTEXT: {
            auto resolved = resolve(node);

            gen(resolved);
            node->llvmData = resolved->llvmData;
        } break;
        case NodeType::DEFER:
        case NodeType::END_SCOPE:
        case NodeType::IMPORT:
        case NodeType::LINK:
        case NodeType::PARAMETERIZED_TYPE:
        case NodeType::TYPEOF:
        case NodeType::ATTR:
        case NodeType::MODULE: {
            // nothing to do
        } break;
        default: cpi_assert(false);
    }

    for (auto stmt : node->postStmts) {
        gen(stmt);
    }
}
