#include "interpreter.h"
#include "node.h"
#include "parser.h"
#include "bytecodegen.h"
#include "semantic.h"

#include <iostream>
#include <string.h>
#include <algorithm>
#include <sstream>
#include <dlfcn.h>
#include <ffi.h>
#include <SDL2/SDL.h>

using namespace std;

void printStmt(Interpreter *interp, int32_t pcStmtStart, bool withLineInfo = false) {
    for (auto stmt : interp->sourceMap.statements) {
        if (stmt.instIndex == (unsigned long) pcStmtStart) {
            cout << interp->sourceMap.sourceInfo.source->substr(stmt.startByte, stmt.endByte - stmt.startByte);

            if (withLineInfo) {
                cout << "[" << stmt.startLine << "] ";
            }

            cout << endl;
        }
    }
}

void printCurrentStmt(Interpreter *interp, bool withLineInfo = false) {
    printStmt(interp, interp->pc, withLineInfo);
}

void debugPrintVar(ostream &target, Interpreter *interp, TypeData td, int64_t offset, vector<string> &extraLines) {
    switch (td.kind) {
        case NodeTypekind::NONE: {
            target << "{}";
        } break;
        case NodeTypekind::INT_LITERAL: {
            target << td.intTypeData;
        } break;
        case NodeTypekind::I8: {
            auto p = (char *) offset;
            if (p != nullptr) {
                target << *p;
            }
        } break;
        case NodeTypekind::I16: {
            auto p = (int16_t *) offset;
            if (p != nullptr) {
                target << *p;
            }
        } break;
        case NodeTypekind::I32: {
            auto p = (int32_t *) offset;
            if (p != nullptr) {
                target << *p;
            }
        } break;
        case NodeTypekind::I64: {
            auto p = (int64_t *) offset;
            if (p != nullptr) {
                target << *p;
            }
        } break;
        case NodeTypekind::FLOAT_LITERAL: {
            target << td.floatTypeData;
        } break;
        case NodeTypekind::BOOLEAN_LITERAL: {
            target << td.boolTypeData;
        } break;
        case NodeTypekind::BOOLEAN: {
            auto p = (int32_t *) offset;
            if (p != nullptr) {
                target << ((*p) == 1 ? "true" : "false");
            }
        } break;
        case NodeTypekind::F32: {
            auto p = (float *) offset;
            if (p != nullptr) {
                target << *p;
            }
        } break;
        case NodeTypekind::F64: {
            auto p = (double *) offset;
            if (p != nullptr) {
                target << *p;
            }
        } break;
        case NodeTypekind::POINTER: {
            auto nvr = interp->nextVarReference;
            interp->nextVarReference += 1;

            auto loadedOffset = *((int64_t *) offset);
            auto found = hash_get(interp->pointerRecursion, loadedOffset);
            if (found != nullptr) {
                target << *found;
                break;
            }

            auto toInsert = string("#" + to_string(nvr));
            hash_insert(interp->pointerRecursion, loadedOffset, toInsert);

            target << "#" << nvr;

            ostringstream extra("");
            extra << "#" << nvr << ": ";
            if (loadedOffset == 0) {
                extra << "nil";
            }
            else {
                debugPrintVar(extra, interp, resolve(td.pointerTypeData.underlyingType)->typeData, loadedOffset, extraLines);
                extraLines.push_back(extra.str());
            }
        } break;
        case NodeTypekind::FN: {
            target << *((int32_t *) offset) << " (todo(chad): look up the fn name)";
        } break;
        case NodeTypekind::STRUCT: {
            if (td.structTypeData.isSecretlyEnum) {
                auto nvr = interp->nextVarReference;
                interp->nextVarReference += 1;

                target << "#" << nvr;

                auto tag = *((int64_t *) offset);

                auto param = vector_at(td.structTypeData.params, (unsigned long) tag);
                cpi_assert(param->type == NodeType::DECL_PARAM);

                ostringstream extra("");
                extra << "#" << nvr << ": ";
                extra << "{";
                if (tag > 0) {
                    extra << "tag:" << tag << " ";
                }
                extra << atomTable->backwardAtoms[param->paramData.name->symbolData.atomId] << ":";
                debugPrintVar(extra, interp, resolve(param->typeInfo)->typeData, offset + 8, extraLines);
                extra << "}";

                extraLines.push_back(extra.str());
            }
            else if (td.structTypeData.isSecretlyArray) {
                auto resolvedElementType = resolve(td.structTypeData.secretArrayElementType);

                auto size = *((int64_t *) (offset + 8));
                auto ts = typeSize(td.structTypeData.secretArrayElementType);
                auto array_offset = *((int64_t *) offset);

                if (resolvedElementType->typeData.kind == NodeTypekind::I8) {
                    target << "\"";
                    for (auto i = 0; i < size; i++) {
                        if ((int8_t *) array_offset != nullptr) {
                            target << *((int8_t *) array_offset);
                        }
                        else {
                            target << "0";
                        }
                        array_offset += ts;
                    }
                    target << "\"";
                }
                else {
                    auto nvr = interp->nextVarReference;
                    interp->nextVarReference += 1;

                    target << "#" << nvr;

                    // array
                    ostringstream extra("");
                    extra << "#" << nvr << ": ";

                    extra << "[";
                    for (auto i = 0; i < size; i++) {
                        extra << to_string(i) << ":";

                        debugPrintVar(extra, interp, resolve(td.structTypeData.secretArrayElementType)->typeData, array_offset, extraLines);
                        array_offset += ts;

                        if (i < size - 1) {
                            extra << " ";
                        }
                    }
                    extra << "]";

                    extraLines.push_back(extra.str());
                }
            }
            else {
                auto nvr = interp->nextVarReference;
                interp->nextVarReference += 1;

                target << "#" << nvr;

                // plain old struct
                ostringstream extra("");
                extra << "#" << nvr << ": ";
                extra << "{";
                auto sizeSoFar = 0;
                unsigned long idx = 0;

                for (const auto &param : td.structTypeData.params) {
                    auto name = "_" + to_string(idx);
                    if (param->paramData.name != nullptr) {
                        name = atomTable->backwardAtoms[param->paramData.name->symbolData.atomId];
                    }
                    extra << name << ":";

                    auto paramSize = typeSize(param->paramData.type);
                    auto paramAlign = typeAlign(param->paramData.type);

                    // alignment
                    if (sizeSoFar > 0 && paramAlign > 0) {
                        sizeSoFar += sizeSoFar % paramAlign;
                    }

                    debugPrintVar(extra, interp, resolve(param->typeInfo)->typeData, offset + sizeSoFar, extraLines);
                    if (idx < td.structTypeData.params.length - 1) {
                        extra << " ";
                    }

                    sizeSoFar += paramSize;

                    idx += 1;
                }
                extra << "}";

                extraLines.push_back(extra.str());
            }
        } break;
        default: cpi_assert(false);
    }
}

