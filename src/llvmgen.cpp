#include "llvmgen.h"
#include "util.h"
#include "node.h"

#include "llvm/Support/TargetRegistry.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetOptions.h"

#include <unistd.h>

#define DBUILDER 1

void debugValue(void *val) {
    ((llvm::Value *) val)->print(llvm::errs());
}

void debugType(void *ty) {
    ((llvm::Type *) ty)->print(llvm::errs());
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

    voidTy = builder.getVoidTy();

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

    llvm::FunctionType *mallocType = llvm::FunctionType::get(builder.getInt8Ty()->getPointerTo(), { builder.getInt64Ty() }, false);
    mallocFunc = module->getOrInsertFunction("malloc", mallocType);

    llvm::FunctionType *freeType = llvm::FunctionType::get(voidTy, { builder.getInt8Ty()->getPointerTo() }, false);
    freeFunc = module->getOrInsertFunction("free", freeType);

    if (DBUILDER) {
        dBuilder = new llvm::DIBuilder(*module);
    }

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

    if (DBUILDER) {
        diCu = dBuilder->createCompileUnit(llvm::dwarf::DW_LANG_C, dBuilder->createFile(fileNameWithExtension, fileBasePath), "cpi", false, "", 0);
    }

    currentScope = diCu;
    currentScopeName = "diCu";

    // Add the current debug info version into the module.
    module->addModuleFlag(llvm::Module::Warning, "Debug Info Version", llvm::DEBUG_METADATA_VERSION);
}

void emitDebugLocation(LlvmGen *gen, Node *node) {
#if DBUILDER
    assert(gen->currentScope != nullptr);
    gen->builder.SetCurrentDebugLocation(llvm::DebugLoc::get(static_cast<unsigned int>(node->region.start.line),
                                                             static_cast<unsigned int>(node->region.start.col),
                                                             gen->currentScope));
#endif
}

void LlvmGen::finalize() {
    if (DBUILDER) {
        dBuilder->finalize();
    }

    // DO_OPTIMIZE
    for (auto fn : allFns) {
        TheFPM->run(*fn);
    }
    verifyModule(*module, &llvm::errs());
}

llvm::Type *LlvmGen::typeFor(Node *node) {
    assert(node->type == NodeType::TYPE);
    switch (node->typeData.kind) {
        case NodeTypekind::I8: {
            return builder.getInt8Ty();
        }
        case NodeTypekind::I32: {
            return builder.getInt32Ty();
        }
        case NodeTypekind::INT_LITERAL:
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
                elementTypes.push_back(builder.getInt32Ty());

                return llvm::StructType::get(context, elementTypes);
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
            return voidTy;
        }
        default: assert(false);
    }
}

llvm::Value *LlvmGen::rvalueFor(Node *node) {
    auto resolved = resolve(node);

    if (resolved->typeInfo->typeData.kind == NodeTypekind::NONE) { return nullptr; }

    if (resolved->isLocal) {
        return builder.CreateLoad((llvm::Value *) resolved->llvmLocal);
    }
    else if (resolved->type == NodeType::DOT && resolved->dotData.lhs->isLocal) {
        return builder.CreateLoad((llvm::Value *) resolved->llvmData);
    }

    return (llvm::Value *) resolved->llvmData;
}

