#include <sstream>
#include <stdlib.h>

#include "parser.h"

Parser::Parser(Lexer *lexer_) {
    lexer = lexer_;
    last = lexer->front;

    scopes = {};
    scopes.push(new Scope(nullptr));
}

void Parser::popFront() {
    last = lexer->front;
    lexer->popFront();
}

void Parser::reportError(string error) {
    cout << last.region.srcInfo.fileName << ":"
         << last.region.start.line << ":"
         << last.region.start.col << ":"
         << Colored<string>{"error: ", {Color::FG_RED}, true}
         << error << endl;
}

LexerToken Parser::expect(LexerTokenType type, string expectation) {
    if (lexer->front.type != type) {
        ostringstream s("");
        s << "expected " << expectation;
        auto errStr = s.str();
        last = lexer->front;
        reportError(errStr);

        exit(1);
    }

    auto saved = lexer->front;
    popFront();
    return saved;
}

void Parser::expectSemicolon() {
    expect(LexerTokenType::SEMICOLON, "';'");
}

void Parser::scopeInsert(int64_t atomId, Node *node) {
    auto _scope = scopes.top();

    auto found = _scope->symbols.find(atomId);
    if (found != _scope->symbols.end()) {
        ostringstream s("");
        s << "redeclaration of symbol '";
        s << AtomTable::current->backwardAtoms[atomId];
        s << "'";
        reportError(s.str());
    }

    _scope->symbols.insert({atomId, node});
}

void Parser::parseRoot() {
    while (!lexer->isEmpty()) {
        // comment
        while (lexer->front.type == LexerTokenType::COMMENT) {
            popFront();
        }

        if (!lexer->isEmpty()) {
            parseTopLevel();
        }
    }
}

Node *Parser::parseTopLevel() {
        // comment
        while (lexer->front.type == LexerTokenType::COMMENT) {
            // todo(chad): do we actually want to save this somehow?
            popFront();
        }

        // fn decl
        if (lexer->front.type == LexerTokenType::FN) {
            return parseFnDecl();
        }

        // type definition
        if (lexer->front.type == LexerTokenType::TYPE) {
            return parseTypeDecl();
        }

        reportError("Expected top level function declaration");
        exit(1);
}

vector<Node *> Parser::parseDeclParams() {
    vector<Node *> params;

    while (!lexer->isEmpty() && lexer->front.type != LexerTokenType::RPAREN && lexer->front.type != LexerTokenType::RCURLY) {
        DeclParamData param{};

        // lvalue
        auto name = parseSymbol();
        param.name = name;

        if (lexer->front.type != LexerTokenType::COLON && lexer->front.type != LexerTokenType::COLON_EQ) {
            reportError("expected ':' or ':='");
        }

        if (lexer->front.type == LexerTokenType::COLON) {
            popFront();
            param.type = parseType();

            if (lexer->front.type == LexerTokenType::EQ) {
                popFront();
                param.initialValue = parseRvalue();
            }
        }
        else {
            expect(LexerTokenType::COLON_EQ, ":=");
            param.initialValue = parseRvalue();
        }

        // comma (if not RPAREN or RCURLY)
        if (lexer->front.type != LexerTokenType::RPAREN && lexer->front.type != LexerTokenType::RCURLY) {
            expect(LexerTokenType::COMMA, ",");
        }

        auto node = new Node(lexer->srcInfo, &allNodes, NodeType::DECL_PARAM, scopes.top());

        node->declParamData = param;

        node->region = {lexer->srcInfo, name->region.start, param.type ? param.type->region.end : name->region.end};
        params.push_back(node);
    }

    return params;
}

vector<Node *> Parser::parseValueParams() {
    vector<Node *> params;

    while (!lexer->isEmpty() && lexer->front.type != LexerTokenType::RPAREN && lexer->front.type != LexerTokenType::RCURLY) {
        ValueParamData param{};

        Node *name = nullptr;

        // lvalue
        if (lexer->front.type == LexerTokenType::SYMBOL && lexer->next.type == LexerTokenType::COLON) {
            name = parseSymbol();
            param.name = name;

            expect(LexerTokenType::COLON, ":");
        }

        param.value = parseRvalue();

        // comma (if not RPAREN or RCURLY)
        if (lexer->front.type != LexerTokenType::RPAREN && lexer->front.type != LexerTokenType::RCURLY) {
            expect(LexerTokenType::COMMA, ",");
        }

        auto node = new Node(lexer->srcInfo, &allNodes, NodeType::VALUE_PARAM, scopes.top());

        node->valueParamData = param;

        node->region = {lexer->srcInfo, name ? name->region.start : param.value->region.start, param.value->region.end};
        params.push_back(node);
    }

    return params;
}