void debugPrintVar(Interpreter *interp, int32_t bp, Node *n) {
    cpi_assert(n->isLocal || n->isBytecodeLocal);

    auto resolvedTypeinfo = resolve(resolve(n)->typeInfo);

    vector<string> extra;
    debugPrintVar(cout, interp, resolvedTypeinfo->typeData, ((int64_t) interp->stack.data()) + bp + n->localOffset, extra);
    cout << endl;
    for (const auto &e : extra) {
        cout << e << endl;
    }
}

void printVarsInScope(Interpreter *interp, Scope *scope, int32_t bp, bool isLast = false) {
    for (auto i = 0; i < scope->symbols->bucket_count; i++) {
        auto bucket = scope->symbols->buckets[i];
        if (bucket != nullptr) {
            if (bucket->value->isLocal || bucket->value->isBytecodeLocal) {
                cout << atomTable->backwardAtoms[bucket->key] << ": ";
                debugPrintVar(interp, bp, bucket->value);
            }

            while (bucket->next != nullptr) {
                bucket = bucket->next;

                if (bucket->value->isLocal || bucket->value->isBytecodeLocal) {
                    cout << atomTable->backwardAtoms[bucket->key] << ": ";
                    debugPrintVar(interp, bp, bucket->value);
                }
            }
        }
    }

    if (scope->isFunctionScope) {
        auto offset = -8;

        for (auto i = 0; i < scope->fnScopeParams.length; i++) {
            auto p = vector_at(scope->fnScopeParams, i);
            assert(p->type == NodeType::DECL_PARAM);

            cout << atomTable->backwardAtoms[p->paramData.name->symbolData.atomId] << ": ";

            auto resolvedTypeinfo = resolve(p->paramData.type);
            offset -= typeSize(resolvedTypeinfo);

            vector<string> extra;
            debugPrintVar(cout, interp, resolvedTypeinfo->typeData, ((int64_t) interp->stack.data()) + bp + p->localOffset, extra);
            cout << endl;

            for (const auto &e : extra) {
                cout << e << endl;
            }
        }
    }

    if (!isLast) {
        printVarsInScope(interp, scope->parent, bp, scope->isFunctionScope);
    }
}

