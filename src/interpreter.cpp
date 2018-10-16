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
#include <stdio.h>

#include<sys/types.h>
#include <fcntl.h>
#include <zconf.h>

using namespace std;

void printStmt(Interpreter *interp, int32_t pcStmtStart, ostream &s, bool withLineInfo = false) {
    for (auto stmt : interp->sourceMap.statements) {
        if (stmt.instIndex == (unsigned long) pcStmtStart) {
            s << stmt.node->region.srcInfo.source->substr(stmt.node->region.start.byteIndex, stmt.node->region.end.byteIndex - stmt.node->region.start.byteIndex);

            if (withLineInfo) {
                s << "[" << stmt.node->region.start.line << "]";
            }
        }
    }
}

void printCurrentStmt(Interpreter *interp, ostream &s, bool withLineInfo = false) {
    printStmt(interp, interp->pc, s, withLineInfo);
}

bool isValidPtr(void *p) {
    if (p == nullptr) { return false; }

    bool ret = true;
    int nullfd = open("/dev/random", O_WRONLY);
    if (write(nullfd, p, sizeof(void *)) < 0) {
        ret = false;
    }

    close(nullfd);
    return ret;
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
            debugPrintIntegerType<char>(target, offset);
        } break;
        case NodeTypekind::U8: {
            debugPrintIntegerType<uint8_t>(target, offset);
        } break;
        case NodeTypekind::I16: {
            debugPrintIntegerType<int16_t>(target, offset);
        } break;
        case NodeTypekind::U16: {
            debugPrintIntegerType<uint16_t>(target, offset);
        } break;
        case NodeTypekind::I32: {
            debugPrintIntegerType<int32_t>(target, offset);
        } break;
        case NodeTypekind::U32: {
            debugPrintIntegerType<uint32_t>(target, offset);
        } break;
        case NodeTypekind::I64: {
            debugPrintIntegerType<int64_t>(target, offset);
        } break;
        case NodeTypekind::U64: {
            debugPrintIntegerType<uint64_t>(target, offset);
        } break;
        case NodeTypekind::ENUM: {
            if (!isValidPtr((void *) offset)) {
                target << "<<INVALID>>";
            }
            else {
                int64_t enumValue = 0;

                switch (td.enumTypeData.type->typeData.kind) {
                    case NodeTypekind::I8: {
                        enumValue = *(int8_t *) offset;
                    } break;
                    case NodeTypekind::I16: {
                        enumValue = *(int16_t *) offset;
                    } break;
                    case NodeTypekind::I32: {
                        enumValue = *(int32_t *) offset;
                    } break;
                    case NodeTypekind::I64: {
                        enumValue = *(int64_t *) offset;
                    } break;
                    case NodeTypekind::INT_LITERAL: {
                        enumValue = td.intTypeData;
                    } break;
                    default: cpi_assert(false);
                }

                if (enumValue >= td.enumTypeData.params.length) {
                    target << "<<INVALID>>";
                }
                else {
                    if (enumValue <= 0 || enumValue >= td.enumTypeData.params.length) {
                        target << "<<NONAME>>";
                    }
                    else {
                        auto enumAtom = vector_at(td.enumTypeData.params, (unsigned long) enumValue - 1)->paramData.name->symbolData.atomId;
                        auto enumName = atomTable->backwardAtoms[enumAtom];
                        target << enumName;
                    }
                }
            }

            target << " (";
            debugPrintVar(target, interp, td.enumTypeData.type->typeData, offset, extraLines);
            target << ")";
        } break;
        case NodeTypekind::FLOAT_LITERAL: {
            target << td.floatTypeData;
        } break;
        case NodeTypekind::BOOLEAN_LITERAL: {
            target << td.boolTypeData;
        } break;
        case NodeTypekind::BOOLEAN: {
            auto p = (int32_t *) offset;
            if (isValidPtr(p)) {
                target << ((*p) == 1 ? "true" : "false");
            }
            else {
                target << "ERROR: could not read";
            }
        } break;
        case NodeTypekind::F32: {
            auto p = (float *) offset;
            if (isValidPtr(p)) {
                target << *p;
            }
            else {
                target << "ERROR: could not read";
            }
        } break;
        case NodeTypekind::F64: {
            auto p = (double *) offset;
            if (isValidPtr(p)) {
                target << *p;
            }
            else {
                target << "ERROR: could not read";
            }
        } break;
        case NodeTypekind::POINTER: {
            auto nvr = interp->nextVarReference;
            interp->nextVarReference += 1;

            int64_t loadedOffset = 0;
            bool isValidOffset = false;
            if (isValidPtr((int64_t *) offset)) {
                isValidOffset = true;
                loadedOffset = *((int64_t *) offset);
            }

            if (isValidOffset) {
                auto found = hash_get(interp->pointerRecursion, loadedOffset);
                if (found != nullptr) {
                    target << *found;
                    break;
                }

                auto toInsert = string("#" + to_string(nvr));
                hash_insert(interp->pointerRecursion, loadedOffset, toInsert);
            }

            target << "#" << nvr;

            ostringstream extra("");
            extra << "#" << nvr << ": ";
            extra << "*(0x" << hex << loadedOffset << dec << ")";
            if (loadedOffset == 0) {
                extra << "nil";
            }
            else if (!isValidOffset) {
                extra << "<<invalid ptr>>";
            }
            else {
                debugPrintVar(extra, interp, resolve(td.pointerTypeData.underlyingType)->typeData, loadedOffset, extraLines);
            }

            extraLines.push_back(extra.str());
        } break;
        case NodeTypekind::FN: {
            if (isValidPtr((int32_t *) offset)) {
                target << *((int32_t *) offset) << " (todo(chad): look up the fn name)";
            }
            else {
                target << "<<invalid fn ptr>>";
            }
        } break;
        case NodeTypekind::STRUCT: {
            if (!isValidPtr((int64_t *) offset)) {
                target << "<<invalid>>";
                break;
            }

            if (td.structTypeData.isSecretlyUnion) {
                auto nvr = interp->nextVarReference;
                interp->nextVarReference += 1;

                target << "#" << nvr;

                auto tag = *((int64_t *) offset);

                ostringstream extra("");
                extra << "#" << nvr << ": ";
                extra << "{";
                if (tag > 0) {
                    extra << "tag:" << tag;
                }

                if (tag >= 0 && tag < td.structTypeData.params.length) {
                    extra << " ";
                    auto param = vector_at(td.structTypeData.params, (unsigned long) tag);
                    cpi_assert(param->type == NodeType::DECL_PARAM);

                    extra << atomTable->backwardAtoms[param->paramData.name->symbolData.atomId] << ":";
                    debugPrintVar(extra, interp, resolve(param->typeInfo)->typeData, offset + 8, extraLines);
                }
                extra << "}";

                auto extraStr = extra.str();
                extraLines.push_back(extraStr);
            }
            else if (td.structTypeData.isSecretlyArray) {
                auto resolvedElementType = resolve(td.structTypeData.secretArrayElementType);

                auto size = *((int64_t *) (offset + 8));
                auto realSize = size > 50 ? 50 : size;
                auto ts = typeSize(td.structTypeData.secretArrayElementType);
                auto array_offset = *((int64_t *) offset);

                if (resolvedElementType->typeData.kind == NodeTypekind::I8) {
                    auto charPtr = (int8_t *) array_offset;

                    target << "\"";
                    for (auto i = 0; i < realSize; i++) {
                        if (isValidPtr(charPtr)) {
                            switch (*charPtr) {
                                case '\n': {
                                    target << "\\n";
                                }
                                default: target << *charPtr;
                            }
                        }
                        else {
                            target << "�";
                        }
                        charPtr += 1;
                    }
                    if (realSize < size) {
                        target << "...(truncated)";
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
                    for (auto i = 0; i < realSize; i++) {
                        extra << to_string(i) << ":";

                        debugPrintVar(extra, interp, resolve(td.structTypeData.secretArrayElementType)->typeData, array_offset, extraLines);
                        array_offset += ts;

                        if (i < realSize - 1) {
                            extra << " ";
                        }
                    }
                    if (realSize < size) {
                        target << "...(truncated)";
                    }
                    extra << "]";

                    extraLines.push_back(extra.str());
                }
            }
            else {
                // plain old struct
                auto nvr = interp->nextVarReference;
                interp->nextVarReference += 1;

                target << "#" << nvr;

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

void debugPrintVar(Interpreter *interp, int32_t bp, Node *n, ostream &s) {
    cpi_assert(n->isLocal || n->isBytecodeLocal);

    auto resolvedTypeinfo = resolve(resolve(n)->typeInfo);

    if (resolvedTypeinfo == nullptr) {
        s << "<<cannot resolve type>>";
        return;
    }

    vector<string> extra;
    debugPrintVar(s, interp, resolvedTypeinfo->typeData, ((int64_t) interp->stack_base) + bp + n->localOffset, extra);
    s << endl;
    for (const auto &e : extra) {
        s << e << endl;
    }
}

void printVarsInScope(Interpreter *interp, Scope *scope, int32_t bp, ostringstream &s, bool isLast = false) {
    for (auto i = 0; i < scope->symbols->bucket_count; i++) {
        auto bucket = scope->symbols->buckets[i];
        if (bucket != nullptr) {
            if (bucket->value->isLocal || bucket->value->isBytecodeLocal) {
                auto name = atomTable->backwardAtoms[bucket->key];
                s << name << ": ";
                debugPrintVar(interp, bp, bucket->value, s);
            }

            while (bucket->next != nullptr) {
                bucket = bucket->next;

                if (bucket->value->isLocal || bucket->value->isBytecodeLocal) {
                    auto name = atomTable->backwardAtoms[bucket->key];
                    s << name << ": ";
                    debugPrintVar(interp, bp, bucket->value, s);
                }
            }
        }
    }

    if (scope->isFunctionScope) {
        auto offset = -8;

        for (auto i = 0; i < scope->fnScopeParams.length; i++) {
            auto p = vector_at(scope->fnScopeParams, i);
            assert(p->type == NodeType::DECL_PARAM);

            auto name = atomTable->backwardAtoms[p->paramData.name->symbolData.atomId];
            s << name << ": ";

            auto resolvedTypeinfo = resolve(p->paramData.type);
            offset -= typeSize(resolvedTypeinfo);

            vector<string> extra;
            debugPrintVar(s, interp, resolvedTypeinfo->typeData, ((int64_t) interp->stack.data()) + bp + p->localOffset, extra);
            s << endl;

            for (const auto &e : extra) {
                s << e << endl;
            }
        }

        // fn return value
        if (scope->fnReturnType != nullptr) {
            s << "*RETURN*: ";

            vector<string> extra;
            debugPrintVar(s, interp, scope->fnReturnType->typeData, ((int64_t) interp->stack.data()) + bp, extra);
            s << endl;
            for (const auto &e : extra) {
                s << e << endl;
            }
        }
    }

    if (!isLast && scope->parent != nullptr) {
        printVarsInScope(interp, scope->parent, bp, s, scope->isFunctionScope);
    }
}

ostringstream printCurrentVars(Interpreter *interp, int32_t bp, uint32_t pc) {
    ostringstream s("");

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

            printVarsInScope(interp, node->scope, bp, s);
        }
    }

    return s;
}

string getInfo(Interpreter *interp) {
    ostringstream s("");

    interp->nextVarReference = 1;
    interp->pointerRecursion = hash_init<int64_t, string>(50);

    s << interp->depth + 1 << endl;

    auto bp = interp->bp;
    auto pc = interp->pc;

    for (uint16_t i = 0; i < interp->depth + 1; i++) {
        // line 1: location
        for (auto stmt : interp->sourceMap.statements) {
            if (stmt.node->region.srcInfo.fileName != nullptr && stmt.instIndex == (unsigned long) pc) {
                s << *stmt.node->region.srcInfo.fileName << endl;
                s << stmt.node->region.start.line << endl;
                s << stmt.node->region.start.col << endl;
            }
        }

        s << printCurrentVars(interp, bp, pc).str();

        if (i < interp->depth) {
            bp = interp->readFromStack<int32_t>(bp - 8);
            pc = (uint32_t) interp->pcs.at(interp->pcs.size() - 1 - i);
            s << "---" << endl;
        }
    }

    return s.str();
}

void runDebugger(Interpreter *interp, MnemonicPrinter *mp) {
    auto stmtStop = false;
    for (auto&& s : interp->sourceMap.statements) {
        if (s.instIndex == (unsigned long) interp->pc) {
            interp->stoppedOnStatement = s;
            stmtStop = true;
            break;
        }
    }

    auto breakStopIf = find_if(interp->breakpoints.begin(), interp->breakpoints.end(), [&](auto bp){ return bp.instIndex == interp->pc; });
    auto breakStop = breakStopIf != interp->breakpoints.end();
    if (breakStop) {
        if (breakStopIf->conditional) {
            auto evald = evaluate<int32_t>(interp, interp->stoppedOnStatement.node->region.srcInfo, interp->stoppedOnStatement.node->scope, breakStopIf->condition);
            if (evald == 0) {
                breakStop = false;
            }
        }
    }
    bool shouldStop = (stmtStop && !interp->continuing) || breakStop;

    while (interp->debugging && shouldStop && !interp->terminated && interp->depth < interp->overDepth) {
        interp->continuing = false;
        interp->overDepth = (2 << 15) + 1;

        interp->lastValidPc = interp->pc;

//            auto received = zmq_recv(zmq_sock, cline, 1024, 0);
//            cline[received] = 0;
//            auto line = string(cline);
        string line;
        getline(cin, line);

        while (!line.empty() && !interp->terminated) {
            if (line == "stack") {
                ostringstream s("");

                s << "[";
                for (auto i = 0; i < interp->sp; i++) {
                    s << static_cast<int32_t>(interp->stack[i]);
                    if (i < interp->sp - 1) { s << ", "; }
                }
                s << "]" << endl;

                interp->zsend(s.str());
            } else if (line == "frame") {
                ostringstream s("");

                s << "[";
                for (auto i = interp->bp; i < interp->sp; i++) {
                    s << static_cast<int32_t>(interp->stack[i]);
                    if (i < interp->sp - 1) { s << ", "; }
                }
                s << "]" << endl;

                interp->zsend(s.str());
            } else if (startsWith(&line, "break ")) {
                interp->breakCommands.push_back(line);

                interp->addBreakpointForCommand(line);

                interp->zsend("");
            } else if (line == "breakRemoveAll") {
                interp->breakpoints = {};
                interp->breakCommands = {};
            } else if (line == "location") {
                ostringstream s("");

                for (auto stmt : interp->sourceMap.statements) {
                    if (stmt.instIndex == (unsigned long) interp->pc) {
                        s << stmt.node->region.start.line << endl;
                        s << stmt.node->region.start.col << endl;
                    }
                }

                interp->zsend(s.str());
            } else if (line == "info") {
                interp->zsend(getInfo(interp));
            } else if (startsWith(&line, "eval")) {
                auto stmt = line.substr(5);

                auto answer = evaluate<int64_t>(interp, interp->stoppedOnStatement.node->region.srcInfo, interp->stoppedOnStatement.node->scope, stmt);

                ostringstream s("");
                s << "answer: " << answer << endl;
                interp->zsend(s.str());
            } else if (line == "stmt") {
                ostringstream s("");
                printCurrentStmt(interp, s);
                interp->zsend(s.str());
            } else if (line == "asm") {
                // print all insts between this stmt and the next one
                auto firstIndex = interp->pc;
                auto lastIndex = interp->pc;
                for (auto statement : interp->sourceMap.statements) {
                    auto stmt = statement;
                    if (stmt.instIndex == (unsigned long) interp->pc) {
                        lastIndex = (int32_t) statement.instEndIndex;
                        break;
                    }
                }

                interp->zsend(mp->debugString(firstIndex, lastIndex));
            } else if (line == "vars") {
                interp->zsend(printCurrentVars(interp, interp->bp, interp->pc).str());
            } else if (line == "step") {
                shouldStop = false;
                interp->zsend("");
                break;
            } else if (line == "over") {
                auto instAt = interp->table.at(interp->instructions[interp->pc]);
                if (instAt == interpretCall || instAt == interpretCalli) {
                    interp->overDepth = interp->depth + 1;
                }
                shouldStop = false;
                interp->zsend("");
                break;
            } else if (line == "out") {
                // prevent accidentally stepping 'out' of main fn
                if (interp->depth > 0) {
                    interp->overDepth = interp->depth;
                }
                shouldStop = false;
                interp->zsend("");
                break;
            } else if (line == "continue") {
                shouldStop = false;
                interp->continuing = true;
                interp->zsend("");
                break;
            } else if (line == "terminate" || line == "q" || line == "quit") {
                interp->terminated = true;
                interp->zsend("");
                break;
            } else {
                interp->zsend("unrecognized command");
            }

//                received = zmq_recv(zmq_sock, cline, 1024, 0);
//                if (received != -1) {
//                    cline[received] = 0;
//                    line = string(cline);
//                }
//                else {
//                    line = "";
//                }
            getline(cin, line);
        }
    }
}

void Interpreter::interpret() {
    auto mp = new MnemonicPrinter(this->instructions);

//    auto cline = (char *) "";
//    if (this->debugging) {
//        zmq_bind(zmq_sock, "tcp://*:5555");
//        cline = (char *) calloc(4096, sizeof(char));
//    }

    while ((unsigned long) pc < instructions.size() && !terminated) {
        if (this->debugging) runDebugger(this, mp);
        if (!terminated) step();
    }

    delete mp;
}

void Interpreter::step() {
//    stepCount++;
    table.at(instructions[pc++])(this);
}

void Interpreter::zsend(string s) {
    // todo(chad): expose this through a command-line flag so we can choose zmq over stderr, for example
//    zmq_send(zmq_sock, s.c_str(), s.size(), 0);

    cerr << s << endl;
}

void interpretExit(Interpreter *interp) {
    interp->terminated = true;
}

void interpretPanic(Interpreter *interp) {
    cout << "PANIC!" << endl;
    printCurrentStmt(interp, cout, true);
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

// bitnot
void interpretBitNot(Interpreter *interp) {
    auto bytes = interp->consume<int32_t>();
    auto offset = interp->consume<int64_t>();

    auto ptr = (char *) (interp->stack.data() + interp->bp + offset);
    for (auto i = 0; i < bytes; i++) {
        *ptr = ~*ptr;
        ptr += 1;
    }
}

// bitwise and
void interpretMathBitwiseAnd(Interpreter *interp) {
    auto bytes = interp->consume<int32_t>();

    auto currentOffset = (char *) interp->stack.data() + interp->bp;
    auto a = currentOffset + interp->consume<int64_t>();
    auto b = currentOffset + interp->consume<int64_t>();

    auto result = (char *) calloc((size_t) bytes, sizeof(char));

    for (int32_t i = 0; i < bytes; i++) {
        result[i] = a[i] & b[i];
    }

    auto storeOffset = interp->consume<int64_t>();
    memcpy(currentOffset + storeOffset, result, bytes * sizeof(char));
}

// bitwise or
void interpretMathBitwiseOr(Interpreter *interp) {
    auto bytes = interp->consume<int32_t>();

    auto currentOffset = (char *) interp->stack.data() + interp->bp;
    auto a = currentOffset + interp->consume<int64_t>();
    auto b = currentOffset + interp->consume<int64_t>();

    auto result = (char *) calloc((size_t) bytes, sizeof(char));

    for (int32_t i = 0; i < bytes; i++) {
        result[i] = a[i] | b[i];
    }

    auto storeOffset = interp->consume<int64_t>();
    memcpy(currentOffset + storeOffset, result, bytes * sizeof(char));
}

// bitwise xor
void interpretMathBitwiseXor(Interpreter *interp) {
    auto bytes = interp->consume<int32_t>();

    auto currentOffset = (char *) interp->stack.data() + interp->bp;
    auto a = currentOffset + interp->consume<int64_t>();
    auto b = currentOffset + interp->consume<int64_t>();

    auto result = (char *) calloc((size_t) bytes, sizeof(char));

    for (int32_t i = 0; i < bytes; i++) {
        result[i] = a[i] ^ b[i];
    }

    auto storeOffset = interp->consume<int64_t>();
    memcpy(currentOffset + storeOffset, result, bytes * sizeof(char));
}

// bitwise shift left
void interpretMathBitwiseShl(Interpreter *interp) {
    auto bytes = interp->consume<int32_t>();

    if (bytes == 1) {
        auto currentOffset = (char *) interp->stack.data() + interp->bp;
        auto a = currentOffset + interp->consume<int64_t>();
        auto b = currentOffset + interp->consume<int64_t>();

        auto result = *((int8_t *) a) << *((int64_t *) b);

        auto storeOffset = interp->consume<int64_t>();

        memcpy(currentOffset + storeOffset, &result, bytes * sizeof(char));
    }
    else if (bytes == 2) {
        auto currentOffset = (char *) interp->stack.data() + interp->bp;
        auto a = currentOffset + interp->consume<int64_t>();
        auto b = currentOffset + interp->consume<int64_t>();

        auto result = *((int16_t *) a) << *((int64_t *) b);

        auto storeOffset = interp->consume<int64_t>();

        memcpy(currentOffset + storeOffset, &result, bytes * sizeof(char));
    }
    else if (bytes == 4) {
        auto currentOffset = (char *) interp->stack.data() + interp->bp;
        auto a = currentOffset + interp->consume<int64_t>();
        auto b = currentOffset + interp->consume<int64_t>();

        auto result = *((int32_t *) a) << *((int64_t *) b);

        auto storeOffset = interp->consume<int64_t>();

        memcpy(currentOffset + storeOffset, &result, bytes * sizeof(char));
    }
    else if (bytes == 8) {
        auto currentOffset = (char *) interp->stack.data() + interp->bp;
        auto a = currentOffset + interp->consume<int64_t>();
        auto b = currentOffset + interp->consume<int64_t>();

        auto result = *((int64_t *) a) << *((int64_t *) b);

        auto storeOffset = interp->consume<int64_t>();

        memcpy(currentOffset + storeOffset, &result, bytes * sizeof(char));
    }
    else {
        cpi_assert(false);
    }
}

// bitwise logical shift right
void interpretMathBitwiseshr(Interpreter *interp) {
    auto bytes = interp->consume<int32_t>();

    if (bytes == 1) {
        auto currentOffset = (char *) interp->stack.data() + interp->bp;
        auto a = currentOffset + interp->consume<int64_t>();
        auto b = currentOffset + interp->consume<int64_t>();

        auto result = *((int8_t *) a) >> *((int64_t *) b);

        auto storeOffset = interp->consume<int64_t>();

        memcpy(currentOffset + storeOffset, &result, bytes * sizeof(char));
    }
    else if (bytes == 2) {
        auto currentOffset = (char *) interp->stack.data() + interp->bp;
        auto a = currentOffset + interp->consume<int64_t>();
        auto b = currentOffset + interp->consume<int64_t>();

        auto result = *((int16_t *) a) >> *((int64_t *) b);

        auto storeOffset = interp->consume<int64_t>();

        memcpy(currentOffset + storeOffset, &result, bytes * sizeof(char));
    }
    else if (bytes == 4) {
        auto currentOffset = (char *) interp->stack.data() + interp->bp;
        auto a = currentOffset + interp->consume<int64_t>();
        auto b = currentOffset + interp->consume<int64_t>();

        auto result = *((int32_t *) a) >> *((int64_t *) b);

        auto storeOffset = interp->consume<int64_t>();

        memcpy(currentOffset + storeOffset, &result, bytes * sizeof(char));
    }
    else if (bytes == 8) {
        auto currentOffset = (char *) interp->stack.data() + interp->bp;
        auto a = currentOffset + interp->consume<int64_t>();
        auto b = currentOffset + interp->consume<int64_t>();

        auto result = *((int64_t *) a) >> *((int64_t *) b);

        auto storeOffset = interp->consume<int64_t>();

        memcpy(currentOffset + storeOffset, &result, bytes * sizeof(char));
    }
    else {
        cpi_assert(false);
    }
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
        case NodeTypekind::U8: {
            interpretConvertFrom<uint8_t>(fromAddr, toAddr, toType);
        } break;
        case NodeTypekind::I16: {
            interpretConvertFrom<int16_t>(fromAddr, toAddr, toType);
        } break;
        case NodeTypekind::U16: {
            interpretConvertFrom<uint16_t>(fromAddr, toAddr, toType);
        } break;
        case NodeTypekind::I32: {
            interpretConvertFrom<int32_t>(fromAddr, toAddr, toType);
        } break;
        case NodeTypekind::U32: {
            interpretConvertFrom<uint32_t>(fromAddr, toAddr, toType);
        } break;
        case NodeTypekind::INT_LITERAL: // todo(chad): why are there still INT_LITERAL types at this point?
        case NodeTypekind::I64: {
            interpretConvertFrom<int64_t>(fromAddr, toAddr, toType);
        } break;
        case NodeTypekind::U64: {
            interpretConvertFrom<uint64_t>(fromAddr, toAddr, toType);
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
        case NodeTypekind::U8: return &ffi_type_uint8;
        case NodeTypekind::I16: return &ffi_type_sint16;
        case NodeTypekind::U16: return &ffi_type_uint16;
        case NodeTypekind::BOOLEAN:
        case NodeTypekind::I32:
            return &ffi_type_sint32;
        case NodeTypekind::U32: return &ffi_type_uint32;
        case NodeTypekind::I64: return &ffi_type_sint64;
        case NodeTypekind::U64: return &ffi_type_uint64;
        case NodeTypekind::F32: return &ffi_type_float;
        case NodeTypekind::F64: return &ffi_type_double;
        case NodeTypekind::STRUCT: {
            // todo(chad): @Leak
            auto args = (ffi_type **) malloc((type->typeData.structTypeData.params.length + 1) * sizeof(ffi_type *));
            for (unsigned long i = 0; i < type->typeData.structTypeData.params.length; i++) {
                args[i] = ffiTypeFor(vector_at(type->typeData.structTypeData.params, i)->typeInfo);
            }

            args[type->typeData.structTypeData.params.length] = nullptr;
            auto dp_type = (ffi_type *) malloc(sizeof(ffi_type));
            *dp_type = {.size = 0, .alignment = 0, .type = FFI_TYPE_STRUCT, .elements = args};
            return dp_type;
        }
        case NodeTypekind::POINTER: return &ffi_type_pointer;
        case NodeTypekind::ENUM: return ffiTypeFor(type->typeData.enumTypeData.type);

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

    auto hashFound = hash_get(interp->externalSymbols, fnName);
    if (hashFound != nullptr) {
        found_fn = *hashFound;
    }

    if (found_fn == nullptr) {
        for (auto handle : interp->libs) {
            found_fn = dlsym(handle, fnName.c_str());
            char* err = dlerror();
            if (!err) {
                hash_insert(interp->externalSymbols, fnName, found_fn);
                break;
            }
        }
    }

    if (found_fn == nullptr) {
        cout << "Fatal error: could not find external function " << fnName << endl;
        exit(1);
    }

    unsigned long paramCount = originalFn->fnDeclData.params.length;
    auto ffiArgs = (ffi_type **) malloc(paramCount * sizeof(ffi_type *));

    for (unsigned long i = 0; i < paramCount; i++) {
        ffiArgs[i] = ffiTypeFor(vector_at(originalFn->fnDeclData.params, i)->typeInfo);
    }

    ffi_cif cif;
    auto returnType = ffiTypeFor(originalFn->fnDeclData.returnType);
    ffi_status status = ffi_prep_cif(&cif, FFI_DEFAULT_ABI, (unsigned int) paramCount, returnType, ffiArgs);
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

void Interpreter::addBreakpointForCommand(string command) {
    int32_t bNum;
    int read;
    sscanf(command.c_str(), "break %d%n", &bNum, &read);

    auto rest = command.substr(read + 1);

    auto openSquareIndex = rest.find('[');
    auto closeSquareIndex = rest.find(']');

    auto fileName = rest.substr(0, openSquareIndex - 1);
    auto condition = rest.substr(openSquareIndex + 1, closeSquareIndex - (openSquareIndex + 1));

    // find the statement which is on this line
    for (auto stmt : sourceMap.statements) {
        if (stmt.node->region.start.line == (unsigned long) bNum && *stmt.node->region.srcInfo.fileName == fileName) {
            bool isConditional = condition.length() > 0;
            if (openSquareIndex == string::npos || closeSquareIndex == string::npos) {
                isConditional = false;
            }

            Breakpoint bp = {stmt.instIndex, isConditional, condition};
            breakpoints.push_back(bp);
        }
    }
}

void interpretMathAddSI64(Interpreter *interp) {
    auto a = interp->read<int64_t>();
    auto b = interp->read<int64_t>();
    auto c = interp->consume<int32_t>();
    auto result = a + b * c;
    auto storeOffset = interp->consume<int64_t>();
    interp->copyToStack(result, interp->bp + storeOffset);
}

void interpretMathSubSI64(Interpreter *interp) {
    auto a = interp->read<int64_t>();
    auto b = interp->read<int64_t>();
    auto c = interp->consume<int32_t>();
    auto result = a - b * c;
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

void storeToFrom(Interpreter *interp, void *to, void *from) {
    auto size = interp->consume<int32_t>();

    if (to == nullptr) {
        cout << "nil pointer dereference!!" << endl;
        cout << "at or near: " << interp->stoppedOnStatement.node->region.start.line << ":" << interp->stoppedOnStatement.node->region.start.col;
        exit(-1);
    }
    else {
        memcpy(to, from, static_cast<size_t>(size));
    }
}

void interpretStore(Interpreter *interp) {
    auto to = interp->stack_base + interp->read<int64_t>();
    auto from = interp->stack_base + interp->read<int64_t>();

    storeToFrom(interp, to, from);
}

void interpretStoreRelconstRelconst(Interpreter *interp) {
    auto to = interp->stack_base + interp->consume<int64_t>() + static_cast<int64_t>(interp->bp);
    auto from = interp->stack_base + interp->consume<int64_t>() + static_cast<int64_t>(interp->bp);

    storeToFrom(interp, to, from);
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

void interp_destroy(Interpreter *interp) {
    if (interp == nullptr) {
        return;
    }

    if (interp->debugging) {
        zmq_ctx_destroy(interp->zmq_ctx);
    }
}