Node *Parser::parseFnDecl() {
    auto decl = new Node(lexer->srcInfo, &allNodes, NodeType::FN_DECL, scopes.top());
    decl->region.start = lexer->front.region.start;

    decl->fnDeclData.tableIndex = fnTableId;
    fnTableId += 1;

    auto savedCurrentFnDecl = currentFnDecl;
    currentFnDecl = decl;

    // 'fn'
    expect(LexerTokenType::FN, "fn");

    // name
    if (lexer->front.type != LexerTokenType::LPAREN) {
        auto name = parseSymbol();
        decl->fnDeclData.name = name;


        ostringstream nameStringstream("");
        nameStringstream << SourceRegion{name->region};
        auto nameString = nameStringstream.str();
        if (nameString == "main") {
            mainFn = decl;
        }

        scopeInsert(decl->fnDeclData.name->symbolData.atomId, decl);
    }

    // params
    expect(LexerTokenType::LPAREN, "(");
    auto firstParams = parseDeclParams();
    expect(LexerTokenType::RPAREN, ")");

    // more params?
    if (lexer->front.type == LexerTokenType::LPAREN) {
        popFront();
        auto secondParams = parseDeclParams();
        expect(LexerTokenType::RPAREN, ")");

        decl->fnDeclData.ctParams = firstParams;
        decl->fnDeclData.params = secondParams;
    } else {
        decl->fnDeclData.params = firstParams;
    }

    // return type
    if (lexer->front.type != LexerTokenType::LCURLY) {
        decl->fnDeclData.returnType = parseType();
    }

    // body
    expect(LexerTokenType::LCURLY, "{");

    scopes.push(new Scope(scopes.top()));

    while (!lexer->isEmpty() && lexer->front.type != LexerTokenType::RCURLY) {
        auto scopedStmt = parseScopedStmt();
        if (scopedStmt != nullptr) {
            scopedStmt->sourceMapStatement = true;
            decl->fnDeclData.body.push_back(scopedStmt);
        }
    }
    auto end = expect(LexerTokenType::RCURLY, "}");

    // put params in scope
    for (auto param : decl->fnDeclData.ctParams) {
        scopeInsert(param->declParamData.name->symbolData.atomId, param);
    }
    for (auto param : decl->fnDeclData.params) {
        scopeInsert(param->declParamData.name->symbolData.atomId, param);
    }

    decl->region.end = end.region.end;

    scopes.pop();
    currentFnDecl = savedCurrentFnDecl;

    return decl;
}

Node *Parser::parseTypeDecl() {
    auto saved = lexer->front.region.start;
    expect(LexerTokenType::TYPE, "type");

    auto typeName = parseSymbol();

    // todo(chad): @Cleanup just parse a type here! should be able to declare any type
    expect(LexerTokenType::STRUCT, "struct");

    expect(LexerTokenType::LCURLY, "{");

    auto typeDecl = new Node(lexer->srcInfo,
                             &allNodes,
                             NodeType::TYPE,
                             scopes.top());
    typeDecl->typeData.kind = NodeTypekind::STRUCT;
    typeDecl->typeData.structTypeData.name = typeName;
    typeDecl->typeData.structTypeData.params = parseDeclParams();
    typeDecl->typeData.structTypeData.isLiteral = false;
    scopeInsert(typeName->symbolData.atomId, typeDecl);

    auto end = lexer->front.region.end;
    expect(LexerTokenType::RCURLY, "}");

    // todo(chad): else  / else if

    typeDecl->region = Region{lexer->srcInfo, saved, end};

    return typeDecl;
}

Node *Parser::parseSymbol() {
    LexerToken front = expect(LexerTokenType::SYMBOL, "identifier");
    auto sym = new Node(lexer->srcInfo, &allNodes, NodeType::SYMBOL, scopes.top());
    sym->region = front.region;
    sym->symbolData.atomId = AtomTable::current->insert(sym->region);
    return sym;
}