llvm::DIType *diTypeFor(LlvmGen *gen, Node *type) {
    auto resolved = resolve(type);

    assert(resolved->type == NodeType::TYPE);

    switch (resolved->typeData.kind) {
        case NodeTypekind::INT_LITERAL:
        case NodeTypekind::I64:
            return gen->dBuilder->createBasicType("i64", 64, llvm::dwarf::DW_ATE_signed);
        case NodeTypekind::I32:
            return gen->dBuilder->createBasicType("i32", 32, llvm::dwarf::DW_ATE_signed);
        case NodeTypekind::I8:
            return gen->dBuilder->createBasicType("i8", 8, llvm::dwarf::DW_ATE_signed);
        case NodeTypekind::BOOLEAN:
            return gen->dBuilder->createBasicType("bool", 1, llvm::dwarf::DW_ATE_signed);
        case NodeTypekind::FLOAT_LITERAL:
        case NodeTypekind::F32:
            return gen->dBuilder->createBasicType("f32", 32, llvm::dwarf::DW_ATE_float);
        case NodeTypekind::F64:
            return gen->dBuilder->createBasicType("f64", 64, llvm::dwarf::DW_ATE_float);
        case NodeTypekind::FN: {
            llvm::SmallVector<llvm::Metadata *, 8> diTypes;
            diTypes.push_back(diTypeFor(gen, resolved->typeData.fnTypeData.returnType));
            for (auto pt : resolved->typeData.fnTypeData.params) {
                assert(pt->type == NodeType::DECL_PARAM);
                diTypes.push_back(diTypeFor(gen, pt->declParamData.type));
            }
            return gen->dBuilder->createSubroutineType(gen->dBuilder->getOrCreateTypeArray(diTypes));
        }
        case NodeTypekind::STRUCT: {
            auto diFile = gen->dBuilder->createFile(gen->diCu->getFilename(), gen->diCu->getDirectory());

            auto ty = gen->typeFor(resolved);
            auto structLayout = gen->module->getDataLayout().getStructLayout(reinterpret_cast<llvm::StructType *>(ty));

            auto alignment = structLayout->getAlignment();
            auto size = structLayout->getSizeInBits();

            llvm::SmallVector<llvm::Metadata *, 8> elements = {};
            unsigned int idx = 0;
            for (auto param : type->typeData.structTypeData.params) {
                auto basicType = diTypeFor(gen, param->typeInfo);

                string name;
                if (param->type == NodeType::VALUE_PARAM && param->valueParamData.name != nullptr) {
                    name = AtomTable::current->backwardAtoms[param->valueParamData.name->symbolData.atomId];
                }
                else if (param->type == NodeType::DECL_PARAM && param->declParamData.name != nullptr) {
                    name = AtomTable::current->backwardAtoms[param->declParamData.name->symbolData.atomId];
                }
                auto sizeInBits = basicType->getSizeInBits();
                auto alignInBits = basicType->getAlignInBits();
                auto offsetInBits = structLayout->getElementOffsetInBits(idx);

                elements.push_back(gen->dBuilder->createMemberType(gen->diCu, name, diFile,
                                                                   static_cast<unsigned int>(param->region.start.line),
                                                                   sizeInBits, alignInBits, offsetInBits,
                                                                   llvm::DINode::FlagZero, basicType));
                idx += 1;
            }

            auto ct = gen->dBuilder->createStructType(gen->diCu, "composite_type", diFile, 0, size,
                                                      alignment, llvm::DINode::FlagZero, nullptr, gen->dBuilder->getOrCreateArray(elements));
            return ct;
        }
        case NodeTypekind::POINTER: {
            auto basicType = diTypeFor(gen, resolved->typeData.pointerTypeData.underlyingType);
            auto pointerType = gen->typeFor(resolved);
            return gen->dBuilder->createPointerType(basicType,
                                                    gen->module->getDataLayout().getTypeSizeInBits(pointerType),
                                                    8 * gen->module->getDataLayout().getABITypeAlignment(pointerType));
        }
        case NodeTypekind::NONE: {
            return gen->dBuilder->createBasicType("none", 0, llvm::dwarf::DW_ATE_unsigned);
        }
        default:
            assert(false);
    }
}