void printCurrentVars(Interpreter *interp, int32_t bp, uint32_t pc) {
    for (auto stmt : interp->sourceMap.statements) {
        if (stmt.instIndex == (unsigned long) pc) {
            auto node = stmt.node;
            if (node == nullptr) {
                continue;
            }

            // hack
            if (node->type == NodeType::FN_DECL && node->fnDeclData.body.length > 0) {
                node = vector_at(node->fnDeclData.body, 0);
            }

            printVarsInScope(interp, node->scope, bp);
        }
    }
}

void Interpreter::interpret() {
    auto mp = new MnemonicPrinter(this->instructions);

    while ((unsigned long) pc < instructions.size() && !terminated) {

        bool shouldStop = false;
        SourceMapStatement stoppedOnStatement;
        if (debugFlag) {
            auto stmtStop = false;
            for (auto&& s : sourceMap.statements) {
                if (s.instIndex == (unsigned long) pc) {
                    stoppedOnStatement = s;
                    stmtStop = true;
                }
            }

            auto breakStopIf = find(breakpoints.begin(), breakpoints.end(), pc);
            auto breakStop = breakStopIf != breakpoints.end();

            shouldStop = (stmtStop && !continuing) || breakStop;
        }

        while (shouldStop && !terminated && depth < overDepth) {
            continuing = false;
            overDepth = (2 << 15) + 1;

            lastValidPc = pc;

            string line;
            getline(cin, line);
            while (line != "" && !terminated) {
                if (line == "stack") {
                    cout << "[";
                    for (auto i = 0; i < sp; i++) {
                        cout << static_cast<int32_t>(stack[i]);
                        if (i < sp - 1) { cout << ", "; }
                    }
                    cout << "]";
                    cout << endl;
                } else if (line == "frame") {
                    cout << "[";
                    for (auto i = bp; i < sp; i++) {
                        cout << static_cast<int32_t>(stack[i]);
                        if (i < sp - 1) { cout << ", "; }
                    }
                    cout << "]";
                    cout << endl;
                } else if (startsWith(&line, "break ")) {
                    int32_t bNum;
                    sscanf(line.c_str(), "break %d", &bNum);

                    // find the statement which is on this line
                    for (auto stmt : sourceMap.statements) {
                        if (stmt.startLine == (unsigned long) bNum) {
                            breakpoints.push_back(stmt.instIndex);
                        }
                    }
                } else if (line == "breakRemove") {
                    breakpoints = {};
                } else if (line == "location") {
                    for (auto stmt : sourceMap.statements) {
                        if (stmt.instIndex == (unsigned long) pc) {
                            cout << stmt.startLine << endl;
                            cout << stmt.startCol << endl;
                        }
                    }
                } else if (line == "info") {
                    this->nextVarReference = 1;
                    this->pointerRecursion = hash_init<int64_t, string>(50);

                    cout << this->depth + 1 << endl;

                    auto bp = this->bp;
                    auto pc = this->pc;

                    for (uint16_t i = 0; i < this->depth + 1; i++) {
                        // line 1: location
                        for (auto stmt : this->sourceMap.statements) {
                            if (stmt.instIndex == (unsigned long) pc) {
                                cout << *stmt.node->region.srcInfo.fileName << endl;
                                cout << stmt.startLine << endl;
                                cout << stmt.startCol << endl;
                            }
                        }

                        printCurrentVars(this, bp, pc);
                        bp = this->readFromStack<int32_t>(bp - 8);

                        if (i < this->depth) {
                            pc = (uint32_t) this->pcs.at(this->pcs.size() - 1 - i);
                            cout << "---" << endl;
                        }
                    }

                    // todo(chad): free pointerRecursion
                } else if (startsWith(&line, "eval")) {
                    auto stmt = line.substr(5);

                    auto evalFnDecl = new Node(stoppedOnStatement.node->region.srcInfo, NodeType::FN_DECL, stoppedOnStatement.node->scope);

                    auto evalLexer = new Lexer(stmt, false);

                    auto evalParser = new Parser(evalLexer);
                    evalParser->isCopying = true;
                    evalParser->scopes.pop();
                    evalParser->scopes.push(stoppedOnStatement.node->scope);
                    evalParser->currentFnDecl = evalFnDecl;
                    auto parsed = evalParser->parseRvalue();

                    // set the srcInfo to the original srcInfo in case there's polymorphs
                    evalLexer->srcInfo = stoppedOnStatement.node->region.srcInfo;

                    auto semantic = new Semantic();
                    semantic->lexer = evalLexer;
                    semantic->parser = evalParser;
                    semantic->addStaticIfs(evalParser->scopes.top());
                    semantic->addImports(*evalParser->imports, *evalParser->scopeDecls);
                    semantic->currentFnDecl = evalFnDecl;

                    auto wrappedRet = new Node(parsed->region.srcInfo, NodeType::RETURN, parsed->scope);
                    wrappedRet->nodeData = parsed;

                    vector_append(evalFnDecl->fnDeclData.body, wrappedRet);
                    vector_append(evalFnDecl->fnDeclData.returns, wrappedRet);
                    semantic->resolveTypes(parsed);
                    semantic->resolveTypes(evalFnDecl);

                    // gen
                    auto gen = new BytecodeGen();
                    gen->isMainFn = true;
                    gen->sourceMap.sourceInfo = evalFnDecl->region.srcInfo;
                    gen->processFnDecls = true;

                    gen->gen(evalFnDecl);
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

//                    auto m = new MnemonicPrinter(gen->instructions);
//                    m->fnTable = gen->fnTable;
//                    cout << m->debugString() << endl;

                    auto interp = new Interpreter(semantic->linkLibs);
                    interp->bp = this->bp;
                    interp->sp = this->bp;
                    interp->externalFnTable = gen->externalFnTable;
                    interp->instructions = gen->instructions;
                    interp->fnTable = gen->fnTable;
                    interp->stack = this->stack; // todo(chad): this copies a lot of stuff. Make stack a vector_t? (or maybe we actually want to copy...)
                    interp->sourceMap = gen->sourceMap;
                    interp->continuing = true;
                    interp->instructions = gen->instructions;
                    interp->fnTable = gen->fnTable;
                    interp->interpret();

                    // todo(chad): present the type of the answer appropriately -- don't just assume it's i64
                    auto answer = interp->readFromStack<int32_t>(interp->bp);
                    cout << "answer: " << answer << endl;
                } else if (line == "stmt") {
                    printCurrentStmt(this);
                } else if (line == "asm") {
                    // print all insts between this stmt and the next one
                    auto firstIndex = pc;
                    auto lastIndex = pc;
                    for (auto statement : sourceMap.statements) {
                        auto stmt = statement;
                        if (stmt.instIndex == (unsigned long) pc) {
                            lastIndex = (int32_t) statement.instEndIndex;
                            break;
                        }
                    }

                    cout << mp->debugString(firstIndex, lastIndex);
                } else if (line == "vars") {
                    printCurrentVars(this, this->bp, this->pc);
                } else if (line == "step") {
                    shouldStop = false;
                    break;
                } else if (line == "over") {
                    auto instAt = table.at(instructions[pc]);
                    if (instAt == interpretCall || instAt == interpretCalli) {
                        overDepth = depth + 1;
                    }
                    shouldStop = false;
                    break;
                } else if (line == "out") {
                    // prevent accidentally stepping 'out' of main fn
                    if (depth > 0) {
                        overDepth = depth;
                    }
                    shouldStop = false;
                    break;
                } else if (line == "continue") {
                    shouldStop = false;
                    continuing = true;
                    break;
                } else if (line == "terminate" || line == "q" || line == "quit") {
                    terminated = true;
                    break;
                } else {
                    cout << "unrecognized command" << endl;
                }

                getline(cin, line);
            }
        }

        if (!terminated) {
            stepCount++;
            table.at(instructions[pc++])(this);
        }
    }

    delete mp;
}