Node *Parser::parseScopedStmt() {
    Node *stmt = nullptr;

    // comment
    if (lexer->front.type == LexerTokenType::COMMENT) {
        popFront();
        return nullptr;
    }

    // ret
    if (lexer->front.type == LexerTokenType::RET) {
        return parseRet();
    }

    // fn decl
    if (lexer->front.type == LexerTokenType::FN) {
        return parseFnDecl();
    }

    // type definition
    if (lexer->front.type == LexerTokenType::TYPE) {
        return parseTypeDecl();
    }

    // if
    if (lexer->front.type == LexerTokenType::IF) {
        return parseIf();
    }

    auto saved = lexer->front.region.start;
    auto lvalue = parseLvalue();

    if (lvalue->type == NodeType::FN_CALL || lvalue->type == NodeType::ASSERT) {
        expectSemicolon();
        return lvalue;
    }

    // declaration
    if (lexer->front.type == LexerTokenType::COLON) {
        // cannot declare anything but a symbol
        if (lvalue->type != NodeType::SYMBOL) {
            ostringstream s("invalid lhs for declaration: ");
            s << lvalue->type;
            reportError(s.str());
        }

        popFront();
        auto type = parseType();

        Node *rvalue = nullptr;
        if (lexer->front.type == LexerTokenType::EQ) {
            popFront();
            rvalue = parseRvalue();
        }

        expectSemicolon();

        auto decl = new Node(lexer->srcInfo, &allNodes, NodeType::DECL, scopes.top());
        addLocal(decl);

        decl->declData.lvalue = lvalue;
        decl->declData.initialValue = rvalue;
        decl->declData.type = type;

        decl->region = {lexer->srcInfo, saved, last.region.end};

        scopeInsert(lvalue->symbolData.atomId, decl);

        return decl;
    }

    // declaration-assignment combo
    if (lexer->front.type == LexerTokenType::COLON_EQ) {
        // cannot declaration-assign anything but a symbol
        if (lvalue->type != NodeType::SYMBOL) {
            ostringstream s("cannot assign to ");
            s << lvalue->type;
        }

        popFront();
        auto rvalue = parseRvalue();

        expectSemicolon();

        auto decl = new Node(lexer->srcInfo, &allNodes, NodeType::DECL, scopes.top());
        addLocal(decl);

        decl->declData.lvalue = lvalue;
        decl->declData.initialValue = rvalue;

        decl->region = {lexer->srcInfo, saved, last.region.end};

        scopeInsert(lvalue->symbolData.atomId, decl);

        return decl;
    }

    // assignment
    if (lexer->front.type == LexerTokenType::EQ) {
        // cannot assign anything but a symbol
        if (lvalue->type != NodeType::SYMBOL && lvalue->type != NodeType::DEREF && lvalue->type != NodeType::DOT) {
            ostringstream s("cannot assign to ");
            s << lvalue->type;
        }

        popFront();
        auto rvalue = parseRvalue();

        expectSemicolon();

        auto ass = new Node(lexer->srcInfo, &allNodes, NodeType::ASSIGN, scopes.top());

        ass->assignData.lhs = lvalue;
        ass->assignData.rhs = rvalue;
        ass->region = {lexer->srcInfo, saved, last.region.end};

        if (lvalue->type == NodeType::DEREF || lvalue->type == NodeType::DOT) {
            addLocal(rvalue);
        }

        return ass;
    }

    reportError("expected a declaration or an assignment");
    exit(1);
}

Node *Parser::parseIf() {
    auto saved = lexer->front.region.start;
    popFront();

    auto if_ = new Node(lexer->srcInfo, &allNodes, NodeType::IF, scopes.top());
    if_->region.start = saved;
    if_->ifData.condition = parseRvalue();

    expect(LexerTokenType::LCURLY, "{");
    while (lexer->front.type != LexerTokenType::RCURLY) {
        if_->ifData.stmts.push_back(parseScopedStmt());
    }

    if_->region.end = lexer->front.region.start;
    expect(LexerTokenType::RCURLY, "}");

    if (lexer->front.type == LexerTokenType::ELSE) {
        popFront();

        if (lexer->front.type == LexerTokenType::IF) {
            if_->ifData.elseStmts.push_back(parseIf());
            return if_;
        }

        expect(LexerTokenType::LCURLY, "{");

        while (lexer->front.type != LexerTokenType::RCURLY) {
            if_->ifData.elseStmts.push_back(parseScopedStmt());
        }

        if_->region.end = lexer->front.region.start;
        expect(LexerTokenType::RCURLY, "}");
    }

    return if_;
}