void LlvmGen::gen(Node *node) {
    if (node->llvmGen) { return; }
    node->llvmGen = true;

    switch (node->type) {
        case NodeType::FN_DECL: {
            auto declOnly = node->fnDeclData.body.empty();

            // debug info
            auto fnName = node->fnDeclData.name ? AtomTable::current->backwardAtoms[node->fnDeclData.name->symbolData.atomId] : "anon";

            auto savedScope = currentScope;
            auto savedScopeName = currentScopeName;

#if DBUILDER
            llvm::DISubprogram *SP = nullptr;
            if (!declOnly) {
                llvm::DIFile *unit = dBuilder->createFile(diCu->getFilename(), diCu->getDirectory());
                 SP = dBuilder->createFunction(
                        unit, fnName, fnName, unit, (unsigned int) node->region.start.line,
                        static_cast<llvm::DISubroutineType *>(diTypeFor(this, node->typeInfo)),
                        false /* internal linkage */, true /* definition */, (unsigned int) node->region.start.line,
                        llvm::DINode::FlagZero, false, nullptr);
                currentScope = SP;
                currentScopeName = "SP_" + fnName;
            }
#endif

            std::vector<llvm::Type*> paramTypes = {};
            for (const auto& param : node->fnDeclData.params) {
                assert(param->type == NodeType::DECL_PARAM);
                paramTypes.push_back(typeFor(param->declParamData.type));
            }

            auto returnType = typeFor(node->fnDeclData.returnType);

            auto FT = llvm::FunctionType::get(returnType, paramTypes, false);

            auto *F = llvm::Function::Create(FT, llvm::Function::ExternalLinkage, fnName, module.get());

            if (!declOnly) {
#if DBUILDER
                emitDebugLocation(this, node);
                F->setSubprogram(SP);
#endif
            }

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
                auto shouldCreateAlloca = nodeTypeToAlloca->typeData.kind != NodeTypekind::NONE
                                          && !resolvedLocal->isAutoDerefStorage;

                if (shouldCreateAlloca) {
                    if (resolvedLocal->type == NodeType::DECL) {
                        if (resolvedLocal->declData.lvalue != nullptr) {
                            assert(resolvedLocal->declData.lvalue->type == NodeType::SYMBOL);
                            auto atomId = resolvedLocal->declData.lvalue->symbolData.atomId;

                            auto localName = AtomTable::current->backwardAtoms[atomId];
                            resolvedLocal->llvmLocal = builder.CreateAlloca(typeToAlloca, nullptr, localName);

                            if (DBUILDER) {
                                auto debugLoc = llvm::DebugLoc::get(
                                        static_cast<unsigned int>(resolvedLocal->region.start.line),
                                        static_cast<unsigned int>(resolvedLocal->region.start.col),
                                        currentScope);
                                auto diFile = dBuilder->createFile(diCu->getFilename(), diCu->getDirectory());
                                auto diLocalVar = dBuilder->createAutoVariable(currentScope, localName, diFile,
                                                                               static_cast<unsigned int>(resolvedLocal->region.start.line),
                                                                               diTypeFor(this, resolvedLocal->typeInfo));
                                dBuilder->insertDeclare((llvm::Value *) resolvedLocal->llvmLocal, diLocalVar,
                                                        dBuilder->createExpression(), debugLoc, builder.GetInsertBlock());
                            }
                        } else {
                            resolvedLocal->llvmLocal = builder.CreateAlloca(typeToAlloca, nullptr, "foreach_index");
                        }
                    } else {
                        resolvedLocal->llvmLocal = builder.CreateAlloca(typeToAlloca, nullptr, "local");
                    }
                }
            }

            auto didTerminate = false;
            for (const auto& stmt : node->fnDeclData.body) {
                if (stmt->type == NodeType::RET) { didTerminate = true; }
                gen(stmt);
            }
            if (!didTerminate) {
                builder.CreateRetVoid();
            }

            if (currentBB) {
                builder.SetInsertPoint(currentBB, currentIP);
            }

            currentScope = savedScope;
            currentScopeName = savedScopeName;
            currentFnDecl = savedFnDecl;
            emitDebugLocation(this, node);

            allFns.push_back(F);
        } break;
        case NodeType::RET: {
            gen(node->retData.value);
            emitDebugLocation(this, node);
            builder.CreateRet(rvalueFor(node->retData.value));
        } break;
        case NodeType::INT_LITERAL: {
            emitDebugLocation(this, node);
            node->llvmData = llvm::ConstantInt::get(typeFor(node->typeInfo), (uint64_t) node->intLiteralData.value);
        } break;
        case NodeType::FLOAT_LITERAL: {
            emitDebugLocation(this, node);
            node->llvmData = llvm::ConstantFP::get(typeFor(node->typeInfo), (float) node->floatLiteralData.value);
        } break;
        case NodeType::NIL_LITERAL: {
            emitDebugLocation(this, node);
            node->llvmData = llvm::ConstantPointerNull::get(builder.getInt8PtrTy(0));
        } break;
        case NodeType::BOOLEAN_LITERAL: {
            emitDebugLocation(this, node);
            node->llvmData = llvm::ConstantInt::get(typeFor(node->typeInfo), (uint64_t) node->boolLiteralData.value ? 1 : 0);
        } break;
        case NodeType::DECL: {
            auto data = node->declData;

            emitDebugLocation(this, node);

            if (data.initialValue == nullptr) { return; }

            gen(data.initialValue);

            auto resolvedInitialValue = resolve(data.initialValue);
            gen(resolvedInitialValue);

            if (data.initialValue->typeInfo->typeData.kind != NodeTypekind::NONE) {
                store(rvalueFor(data.initialValue), (llvm::Value *) node->llvmLocal);
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
            for (auto param : node->fnCallData.params) {
                gen(param);
                args.push_back(rvalueFor(param->valueParamData.value));
            }

            auto resolvedFn = resolve(node->fnCallData.fn);
            gen(resolvedFn);

            emitDebugLocation(this, node);
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
            emitDebugLocation(this, node);

            if (node->binopData.type == LexerTokenType::AND) {
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

                storeIfNeeded(node->binopData.lhs);
                storeIfNeeded(node->binopData.rhs);

                auto lhsValue = rvalueFor(node->binopData.lhs);
                auto rhsValue = rvalueFor(node->binopData.rhs);

                if (node->binopData.lhs->typeInfo->typeData.kind == NodeTypekind::POINTER) {
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
                            default: assert(false);
                        }
                    }
                    else {
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
                            case LexerTokenType::NE: {
                                value = builder.CreateICmpNE(lhsValue, rhsValue);
                            } break;
                            case LexerTokenType::LT: {
                                value = builder.CreateICmpSLT(lhsValue, rhsValue);
                            } break;
                            default: assert(false);
                        }
                    }

                    store(value, (llvm::Value *) node->llvmLocal);
                }
            }
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

                if (node->isLocal) {
                    store((llvm::Value *) node->llvmData, (llvm::Value *) node->llvmLocal);
                }
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

                emitDebugLocation(this, node);
                node->llvmData = store(rvalueFor(resolvedRhs), (llvm::Value *) resolvedLhs->llvmLocal);
            } else if (resolvedDecl->type == NodeType::DEREF) {
                // store rvalue into its slot
                storeIfNeeded(resolvedRhs);

                auto storageTargetNode = resolve(resolvedDecl->derefData.target);
                gen(storageTargetNode);

                auto storageTarget = (llvm::Value *) storageTargetNode->llvmLocal;
                if (storageTargetNode->type == NodeType::DECL_PARAM) {
                    storageTarget = (llvm::Value *) storageTargetNode->llvmData;
                    emitDebugLocation(this, node);
                    node->llvmData = store(rvalueFor(resolvedRhs), storageTarget);
                } else {
                    emitDebugLocation(this, node);
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
                emitDebugLocation(this, node);
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
                if (stmt->type == NodeType::RET) { didTerminate = true; }
                gen(stmt);
            }
            if (!didTerminate) {
                builder.CreateBr(mergeBlock);
            }

            builder.SetInsertPoint(elseBlock);
            didTerminate = false;
            for (auto stmt : node->ifData.elseStmts) {
                if (stmt->type == NodeType::RET) { didTerminate = true; }
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
                if (stmt->type == NodeType::RET) { didTerminate = true; }
                gen(stmt);
            }
            if (!didTerminate) {
                builder.CreateBr(condBlock);
            }

            builder.SetInsertPoint(mergeBlock);
        } break;
        case NodeType::PANIC: {
            emitDebugLocation(this, node);
            builder.CreateCall(panicFunc, { builder.CreateGlobalStringPtr("assertion failed!!!") });
        } break;
        case NodeType::STRUCT_LITERAL: {
            auto structType = (llvm::StructType *) typeFor(node->typeInfo);
            auto blankSlate = (llvm::Value *) llvm::ConstantStruct::get(structType);

            unsigned int idx = 0;
            for (auto param : node->structLiteralData.params) {
                gen(param);

                auto valueToInsert = rvalueFor(param->valueParamData.value);
                assert(valueToInsert);

                if (node->typeInfo->typeData.structTypeData.isSecretlyArray && idx == 0) {
                    valueToInsert = builder.CreateBitCast(valueToInsert,
                                                          typeFor(node->typeInfo->typeData.structTypeData.secretArrayElementType)->getPointerTo(0));
                }
                blankSlate = builder.CreateInsertValue(blankSlate, valueToInsert, idx);

                idx += 1;
            }

            node->llvmData = blankSlate;

            if (node->isLocal) {
                store((llvm::Value *) node->llvmData, (llvm::Value *) node->llvmLocal);
            }
        } break;
        case NodeType::ARRAY_LITERAL: {
            gen(node->arrayLiteralData.structLiteralRepresentation);

            node->llvmData = node->arrayLiteralData.structLiteralRepresentation->llvmData;
            node->llvmLocal = node->arrayLiteralData.structLiteralRepresentation->llvmLocal;
        } break;
        case NodeType::CAST: {
            gen(node->castData.value);

            auto toType = resolve(node->castData.type);
            auto fromType = resolve(node->castData.value->typeInfo);

            if (node->castData.isCastFromArrayToDataPtr) {
                auto arrayStruct = rvalueFor(node->castData.value);

                node->llvmData = builder.CreateExtractValue(arrayStruct, 0);
                node->llvmLocal = nullptr;
                node->isLocal = false;
            }
            else if ((fromType->typeData.kind == NodeTypekind::INT_LITERAL || fromType->typeData.kind == NodeTypekind::F32)
                     && toType->typeData.kind == NodeTypekind::F64) {
                node->llvmData = builder.CreateFPCast(static_cast<llvm::Value *>(node->castData.value->llvmData), builder.getDoubleTy());
            }
            else if ((fromType->typeData.kind == NodeTypekind::INT_LITERAL || fromType->typeData.kind == NodeTypekind::F64)
                     && toType->typeData.kind == NodeTypekind::F32) {
                // todo(chad): warn about losing information here
                node->llvmData = builder.CreateFPCast(static_cast<llvm::Value *>(node->castData.value->llvmData), builder.getFloatTy());
            }
            else if ((fromType->typeData.kind == NodeTypekind::INT_LITERAL || fromType->typeData.kind == NodeTypekind::I32)
                     && toType->typeData.kind == NodeTypekind::I64) {
                node->llvmData = builder.CreateIntCast(static_cast<llvm::Value *>(node->castData.value->llvmData), builder.getInt64Ty(), true);
            }
            else if ((fromType->typeData.kind == NodeTypekind::INT_LITERAL || fromType->typeData.kind == NodeTypekind::I32)
                     && toType->typeData.kind == NodeTypekind::I32) {
                // todo(chad): warn about losing information here
                node->llvmData = builder.CreateIntCast(static_cast<llvm::Value *>(node->castData.value->llvmData), builder.getInt32Ty(), true);
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
        case NodeType::STRING_LITERAL: {
            gen(node->resolved);

            node->llvmData = node->resolved->llvmData;
            node->llvmLocal = node->resolved->llvmLocal;
            node->isLocal = node->resolved->isLocal;
        } break;
        case NodeType::UNARY_NEG: {
            gen(node->nodeData);

            if (node->typeInfo->typeData.kind == NodeTypekind::INT_LITERAL
                || node->typeInfo->typeData.kind == NodeTypekind::I32
                || node->typeInfo->typeData.kind == NodeTypekind::I64) {
                node->llvmData = builder.CreateNeg(rvalueFor(node->nodeData));
            } else if (node->typeInfo->typeData.kind == NodeTypekind::FLOAT_LITERAL
                       || node->typeInfo->typeData.kind == NodeTypekind::F32
                       || node->typeInfo->typeData.kind == NodeTypekind::F64) {
                node->llvmData = builder.CreateFNeg(rvalueFor(node->nodeData));
            } else {
                assert(false);
            }
        } break;
        case NodeType::MALLOC: {
            gen(node->nodeData);
            emitDebugLocation(this, node);
            node->llvmData = builder.CreateCall(mallocFunc, { rvalueFor(node->nodeData) });
        } break;
        case NodeType::FREE: {
            gen(node->nodeData);
            emitDebugLocation(this, node);
            node->llvmData = builder.CreateCall(freeFunc, { rvalueFor(node->nodeData) });
        } break;
        case NodeType::SIZEOF: {
            auto sizeInBits = module->getDataLayout().getTypeSizeInBits(typeFor(node->nodeData));
            auto sizeInBytes = sizeInBits / 8;
            node->llvmData = llvm::ConstantInt::get(builder.getInt64Ty(), sizeInBytes);
        } break;
        case NodeType::FOR: {
            for (auto n : node->forData.rewritten) {
                gen(n);
            }
        } break;
        default: assert(false);
    }
}
