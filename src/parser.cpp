#include <sstream>

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

void Parser::scopeInsert(int64_t atomId, Node *node) {
    auto _scope = scopes.top();

    auto found = _scope->symbols.find(atomId);
    if (found != _scope->symbols.end()) {
        ostringstream s("redeclaration of symbol ");
        s << AtomTable::current->backwardAtoms[atomId];
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

        reportError("Expected top level function declaration");
        exit(1);
}

Node *Parser::parseFnDecl() {
    auto decl = new Node(lexer->srcInfo, &allNodes, NodeType::FN_DECL, scopes.top());
    decl->region.start = lexer->front.region.start;

    auto savedCurrentFnDecl = currentFnDecl;
    currentFnDecl = decl;

    // 'fn'
    expect(LexerTokenType::FN, "fn");

    // name
    auto name = parseSymbol();
    decl->fnDeclData.atomId = AtomTable::current->insert(name->region);

    ostringstream nameStringstream("");
    nameStringstream << SourceRegion{name->region};
    auto nameString = nameStringstream.str();
    if (nameString == "main") {
        mainFn = decl;
    }

    scopeInsert(decl->fnDeclData.atomId, decl);

    // params
    expect(LexerTokenType::LPAREN, "(");

    // decl->fnDeclData.params = parseDeclParams();

    expect(LexerTokenType::RPAREN, ")");

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
    for (auto param : decl->fnDeclData.params) {
        scopeInsert(param->declParamData.atomId, param);
    }

    decl->region.end = end.region.end;

    scopes.pop();
    currentFnDecl = savedCurrentFnDecl;

    return decl;
}

Node *Parser::parseSymbol() {
    LexerToken front = expect(LexerTokenType::SYMBOL, "identifier");
    auto sym = new Node(lexer->srcInfo, &allNodes, NodeType::SYMBOL, scopes.top());
    sym->region = front.region;
    sym->symbolData.atomId = AtomTable::current->insert(sym->region);
    return sym;
}

Node *Parser::parseScopedStmt() {
    // comment
    if (lexer->front.type == LexerTokenType::COMMENT) {
        popFront();
        return nullptr;
    }

    // ret
    if (lexer->front.type == LexerTokenType::RET) {
        return parseRet();
    }

    auto saved = lexer->front.region.start;
    auto lvalue = parseLvalue();

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

        auto decl = new Node(lexer->srcInfo, &allNodes, NodeType::DECL, scopes.top());
        currentFnDecl->fnDeclData.locals.push_back(decl);

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

        auto decl = new Node(lexer->srcInfo, &allNodes, NodeType::DECL, scopes.top());
        currentFnDecl->fnDeclData.locals.push_back(decl);

        decl->declData.lvalue = lvalue;
        decl->declData.initialValue = rvalue;

        decl->region = {lexer->srcInfo, saved, last.region.end};

        scopeInsert(lvalue->symbolData.atomId, decl);

        return decl;
    }

    // assignment
    if (lexer->front.type == LexerTokenType::EQ) {
        // cannot assign anything but a symbol
        if (lvalue->type != NodeType::SYMBOL) {
            ostringstream s("cannot assign to ");
            s << lvalue->type;
        }

        popFront();
        auto rvalue = parseRvalue();

        auto ass = new Node(lexer->srcInfo, &allNodes, NodeType::ASSIGN, scopes.top());

        ass->assignData.lhs = lvalue;
        ass->assignData.rhs = rvalue;

        ass->region = {lexer->srcInfo, saved, last.region.end};
        return ass;
    }

    reportError("expected a declaration or an assignment");
    exit(1);
}

Node *Parser::parseRet() {
    auto saved = lexer->front.region.start;
    expect(LexerTokenType::RET, "return");

    auto ret = new Node(lexer->srcInfo, &allNodes, NodeType::RET, scopes.top());
    ret->retData.value = parseRvalue();
    ret->region = {lexer->srcInfo, saved, ret->retData.value->region.end};

    currentFnDecl->fnDeclData.returns.push_back(ret);

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
    }

    top = rvalues.top();
    reportError("expected function call or symbol");
    exit(-1);
}

Node *Parser::parseLvalueHelper(Node *symbol, Location saved) {
    stack<Node *> rvalues;
    rvalues.push(symbol);

    while (lexer->front.type == LexerTokenType::LPAREN) {
        auto fnCall = parseFnCall();
        rvalues.push(fnCall);
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

    switch (lexer->front.type) {
        case LexerTokenType::SYMBOL: {
            auto symbol = parseSymbol();
            return parseLvalueHelper(symbol, saved);
        }
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

    popFront();

    switch (saved.type) {
        case LexerTokenType::I32: {
            type->typeData.kind = NodeTypekind::I32;
        } break;
        case LexerTokenType::I64: {
            type->typeData.kind = NodeTypekind::I64;
        } break;
        default: {
            ostringstream s("");
            s << "unknown type '" << SourceRegion{saved.region} << "'";
            reportError(s.str());
        }
    }

    return type;
}

Node *parseLvalueHelper(Parser *parser, Node *symbol, Location start) {
    stack<Node *> rvalues;
    rvalues.push(symbol);

    while (parser->lexer->front.type == LexerTokenType::LPAREN) {
        auto fnCall = parser->parseFnCall();
        rvalues.push(fnCall);
    }

    if (rvalues.size() == 1) {
        return rvalues.top();
    }

    // build dots
    auto dot = parser->buildDots(rvalues);
    dot->region = Region{parser->lexer->srcInfo, start, parser->last.region.end};
    return dot;
}

Node *Parser::parseLvalueOrLiteral() {
    auto saved = lexer->front.region.start;
    Node *symbol;

    if (lexer->front.type == LexerTokenType::INT_LITERAL) {
        symbol = parseIntLiteral();
    } else if (lexer->front.type == LexerTokenType::FLOAT_LITERAL) {
        symbol = parseFloatLiteral();
    } else {
        symbol = parseSymbol();
    }

    assert(symbol != nullptr);
    return parseLvalueHelper(symbol, saved);
}

Node *Parser::parseRvalueSimple() {
    return parseLvalueOrLiteral();
}

Node *Parser::parseRvalue() {
    stack<ShuntingYard> operatingStack;
    stack<ShuntingYard> output;

    ShuntingYardData dataInit;
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

bool Parser::isBinop(LexerTokenType type) {
    return isBooleanBinop(type)
           || type == LexerTokenType::ADD
           || type == LexerTokenType::SUB
           || type == LexerTokenType::MUL
           || type == LexerTokenType::DIV
           || type == LexerTokenType::VERTICAL_BAR;
}

bool Parser::isBooleanBinop(LexerTokenType type) {
    return type == LexerTokenType::EQ_EQ
           || type == LexerTokenType::NE
           || type == LexerTokenType::LT
           || type == LexerTokenType::GT
           || type == LexerTokenType::LE
           || type == LexerTokenType::GE
           || type == LexerTokenType::AND
           || type == LexerTokenType::OR;
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

        currentFnDecl->fnDeclData.locals.push_back(binop);

        return binop;
    }
}

Node *Parser::parseFnCall() {
    auto call = new Node(lexer->srcInfo, &allNodes, NodeType::FN_CALL, scopes.top());

    popFront();
//    parse_params(parser, call->data.fn_call_data.params);
    expect(LexerTokenType::RPAREN, ")");

    call->region.end = last.region.end;
    return call;
}