Node *Parser::parseRet() {
    auto saved = lexer->front.region.start;
    expect(LexerTokenType::RET, "return");

    auto ret = new Node(lexer->srcInfo, &allNodes, NodeType::RET, scopes.top());
    ret->retData.value = parseRvalue();
    ret->region = {lexer->srcInfo, saved, ret->retData.value->region.end};

    if (currentFnDecl) {
        currentFnDecl->fnDeclData.returns.push_back(ret);
    }

    expectSemicolon();

    return ret;
}

Node *Parser::buildDots(stack<Node *> rvalues) {
    auto top = rvalues.top();

    assert(!rvalues.empty());

    if (rvalues.size() == 1) {
        return top;
    }
    else if (top->type == NodeType::FN_CALL) {
        rvalues.pop();

        // create a new node (but reuse memory from top)
        top->type = NodeType::FN_CALL;

        top->fnCallData.fn = buildDots(rvalues);

        top->region = {lexer->srcInfo, top->fnCallData.fn->region.start, top->region.end};
        return top;
    } else if (top->type == NodeType::SYMBOL) {
        rvalues.pop();

        auto dot = new Node(lexer->srcInfo,
                            &allNodes,
                            NodeType::DOT,
                            scopes.top());

        dot->dotData.lhs = buildDots(rvalues);
        dot->dotData.rhs = top;

        dot->region = Region{lexer->srcInfo,
                             dot->dotData.lhs->region.start,
                             dot->dotData.rhs->region.end};

        addLocal(dot->dotData.lhs);

        return dot;
    }


    reportError("expected function call or symbol");
    exit(-1);
}

Node *Parser::parseLvalueHelper(Node *symbol, Location saved) {
    stack<Node *> rvalues;
    rvalues.push(symbol);

    while (lexer->front.type == LexerTokenType::LPAREN
            || lexer->front.type == LexerTokenType::NOT) {
        auto fnCall = parseFnCall();
        rvalues.push(fnCall);
    }

    // ('.' lvalue_simple)*
    while (lexer->front.type == LexerTokenType::DOT) {
        expect(LexerTokenType::DOT, ".");
        auto sym = parseSymbol();
        rvalues.push(sym);

        while (lexer->front.type == LexerTokenType::LPAREN
                || lexer->front.type == LexerTokenType::NOT) {
            auto fnCall = parseFnCall();
            rvalues.push(fnCall);
        }
    }

    if (rvalues.size() == 1) {
        return rvalues.top();
    }

    auto dot = buildDots(rvalues);
    dot->region = {lexer->srcInfo, saved, last.region.end};
    return dot;
}

Node *Parser::parseLvalue() {
    auto saved = lexer->front.region.start;

    // '(' lvalue ')'
    if (lexer->front.type == LexerTokenType::LPAREN) {
        popFront();
        auto symbol = parseLvalue();
        expect(LexerTokenType::RPAREN, ")");
        return parseLvalueHelper(symbol, saved);
    }

    // ^(lvalue)
    if (lexer->front.type == LexerTokenType::DEREF) {
        popFront();
        auto deref = new Node(lexer->srcInfo, &allNodes, NodeType::DEREF, scopes.top());
        deref->derefData.target = parseRvalueSimple();
        deref->derefData.isRvalue = false;
        return deref;
    }

    switch (lexer->front.type) {
        case LexerTokenType::SYMBOL: {
            auto symbol = parseSymbol();
            return parseLvalueHelper(symbol, saved);
        }
        case LexerTokenType::ASSERT: {
            popFront();
            auto assertNode = new Node(lexer->srcInfo, &allNodes, NodeType::ASSERT, scopes.top());
            expect(LexerTokenType::LPAREN, "(");

            // todo(chad): get rid of this.
            assertNode->nodeData = parseRvalue();
            addLocal(assertNode->nodeData);

            assertNode->sourceMapStatement = true;
            expect(LexerTokenType::RPAREN, ")");

            assertNode->region.start = saved;
            assertNode->region.end = lexer->front.region.start;

            return assertNode;
        } break;
        default: {
            auto savedType = lexer->front.type;
            popFront();

            ostringstream s("");
            s << savedType;
            s << " is not an lvalue!";
            reportError(s.str());
            exit(1);
        }
    }
}