void interpretExit(Interpreter *interp) {
    interp->terminated = true;
}

void interpretPanic(Interpreter *interp) {
    cout << "assertion failed!!!" << endl;
//    printStmt(interp, interp->lastStmtPc, true);
    exit(0);
}

// puts
void interpretPuts(Interpreter *interp) {
    auto offset_from_stack = interp->read<int64_t>();
    auto ptr_to_offset = (int64_t *) (interp->stack.data() + offset_from_stack);
    auto followed_ptr = (char *) *ptr_to_offset;

    auto ptr_to_count = (int64_t *) ((int8_t *) interp->stack.data() + offset_from_stack + 8);
    auto count = static_cast<size_t>(*ptr_to_count);

    fwrite(followed_ptr, sizeof(char), count, stdout);
}

// nop
void interpretNop(Interpreter *interp) {
    // do nothing!
}

// not
void interpretNot(Interpreter *interp) {
    auto offset = interp->consume<int64_t>();

    auto b = interp->readFromStack<int32_t>(offset + interp->bp);

    auto ptr = (int32_t *) (interp->stack.data() + interp->bp + offset);
    *ptr = !b;
}

// convert
void interpretConvert(Interpreter *interp) {
    auto fromType = (NodeTypekind) interp->consume<int32_t>();
    auto fromAddr = (interp->stack.data() + interp->bp + interp->consume<int64_t>());

    auto toType = (NodeTypekind) interp->consume<int32_t>();
    auto toAddr = (interp->stack.data() + interp->bp + interp->consume<int64_t>());

    switch (fromType) {
        case NodeTypekind::I8: {
            interpretConvertFrom<int8_t>(fromAddr, toAddr, toType);
        } break;
        case NodeTypekind::I16: {
            interpretConvertFrom<int16_t>(fromAddr, toAddr, toType);
        } break;
        case NodeTypekind::I32: {
            interpretConvertFrom<int32_t>(fromAddr, toAddr, toType);
        } break;
        case NodeTypekind::INT_LITERAL: // todo(chad): why are there still INT_LITERAL types at this point?
        case NodeTypekind::I64: {
            interpretConvertFrom<int64_t>(fromAddr, toAddr, toType);
        } break;
        case NodeTypekind::FLOAT_LITERAL:
        case NodeTypekind::F32: {
            interpretConvertFrom<float>(fromAddr, toAddr, toType);
        } break;
        case NodeTypekind::F64: {
            interpretConvertFrom<double>(fromAddr, toAddr, toType);
        } break;
        default: cpi_assert(false);
    }
}

