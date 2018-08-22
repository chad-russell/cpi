#ifndef LLVMGEN_H
#define LLVMGEN_H

#include "llvm/IR/LegacyPassManager.h"
#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"
#include "util.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Scalar/GVN.h"
#include "llvm/IR/DIBuilder.h"

class LlvmGen {
public:
    llvm::LLVMContext context;
    llvm::IRBuilder<> builder;
    unique_ptr<llvm::Module> module;
    llvm::TargetMachine *targetMachine;

    llvm::Constant *panicFunc;
    llvm::Constant *printfFunc;

    llvm::Type *voidTy;

    // Create a new pass manager attached to it.
    unique_ptr<llvm::legacy::FunctionPassManager> TheFPM;

    Node *currentFnDecl = nullptr;
    vector<llvm::Function *> allFns;
    llvm::DIScope *currentScope;
    string currentScopeName;
    bool forcing;

    LlvmGen(const char *fileName);

    llvm::Type *typeFor(Node *node);
    llvm::Value *rvalueFor(Node *node);
    llvm::Value *store(llvm::Value *val, llvm::Value *ptr);
    void storeIfNeeded(Node *node);

    void gen(Node *node);

    void finalize();
};

#endif // LLVM_CODEGEN_H