Node *Parser::parseType() {
    auto saved = lexer->front;

    auto type = new Node(lexer->srcInfo, &allNodes, NodeType::TYPE, scopes.top());
    type->region = saved.region;

    switch (saved.type) {
        case LexerTokenType::I32: {
            popFront();
            type->typeData.kind = NodeTypekind::I32;
        } break;
        case LexerTokenType::I64: {
            popFront();
            type->typeData.kind = NodeTypekind::I64;
        } break;
        case LexerTokenType::FN: {
            popFront();
            type->typeData.kind = NodeTypekind::FN;
            expect(LexerTokenType::LPAREN, "(");
            type->typeData.fnTypeData.params = parseDeclParams();
            expect(LexerTokenType::RPAREN, ")");
            type->typeData.fnTypeData.returnType = parseType();
        } break;
        case LexerTokenType::SYMBOL: {
            auto typeName = parseSymbol();
            type->typeData.kind = NodeTypekind::SYMBOL;
            type->typeData.symbolTypeData.atomId = typeName->symbolData.atomId;
        } break;
        case LexerTokenType::STRUCT: {
            expect(LexerTokenType::STRUCT, "struct");

            expect(LexerTokenType::LCURLY, "{");

            type->typeData.kind = NodeTypekind::STRUCT;
            type->typeData.structTypeData.params = parseDeclParams();

            expect(LexerTokenType::RCURLY, "}");

            type->region.end = lexer->lastLoc;
        } break;
        case LexerTokenType::MUL: {
            popFront();

            type->typeData.kind = NodeTypekind::POINTER;
            type->typeData.pointerTypeData.underlyingType = parseType();

            type->region.end = lexer->lastLoc;
        } break;
        default: {
            ostringstream s("");
            s << "unknown type '" << SourceRegion{saved.region} << "'";
            reportError(s.str());
        }
    }

    return type;
}

Node *Parser::parseStructLiteral() {
    expect(LexerTokenType::LCURLY, "{");

    Node *node = new Node(lexer->srcInfo, &allNodes, NodeType::STRUCT_LITERAL, scopes.top());
    node->structLiteralData.params = parseValueParams();

    expect(LexerTokenType::RCURLY, "}");

    return node;
}

Node *Parser::parseRun() {
    Node *node = new Node(lexer->srcInfo, &allNodes, NodeType::RUN, scopes.top());
    node->region.start = lexer->front.region.start;
    popFront();

    auto savedFnDecl = currentFnDecl;
    currentFnDecl = nullptr;

    node->nodeData = parseRvalue();

    currentFnDecl = savedFnDecl;

    node->region.end = node->nodeData->region.end;
    return node;
}

Node *Parser::parseLvalueOrLiteral() {
    auto saved = lexer->front.region.start;
    Node *symbol;

    if (lexer->front.type == LexerTokenType::LPAREN) {
        popFront();
        symbol = parseRvalue();
        expect(LexerTokenType::RPAREN, ")");
    } else if (lexer->front.type == LexerTokenType::INT_LITERAL) {
        symbol = parseIntLiteral();
    } else if (lexer->front.type == LexerTokenType::FLOAT_LITERAL) {
        symbol = parseFloatLiteral();
    } else if (lexer->front.type == LexerTokenType::FN) {
        symbol = parseFnDecl();
        symbol->fnDeclData.isLiteral = true;
    } else if (lexer->front.type == LexerTokenType::LCURLY) {
        symbol = parseStructLiteral();
    } else if (lexer->front.type == LexerTokenType::RUN) {
        symbol = parseRun();
    } else {
        symbol = parseSymbol();
    }

    assert(symbol != nullptr);
    return parseLvalueHelper(symbol, saved);
}

Node *Parser::parseRvalueSimple() {
    auto saved = lexer->front.region.start;

    // & (rvalue_simple)
    if (lexer->front.type == LexerTokenType::AMP) {
        popFront();
        auto addrOf = new Node(lexer->srcInfo, &allNodes, NodeType::ADDRESS_OF, scopes.top());
        addrOf->nodeData = parseRvalueSimple();
        addrOf->region = Region{lexer->srcInfo, saved, addrOf->nodeData->region.end};
        addLocal(addrOf->nodeData);
        return addrOf;
    }

    // * (rvalue_simple)
    if (lexer->front.type == LexerTokenType::DEREF) {
        popFront();
        auto deref = new Node(lexer->srcInfo, &allNodes, NodeType::DEREF, scopes.top());

        deref->derefData.target = parseRvalueSimple();
        deref->derefData.isRvalue = true;

        addLocal(deref->derefData.target);
        addLocal(deref);

        deref->region = Region{lexer->srcInfo, saved, deref->derefData.target->region.end};
        return deref;
    }

    return parseLvalueOrLiteral();
}