// calli
void interpretCalli(Interpreter *interp) {
    auto fnTableIndex = interp->read<int64_t>();

    auto found = hash_get(interp->fnTable, (uint32_t) fnTableIndex);
    auto callIndex = *found;

    interp->callIndex(callIndex);
}

ffi_type *ffiTypeFor(Node *type) {
    type = resolve(type);
    cpi_assert(type->type == NodeType::TYPE);

    switch (type->typeData.kind) {
        case NodeTypekind::NONE: return &ffi_type_void;
        case NodeTypekind::I8: return &ffi_type_sint8;
        case NodeTypekind::I16: return &ffi_type_sint16;
        case NodeTypekind::BOOLEAN:
        case NodeTypekind::I32:
            return &ffi_type_sint32;
        case NodeTypekind::I64: return &ffi_type_sint64;
        case NodeTypekind::F32: return &ffi_type_float;
        case NodeTypekind::F64: return &ffi_type_double;
        case NodeTypekind::STRUCT: {
            // todo(chad): @Leak
            auto args = (ffi_type **) malloc((type->typeData.structTypeData.params.length + 1) * sizeof(ffi_type *));
            for (unsigned long i = 0; i < type->typeData.structTypeData.params.length; i++) {
                args[i] = ffiTypeFor(vector_at(type->typeData.structTypeData.params, i)->typeInfo);
                i += 1;
            }

            args[type->typeData.structTypeData.params.length] = nullptr;
            auto dp_type = (ffi_type *) malloc(sizeof(ffi_type));
            *dp_type = {.size = 0, .alignment = 0, .type = FFI_TYPE_STRUCT, .elements = args};
            return dp_type;
        }
        case NodeTypekind::POINTER: return &ffi_type_pointer;

        default: cpi_assert(false);
    }

    cpi_assert(false);
    return &ffi_type_void;
}

// calle
void interpretCalle(Interpreter *interp) {
    auto fnTableIndex = interp->consume<int32_t>();
    auto originalCallNode = vector_at(interp->externalFnTable, (unsigned long) fnTableIndex);
    assert(originalCallNode != nullptr);

    auto originalFn = resolve(originalCallNode->fnCallData.fn);
    assert(originalFn->type == NodeType::FN_DECL);
    auto fnName = atomTable->backwardAtoms[originalFn->fnDeclData.name->symbolData.atomId];

    void *found_fn = nullptr;
    for (auto handle : interp->libs) {
        found_fn = dlsym(handle, fnName.c_str());
        char* err = dlerror();
        if (!err) {
            break;
        }
    }

    if (found_fn == nullptr) {
        cout << "Fatal error: could not find external function " << fnName << endl;
        exit(1);
    }

    auto paramCount = originalFn->fnDeclData.params.length;
    auto ffiArgs = (ffi_type **) malloc(paramCount * sizeof(ffi_type *));

    for (auto i = 0; i < paramCount; i++) {
        ffiArgs[i] = ffiTypeFor(vector_at(originalFn->fnDeclData.params, i)->typeInfo);
    }

    ffi_cif cif;
    auto returnType = ffiTypeFor(originalFn->fnDeclData.returnType);
    ffi_status status = ffi_prep_cif(&cif, FFI_DEFAULT_ABI, paramCount, returnType, ffiArgs);
    if (status != FFI_OK) {
        fprintf(stderr, "ffi_prep_cif failed: %d\n", status);
        exit(1);
    }

    auto values = (void **) malloc(paramCount * sizeof(void *));
    auto paramSp = interp->sp;

    for (unsigned long i = 0; i < paramCount; i++) {
        auto paramType = vector_at(originalFn->fnDeclData.params, i)->typeInfo;
        paramSp -= typeSize(paramType);

        auto offsetPtr = &interp->stack[paramSp];
        values[i] = offsetPtr;
    }

    // call function copying return value bits to return slot
    // todo(chad): according to ffi this *must* be at least an int32_t unless the return type is void. So we'll need to deal with return types smaller than that
    auto storeInto = &interp->stack[interp->sp + 8];
    ffi_call(&cif, FFI_FN(found_fn), storeInto, values);

    free(ffiArgs);
    free(values);
}

// call
void interpretCall(Interpreter *interp) {
    interp->callIndex(interp->consume<int32_t>());
}