Node *Parser::parseRvalue() {
    stack<ShuntingYard> operatingStack;
    stack<ShuntingYard> output;

    ShuntingYardData dataInit{};
    dataInit.node = parseRvalueSimple();

    auto sh = ShuntingYard{ShuntingYardType::NODE, dataInit};
    output.push(sh);

    auto type = lexer->front.type;
    while (isBinop(type)) {
        popFront();

        // keep popping while we find operators with a smaller precedence than what we're pushing
        while (!operatingStack.empty()
                && operatingStack.top().type == ShuntingYardType::OP
                && operatorPrecedence(operatingStack.top().data.type) >= operatorPrecedence(type)) {
            output.push(operatingStack.top());
            operatingStack.pop();
        }

        sh = ShuntingYard{ShuntingYardType::OP, {nullptr, type}};
        operatingStack.push(sh);

        ShuntingYardData dataNode = {parseRvalueSimple()};
        sh = ShuntingYard{ShuntingYardType::NODE, {dataNode}};
        output.push(sh);

        type = lexer->front.type;
    }

    while (!operatingStack.empty()) {
        output.push(operatingStack.top());
        operatingStack.pop();
    }

    return unwindPolish(&output);
}

Node *Parser::parseIntLiteral() {
    auto front = expect(LexerTokenType::INT_LITERAL, "integer literal");
    auto node = new Node(lexer->srcInfo, &allNodes, NodeType::INT_LITERAL, scopes.top());
    node->region = front.region;

    ostringstream s("");
    for (auto i = node->region.start.byteIndex; i < node->region.end.byteIndex; i++) {
        if (lexer->srcInfo.source[i] != '_') {
            s << lexer->srcInfo.source[i];
        }
    }

    node->intLiteralData.value = stoi(s.str());

    return node;
}

Node *Parser::parseFloatLiteral() {
    auto front = expect(LexerTokenType::FLOAT_LITERAL, "floating point literal");
    auto node = new Node(lexer->srcInfo, &allNodes, NodeType::FLOAT_LITERAL, scopes.top());
    node->region = front.region;

    ostringstream s("");
    for (auto i = node->region.start.byteIndex; i < node->region.end.byteIndex; i++) {
        if (lexer->srcInfo.source[i] != '_') {
            s << lexer->srcInfo.source[i];
        }
    }

    node->floatLiteralData.value = stod(s.str());

    return node;
}

int8_t Parser::operatorPrecedence(LexerTokenType type) {
    switch (type) {
        case LexerTokenType::OR:
        case LexerTokenType::AND:
            return 1;

        case LexerTokenType::EQ_EQ:
        case LexerTokenType::NE:
        case LexerTokenType::LE:
        case LexerTokenType::GE:
        case LexerTokenType::LT:
        case LexerTokenType::GT:
            return 2;

        case LexerTokenType::SUB:
        case LexerTokenType::ADD:
            return 3;

        case LexerTokenType::DIV:
        case LexerTokenType::MUL:
            return 4;

        case LexerTokenType::VERTICAL_BAR:
            return 5;

        default: assert(false);
    }
}

Node *Parser::unwindPolish(stack<ShuntingYard> *values) {
    auto top = values->top();
    values->pop();

    if (top.type == ShuntingYardType::NODE) {
        return top.data.node;
    } else {
        Node *binop = new Node(lexer->srcInfo, &allNodes, NodeType::BINOP, scopes.top());

        binop->binopData.type = top.data.type;
        binop->binopData.rhs = unwindPolish(values);
        binop->binopData.lhs = unwindPolish(values);

        binop->region = {lexer->srcInfo,
                         binop->binopData.lhs->region.start,
                         binop->binopData.rhs->region.end};

        addLocal(binop);

        return binop;
    }
}

void Parser::addLocal(Node *local) {
    if (local->isLocal) { return; }
    local->isLocal = true;

    if (currentFnDecl) {
        currentFnDecl->fnDeclData.locals.push_back(local);
    }
}

Node *Parser::parseFnCall() {
    auto call = new Node(lexer->srcInfo, &allNodes, NodeType::FN_CALL, scopes.top());

    if (lexer->front.type == LexerTokenType::NOT) {
        expect(LexerTokenType::NOT, "!");
        expect(LexerTokenType::LPAREN, "(");
        call->fnCallData.ctParams = parseValueParams();
        expect(LexerTokenType::RPAREN, ")");
    }

    expect(LexerTokenType::LPAREN, "(");
    call->fnCallData.params = parseValueParams();
    expect(LexerTokenType::RPAREN, ")");

    call->region.end = last.region.end;

    addLocal(call);

    return call;
}