void Interpreter::callIndex(int64_t index) {
    depth += 1;

    pcs.push_back(lastValidPc);

    pushToStack(bp);
    pushToStack(pc);
    bp = sp;

    pc = index;
}

void Interpreter::dumpStack() {
    cout << "INTERP STACK:" << endl;
    for (auto i = 0; i < 64; i++) {
        cout << static_cast<int32_t>(stack[i]) << ", ";
    }
    cout << endl << endl;
}

void interpretMathAddSI64(Interpreter *interp) {
    auto a = interp->read<int64_t>();
    auto b = interp->read<int64_t>();
    auto c = interp->consume<int32_t>();
    auto result = a + b * c;
    auto storeOffset = interp->consume<int64_t>();
    interp->copyToStack(result, interp->bp + storeOffset);
}

// bumpsp numBits
void interpretBumpSP(Interpreter *interp) {
    auto numBytes = interp->consume<int32_t>();
    interp->sp += numBytes;
}

// ret
void interpretReturn(Interpreter *interp) {
    interp->sp = interp->bp - 8;
    interp->pc = (uint32_t) interp->readFromStack<int32_t>(interp->bp - 4);
    interp->bp = interp->readFromStack<int32_t>(interp->bp - 8);

    interp->depth -= 1;

    interp->pcs.pop_back();
}

// jumpif
void interpretJumpIf(Interpreter *interp) {
    auto constCond = interp->read<int32_t>();

    auto trueInst = interp->read<int32_t>();
    auto falseInst = interp->read<int32_t>();

    if (constCond == 1) {
        interp->pc = trueInst;
    } else {
        interp->pc = falseInst;
    }
}

// jump
void interpretJump(Interpreter *interp) {
    auto index = (uint32_t)interp->consume<int32_t>();

    interp->pc = index;
}

void interpretStore(Interpreter *interp) {
    auto storeOffset = interp->read<int64_t>();

    auto readOffset = interp->read<int64_t>();

    auto size = interp->consume<int32_t>();

    auto to = &interp->stack[storeOffset];
    auto from = &interp->stack[readOffset];

    memcpy(to, from, static_cast<size_t>(size));
}

void interpretStoreConst(Interpreter *interp) {
    auto storeOffset = interp->read<int64_t>();

    auto inst = static_cast<Instruction>(interp->instructions[interp->pc]);
    auto instStr = AssemblyLexer::instructionStrings[interp->instructions[interp->pc]];

    interp->pc += 1;
    if (inst == Instruction::CONSTI8) {
        int8_t value = interp->consume<int8_t>();
        memcpy(&interp->stack[storeOffset], &value, sizeof(int8_t));
    } else if (inst == Instruction::CONSTI16) {
        int16_t value = interp->consume<int16_t>();
        memcpy(&interp->stack[storeOffset], &value, sizeof(int16_t));
    } else if (inst == Instruction::CONSTI32) {
        int64_t value = interp->consume<int32_t>();
        memcpy(&interp->stack[storeOffset], &value, sizeof(int32_t));
    } else if (inst == Instruction::RELCONSTI64) {
        int64_t value = interp->consume<int64_t>() + interp->bp;
        memcpy(&interp->stack[storeOffset], &value, sizeof(int64_t));
    } else if (inst == Instruction::RELI64) {
        int64_t value = interp->consume<int64_t>() + interp->bp + (int64_t) interp->stack.data();
        memcpy(&interp->stack[storeOffset], &value, sizeof(int64_t));
    } else if (inst == Instruction::CONSTI64) {
        int64_t value = interp->consume<int64_t>();
        memcpy(&interp->stack[storeOffset], &value, sizeof(int64_t));
    } else if (inst == Instruction::CONSTF32) {
        auto value = interp->consume<float>();
        memcpy(&interp->stack[storeOffset], &value, sizeof(float));
    } else if (inst == Instruction::CONSTF64) {
        auto value = interp->consume<double>();
        memcpy(&interp->stack[storeOffset], &value, sizeof(double));
    } else {
        cpi_assert(false);
    }
}
