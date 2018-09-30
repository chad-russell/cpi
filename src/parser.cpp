#include <sstream>
#include <stdlib.h>

#include "parser.h"

Parser::Parser(Lexer *lexer_) {
    lexer = lexer_;
    last = lexer->front;

    mainAtom = atomTable->insertStr("main");

    imports = (vector_t<Node *> *) malloc(sizeof(vector_t<Node *>));
    *imports = vector_init<Node *>(16);

    impls = (vector_t<Node *> *) malloc(sizeof(vector_t<Node *>));
    *impls = vector_init<Node *>(16);

    contexts = (vector_t<Node *> *) malloc(sizeof(vector_t<Node *>));
    *contexts = vector_init<Node *>(16);

    contextInits = (vector_t<Node *> *) malloc(sizeof(vector_t<Node *>));
    *contextInits = vector_init<Node *>(16);

    allTopLevel = vector_init<Node *>(256);

    scopes.push(new Scope(nullptr));

    staticIfScope = scopes.top();
}

void Parser::popFront() {
    last = lexer->front;
    lexer->popFront();
}

void Parser::reportError(string error) {
    cout << *last.region.srcInfo.fileName << ":"
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

void Parser::appendEndScope(vector_t<Node *> &stmts) {
    auto endScopeNode = new Node(lexer->srcInfo, NodeType::END_SCOPE, scopes.top());
    endScopeNode->region = lexer->front.region;

    possiblyInsertDeferreds(endScopeNode, false);

    vector_append(stmts, endScopeNode);
}

void Parser::scopeInsert(int64_t atomId, Node *node) {
    auto _scope = scopes.top();

    auto found = hash_get(_scope->symbols, atomId);
    if (found != nullptr) {
        ostringstream s("");
        s << "redeclaration of symbol '" << atomTable->backwardAtoms[atomId] << "'";
        reportError(s.str());
    }

    hash_insert(_scope->symbols, atomId, node);
}

void Parser::initContext(Node *decl) {
    auto contextSym = new Node(lexer->srcInfo, NodeType::SYMBOL, scopes.top());
    contextSym->symbolData.atomId = atomTable->insertStr("context");

    auto contextDecl = new Node(lexer->srcInfo, NodeType::DECL, scopes.top());

    contextDecl->declData.lhs = contextSym;
    contextDecl->declData.initialValue = new Node(lexer->srcInfo, NodeType::CREATE_CONTEXT, scopes.top());

    addLocal(contextDecl);
    scopeInsert(contextSym->symbolData.atomId, contextDecl);

    vector_append(decl->fnDeclData.body, contextDecl);
}

Node *Parser::addImport(string importName, Node *alias) {
    auto concatPath = importName + ".cpi";

    auto rpath = realpath(concatPath.c_str(), nullptr);
    if (rpath == nullptr) {
        auto home = strdup(getenv("HOME"));
        rpath = realpath(string(strcat(home, "/.cpi/include/") + concatPath).c_str(), nullptr);
    }
    if (rpath == nullptr) {
        // this doesn't exist. Might not be an issue (that's for semantic to decide), but definitely don't continue
        return nullptr;
    }

    auto path = string(rpath);

    unsigned long lastSlash = 0;
    unsigned long lastDot = 0;
    for (unsigned long i = 0; i < path.length(); i++) {
        if (path[i] == '/') {
            lastSlash =  i;
        }
        if (path[i] == '.') {
            lastDot = i;
        }
    }
    auto defaultImportName = path.substr(lastSlash + 1, lastDot - (lastSlash + 1));

    auto found = false;

    auto importAtomId = atomTable->insertStr(defaultImportName);
    if (alias != nullptr) {
        importAtomId = alias->symbolData.atomId;
    }

    for (auto i : importedFileModules) {
        auto iName = atomTable->backwardAtoms[i->moduleData.fullImportAtomId].c_str();
        if (strcmp(iName, path.c_str()) == 0) {
            scopeInsert(importAtomId, i);

            return i;
        }
    }

    if (!found) {
        auto lexer = new Lexer(new string(path), nullptr);
        auto parser = new Parser(lexer);
        parser->contexts = this->contexts;
        parser->contextInits = this->contextInits;

        auto fileModule = new Node(lexer->srcInfo, NodeType::MODULE, nullptr);
        fileModule->moduleData.name = new Node(lexer->srcInfo, NodeType::SYMBOL, parser->scopes.top());
        fileModule->moduleData.name->symbolData.atomId = atomTable->insertStr(importName);
        fileModule->moduleData.fullImportAtomId = atomTable->insertStr(path);
        vector_append(importedFileModules, fileModule);

        fileModule->scope = parser->scopes.top();
        parser->parseRoot();

        fileModule->moduleData.stmts = parser->allTopLevel;

        scopeInsert(importAtomId, fileModule);

        return fileModule;
    }

    return nullptr;
}

void Parser::addBasicImport() {
    auto importNode = new Node(this->lexer->srcInfo, NodeType::IMPORT, scopes.top());
    importNode->importData.isFile = true;
    importNode->importData.target = addImport("basic", nullptr);

    vector_append(*this->imports, importNode);
    vector_append(allTopLevel, importNode);
}

void Parser::addContextParameterForDecl(vector_t<Node *> &currentParams, Scope *scope) {
    auto paramsWithContext = vector_init<Node *>(currentParams.length + 1);

    auto ptrToContextSym = new Node(NodeTypekind::POINTER);
    ptrToContextSym->typeData.pointerTypeData.underlyingType = new Node(NodeTypekind::CONTEXT_TYPE);
    vector_append(paramsWithContext, wrapInDeclParam(ptrToContextSym, "context", 0));
    for (auto p : currentParams) {
        vector_append(paramsWithContext, p);
    }

    currentParams = paramsWithContext;
}

void Parser::parseRoot() {
    if (!noIppFlag) {
        addBasicImport();
    }

    while (!lexer->isEmpty()) {
        // comment
        while (lexer->front.type == LexerTokenType::COMMENT) {
            popFront();
        }

        if (!lexer->isEmpty()) {
            auto topLevel = parseTopLevel();
            vector_append(allTopLevel, topLevel);
        }
    }
}

Node *Parser::parseTopLevel() {
    // comment
    while (lexer->front.type == LexerTokenType::COMMENT) {
        popFront();
    }

    // fn decl
    if (lexer->front.type == LexerTokenType::FN) {
        return parseFnDecl();
    }

    // run
    if (lexer->front.type == LexerTokenType::RUN) {
        return parseRun();
    }

    // #attr
    if (lexer->front.type == LexerTokenType::ATTR) {
        return parseAttr();
    }

    // #if
    if (lexer->front.type == LexerTokenType::STATIC_IF) {
        return parseIf();
    }

    // #context
    if (lexer->front.type == LexerTokenType::CONTEXT) {
        return parseContext();
    }

    // #contextinit
    if (lexer->front.type == LexerTokenType::CONTEXT_INIT) {
        return parseContextInit();
    }

    // type definition
    if (lexer->front.type == LexerTokenType::TYPE) {
        return parseTypeDecl();
    }

    // module declaration
    if (lexer->front.type == LexerTokenType::MODULE) {
        return parseModuleDecl();
    }

    // import
    if (lexer->front.type == LexerTokenType::IMPORT) {
        return parseImport();
    }

    // link
    if (lexer->front.type == LexerTokenType::LINK) {
        return parseLink();
    }

    // alias
    auto ss = parseScopedStmt();
    if (ss->type == NodeType::ALIAS) {
        return ss;
    }

    reportError("Expected top level declaration");
    exit(1);
}

Node *Parser::parseDeclParam() {
    auto node = new Node(lexer->srcInfo, NodeType::DECL_PARAM, scopes.top());
    initParamData(node);

    // lvalue
    auto name = parseSymbol();
    node->paramData.name = name;

    if (lexer->front.type != LexerTokenType::COLON && lexer->front.type != LexerTokenType::COLON_EQ) {
        reportError("expected ':' or ':='");
    }

    if (lexer->front.type == LexerTokenType::COLON) {
        popFront();

        if (lexer->front.type == LexerTokenType::NOT) {
            // fn foo(t: !T) { ... }
            popFront();
            node->paramData.isAutoPolyParam = true;
        }
        node->paramData.type = parseType();

        if (lexer->front.type == LexerTokenType::DIV) {
            popFront(); // '/'
            node->paramData.polyCameFrom = parseSymbol();
        }

        if (lexer->front.type == LexerTokenType::EQ) {
            popFront();
            node->paramData.value = parseRvalue();
        }
    }
    else {
        expect(LexerTokenType::COLON_EQ, ":=");
        node->paramData.value = parseRvalue();
    }

    node->region.start = name->region.start;
    if (node->paramData.value != nullptr) {
        node->region.end = node->paramData.value->region.end;
    } else if (node->paramData.type != nullptr) {
        node->region.end = node->paramData.type->region.end;
    } else {
        node->region.end = name->region.end;
    }

    return node;
}

vector_t<Node *> Parser::parseDeclParams() {
    auto params = vector_init<Node *>(10);

    while (!lexer->isEmpty() && lexer->front.type != LexerTokenType::RPAREN && lexer->front.type != LexerTokenType::RCURLY) {
        auto param = parseDeclParam();

        // comma (if not RPAREN or RCURLY)
        if (lexer->front.type != LexerTokenType::RPAREN && lexer->front.type != LexerTokenType::RCURLY) {
            expect(LexerTokenType::COMMA, ",");
        }

        vector_append(params, param);
    }

    return params;
}

vector_t<Node *> Parser::parseValueParams() {
    vector_t<Node *> params = vector_init<Node *>(10);

    while (!lexer->isEmpty() && lexer->front.type != LexerTokenType::RPAREN && lexer->front.type != LexerTokenType::RCURLY) {
        ParamData param{};

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

        auto node = new Node(lexer->srcInfo, NodeType::VALUE_PARAM, scopes.top());

        node->paramData = param;

        node->region = {lexer->srcInfo, name ? name->region.start : param.value->region.start, param.value->region.end};
        vector_append(params, node);
    }

    return params;
}

Node *makeAutoPolyCtParam(Node *originalParam) {
    // fn foo(t: !T) ==> fn foo(T := typeof(t))(t: T)

    cpi_assert(originalParam->type == NodeType::DECL_PARAM);
    cpi_assert(originalParam->paramData.isAutoPolyParam);

    auto paramType = originalParam->paramData.type;
    cpi_assert(paramType->type == NodeType::TYPE);
    cpi_assert(paramType->typeData.kind == NodeTypekind::SYMBOL);
    auto paramTypeDebug = atomTable->backwardAtoms[paramType->typeData.symbolTypeData.atomId];

    auto paramName = originalParam->paramData.name;
    cpi_assert(paramName->type == NodeType::SYMBOL);
    auto paramNameDebug = atomTable->backwardAtoms[paramName->symbolData.atomId];

    auto newParam = new Node(originalParam->region.srcInfo, NodeType::DECL_PARAM, originalParam->scope);

    newParam->paramData.name = new Node(originalParam->region.srcInfo, NodeType::SYMBOL, originalParam->scope);
    newParam->paramData.name->symbolData.atomId = paramType->typeData.symbolTypeData.atomId;

    newParam->paramData.value = new Node(originalParam->region.srcInfo, NodeType::TYPEOF, originalParam->scope);

    newParam->paramData.value->nodeData = new Node(originalParam->region.srcInfo, NodeType::SYMBOL, originalParam->scope);
    newParam->paramData.value->nodeData->symbolData.atomId = paramName->symbolData.atomId;

    newParam->paramData.isAutoPolyParam = true;

    return newParam;
}

void maybeAddAutoPolyForFnDecl(Node *decl, vector_t<Node *> params) {
    auto isAutoPoly = false;
    for (auto p : params) {
        if (p->paramData.isAutoPolyParam) {
            isAutoPoly = true;
        }
    }

    if (isAutoPoly) {
        for (auto p : params) {
            if (p->paramData.isAutoPolyParam) {
                vector_append(decl->fnDeclData.ctParams, makeAutoPolyCtParam(p));
            }
        }
    }
}

void maybeAddAutoPolyForTypeDecl(vector_t<Node *> &ctParams, vector_t<Node *> params) {
    auto isAutoPoly = false;
    for (auto p : params) {
        if (p->paramData.isAutoPolyParam) {
            isAutoPoly = true;
        }
    }

    if (isAutoPoly) {
        for (auto p : params) {
            if (p->paramData.isAutoPolyParam) {
                vector_append(ctParams, makeAutoPolyCtParam(p));
            }
        }
    }
}

Node *Parser::parseFnDecl() {
    auto decl = new Node(lexer->srcInfo, NodeType::FN_DECL, scopes.top());
    decl->region.start = lexer->front.region.start;
    decl->sourceMapStatement = true;

    auto savedCurrentFnDecl = currentFnDecl;
    currentFnDecl = decl;

    // 'fn'
    expect(LexerTokenType::FN, "fn");

    // name
    if (lexer->front.type != LexerTokenType::LPAREN) {
        auto name = parseSymbol();
        decl->fnDeclData.name = name;

        if (name->symbolData.atomId == mainAtom) {
            mainFn = decl;
        }

        if (!this->isCopying) {
            scopeInsert(decl->fnDeclData.name->symbolData.atomId, decl);
        }
    }

    scopes.push(new Scope(scopes.top()));

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

        maybeAddAutoPolyForFnDecl(decl, secondParams);
    } else {
        decl->fnDeclData.params = firstParams;

        maybeAddAutoPolyForFnDecl(decl, firstParams);
    }

    // return type
    if (lexer->front.type != LexerTokenType::LCURLY) {
        decl->fnDeclData.returnType = parseType();
    }

    // if there's no more then it's an external declaration
    // todo(chad): once we have annotations, make an @extern. Everything else can be a faster calling convention
    if (lexer->front.type != LexerTokenType::LCURLY) {
        decl->fnDeclData.isExternal = true;

        decl->region.end = decl->fnDeclData.returnType->region.end;

        scopes.pop();
        currentFnDecl = savedCurrentFnDecl;

        return decl;
    }

    // This is *not* an external declaration at this point
    // so as long as it's not the main fn OR the initContext() fn, add `context: *basic.Context` as the first parameter
    if (decl->fnDeclData.name == nullptr || decl->fnDeclData.name->symbolData.atomId != mainAtom) {
        if (!noIppFlag) {
            addContextParameterForDecl(decl->fnDeclData.params, scopes.top());
        }
    }

    // put params in scope
    for (auto param : decl->fnDeclData.ctParams) {
        scopeInsert(param->paramData.name->symbolData.atomId, param);
    }
    for (auto param : decl->fnDeclData.params) {
        scopeInsert(param->paramData.name->symbolData.atomId, param);
    }

    // body
    expect(LexerTokenType::LCURLY, "{");

    scopes.push(new Scope(scopes.top()));
    scopes.top()->isFunctionScope = true;
    scopes.top()->fnScopeParams = decl->fnDeclData.params;
    decl->fnDeclData.bodyScope = scopes.top();

    auto savedStaticIfScope = this->staticIfScope;
    this->staticIfScope = scopes.top();

    if (!noIppFlag && mainFn == decl) {
        initContext(decl);
    }

    while (!lexer->isEmpty() && lexer->front.type != LexerTokenType::RCURLY) {
        auto scopedStmt = parseScopedStmt();
        if (scopedStmt != nullptr) {
            vector_append(decl->fnDeclData.body, scopedStmt);
        }
    }
    appendEndScope(decl->fnDeclData.body);
    auto end = expect(LexerTokenType::RCURLY, "}");

    decl->region.end = end.region.end;

    this->staticIfScope = savedStaticIfScope;

    scopes.pop();
    scopes.pop();
    currentFnDecl = savedCurrentFnDecl;

    return decl;
}

Node *Parser::parseAttr() {
    expect(LexerTokenType::ATTR, "#attr");

    auto attrDecl = new Node(lexer->srcInfo, NodeType::ATTR, scopes.top());

    expect(LexerTokenType::LPAREN, "(");
    attrDecl->attrData.target = parseType();
    expect(LexerTokenType::RPAREN, ")");

    if (lexer->front.type == LexerTokenType::LCURLY) {
        popFront(); // '{'

        scopes.push(new Scope(scopes.top()));

        while (lexer->front.type != LexerTokenType::RCURLY) {
            vector_append(attrDecl->attrData.stmts, parseScopedStmt());
        }

        scopes.pop();

        expect(LexerTokenType::RCURLY, "}");
    }
    else {
        vector_append(attrDecl->attrData.stmts, parseScopedStmt());
    }

    vector_append(*impls, attrDecl);

    return attrDecl;
}

Node *Parser::parseImport() {
    auto saved = lexer->front.region.start;
    expect(LexerTokenType::IMPORT, "import");

    auto importNode = new Node(this->lexer->srcInfo, NodeType::IMPORT, scopes.top());

    if (this->lexer->front.type == LexerTokenType::DOUBLE_QUOTE) {
        auto importName = parseStringLiteral();

        if (lexer->front.type == LexerTokenType::COLON) {
            popFront(); // :
            importNode->importData.alias = parseSymbol();
        }

        importNode->importData.isFile = true;
        importNode->importData.target = addImport(*importName->stringLiteralData.value, importNode->importData.alias);
    }
    else {
        importNode->importData.isFile = false;
        importNode->importData.target = parseLvalue();
    }

    expectSemicolon();

    importNode->region = Region{lexer->srcInfo, saved, lexer->front.region.end};

    vector_append(*this->imports, importNode);

    return importNode;
}

Node *Parser::parseDefer() {
    auto saved = lexer->front.region.start;
    popFront();

    auto node = new Node(lexer->srcInfo, NodeType::DEFER, scopes.top());

    expect(LexerTokenType::LCURLY, "{");

    while (lexer->front.type != LexerTokenType::RCURLY) {
        auto deferredStmt = parseScopedStmt();
        vector_append(node->deferData.stmts, deferredStmt);
        vector_append(node->scope->deferredStmts, deferredStmt);
    }

    node->region = Region{lexer->srcInfo, saved, lexer->front.region.end};
    expect(LexerTokenType::RCURLY, "}");

    return node;
}

Node *Parser::parseLink() {
    auto saved = lexer->front.region.start;
    popFront();

    auto node = new Node(lexer->srcInfo, NodeType::LINK, scopes.top());

    auto name = parseStringLiteral();
    node->linkData.name = name->stringLiteralData.value;
    expectSemicolon();

    node->region = Region{lexer->srcInfo, saved, name->region.end};

    return node;
}

Node *Parser::parseTypeDecl() {
    auto saved = lexer->front.region.start;

    expect(LexerTokenType::TYPE, "type");

    auto typeName = parseSymbol();

    vector_t<Node *> ctParams = {};
    scopes.push(new Scope(scopes.top()));

    if (lexer->front.type == LexerTokenType::LPAREN) {
        popFront(); // '('
        ctParams = parseDeclParams();
        expect(LexerTokenType::RPAREN, ")");
    }

    auto typeDecl = parseType();
    initTypeData(typeDecl);

    typeDecl->typeData.name = typeName;
    typeDecl->region = Region{lexer->srcInfo, saved, typeDecl->region.end};

    maybeAddAutoPolyForTypeDecl(ctParams, typeDecl->typeData.structTypeData.params);

    if (ctParams.length == 0) {
        scopes.pop();
        scopeInsert(typeName->symbolData.atomId, typeDecl);
        return typeDecl;
    }

    auto pt = new Node(typeDecl->region.srcInfo, NodeType::PARAMETERIZED_TYPE, typeDecl->scope);
    pt->region = typeDecl->region;
    pt->parameterizedTypeData.ctParams = ctParams;
    pt->parameterizedTypeData.typeDecl = typeDecl;

    for (auto param : ctParams) {
        scopeInsert(param->paramData.name->symbolData.atomId, param);
    }
    if (typeDecl->typeData.kind == NodeTypekind::STRUCT) {
        for (auto param : typeDecl->typeData.structTypeData.params) {
            scopeInsert(param->paramData.name->symbolData.atomId, param);
        }
    }

    scopes.pop();

    if (!isCopying) {
        scopeInsert(typeName->symbolData.atomId, pt);
    }

    return pt;
}

Node *Parser::parseModuleDecl() {
    auto saved = lexer->front.region.start;

    expect(LexerTokenType::MODULE, "module");

    auto moduleDecl = new Node(lexer->srcInfo, NodeType::MODULE, scopes.top());
    moduleDecl->moduleData.name = parseSymbol();
    scopeInsert(moduleDecl->moduleData.name->symbolData.atomId, moduleDecl);

    auto savedCurrentFnDecl = this->currentFnDecl;
    this->currentFnDecl = nullptr;

    scopes.push(new Scope(scopes.top()));
    moduleDecl->scope = scopes.top();

    expect(LexerTokenType::LCURLY, "{");

    while (lexer->front.type != LexerTokenType::RCURLY) {
        vector_append(moduleDecl->moduleData.stmts, parseTopLevel());
    }
    scopes.pop();

    moduleDecl->region = Region{lexer->srcInfo, saved, lexer->front.region.end};

    expect(LexerTokenType::RCURLY, "}");

    this->currentFnDecl = savedCurrentFnDecl;

    return moduleDecl;
}

Node *Parser::parseSymbol() {
    auto sym = new Node(lexer->srcInfo, NodeType::SYMBOL, scopes.top());

    if (lexer->front.type == LexerTokenType::COLON) {
        sym->symbolData.isAttr = true;
        popFront();
    }

    LexerToken front = expect(LexerTokenType::SYMBOL, "identifier");
    sym->region = front.region;
    sym->symbolData.atomId = atomTable->insert(sym->region);
    return sym;
}

Node *Parser::parseScopedStmt() {
    // comment
    if (lexer->front.type == LexerTokenType::COMMENT) {
        popFront();
        return nullptr;
    }

    // ret
    if (lexer->front.type == LexerTokenType::RETURN) {
        return parseReturn();
    }

    // run
    if (lexer->front.type == LexerTokenType::RUN) {
        return parseRun();
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
    if (lexer->front.type == LexerTokenType::IF || lexer->front.type == LexerTokenType::STATIC_IF) {
        return parseIf();
    }

    // while
    if (lexer->front.type == LexerTokenType::WHILE) {
        return parseWhile();
    }

    // #attr
    if (lexer->front.type == LexerTokenType::ATTR) {
        return parseAttr();
    }

    // #context
    if (lexer->front.type == LexerTokenType::CONTEXT) {
        return parseContext();
    }

    // puts
    if (lexer->front.type == LexerTokenType::PUTS) {
        return parsePuts();
    }

    // for
    if (lexer->front.type == LexerTokenType::FOR || lexer->front.type == LexerTokenType::STATIC_FOR) {
        return parseFor();
    }

    // module
    if (lexer->front.type == LexerTokenType::MODULE) {
        return parseModuleDecl();
    }

    // import
    if (lexer->front.type == LexerTokenType::IMPORT) {
        return parseImport();
    }

    // defer
    if (lexer->front.type == LexerTokenType::DEFER) {
        return parseDefer();
    }

    // link
    if (lexer->front.type == LexerTokenType::LINK) {
        return parseLink();
    }

    // declaration/assignment/combo
    auto saved = lexer->front.region.start;
    auto lvalue = parseRvalue();

    if (lvalue->type == NodeType::FN_CALL
        || lvalue->type == NodeType::PANIC
        || lvalue->type == NodeType::BINOP) {
        lvalue->sourceMapStatement = true;
        expectSemicolon();
        return lvalue;
    }

    bool isConstant = this->currentFnDecl == nullptr;

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
        else if (isConstant) {
            reportError("expected a value for constant declaration");
        }

        expectSemicolon();

        auto decl = new Node(lexer->srcInfo, NodeType::DECL, scopes.top());

        if (!isConstant) {
            addLocal(decl);
        }
        else {
            decl->type = NodeType::ALIAS;
        }

        decl->declData.lhs = lvalue;
        decl->declData.initialValue = rvalue;
        decl->declData.type = type;

        decl->region = {lexer->srcInfo, saved, last.region.end};
        decl->sourceMapStatement = true;

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

        auto decl = new Node(lexer->srcInfo, NodeType::DECL, scopes.top());

        if (!isConstant) {
            addLocal(decl);
        }
        else {
            decl->type = NodeType::ALIAS;
        }

        decl->declData.lhs = lvalue;
        decl->declData.initialValue = rvalue;

        decl->region = {lexer->srcInfo, saved, last.region.end};
        decl->sourceMapStatement = true;

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

        auto ass = new Node(lexer->srcInfo, NodeType::ASSIGN, scopes.top());

        ass->assignData.lhs = lvalue;
        ass->assignData.rhs = rvalue;
        ass->region = {lexer->srcInfo, saved, last.region.end};

        if (lvalue->type == NodeType::DEREF || lvalue->type == NodeType::DOT || lvalue->type == NodeType::ARRAY_INDEX) {
            addLocal(rvalue);
        }

        ass->sourceMapStatement = true;

        return ass;
    }

    reportError("expected a declaration or an assignment");
    exit(1);
}

Node *Parser::parseIf() {
    auto isStatic = lexer->front.type == LexerTokenType::STATIC_IF;

    auto saved = lexer->front.region.start;
    popFront();

    auto if_ = new Node(lexer->srcInfo, NodeType::IF, scopes.top());
    if_->region.start = saved;
    if_->ifData.condition = parseRvalue();
    if_->ifData.condition->sourceMapStatement = true;
    if_->ifData.isStatic = isStatic;
    if_->sourceMapStatement = true;
    if_->ifData.condition->sourceMapStatement = true;

    scopes.push(new Scope(scopes.top()));
    if_->ifData.ifScope = scopes.top();

    auto savedStaticIfScope = this->staticIfScope;
    auto savedImports = this->imports;
    auto savedContexts = this->contexts;
    auto savedContextInits = this->contextInits;

    if (isStatic) {
        this->staticIfScope = scopes.top();
        this->imports = &if_->ifData.trueImports;
        this->impls = &if_->ifData.trueImpls;
        this->contexts = &if_->ifData.trueContexts;
        this->contextInits = &if_->ifData.trueContextInits;
    }

    expect(LexerTokenType::LCURLY, "{");
    while (lexer->front.type != LexerTokenType::RCURLY) {
        auto scopedStmt = parseScopedStmt();
        if (scopedStmt != nullptr) {
            vector_append(if_->ifData.stmts, scopedStmt);
        }
    }
    appendEndScope(if_->ifData.stmts);

    if_->region.end = lexer->front.region.end;
    expect(LexerTokenType::RCURLY, "}");

    scopes.pop();

    if (isStatic) {
        this->staticIfScope = savedStaticIfScope;
        this->imports = &if_->ifData.falseImports;
        this->contexts = &if_->ifData.falseContexts;
        this->contextInits = &if_->ifData.falseContextInits;
    }

    if (lexer->front.type == LexerTokenType::ELSE) {
        popFront();

        scopes.push(new Scope(scopes.top()));
        if_->ifData.elseScope = scopes.top();

        if (isStatic) {
            this->staticIfScope = scopes.top();
        }

        auto isElseIf = lexer->front.type == LexerTokenType::IF || lexer->front.type == LexerTokenType::STATIC_IF;

        if (isElseIf) {
            auto elseIf = parseIf();
            vector_append(if_->ifData.elseStmts, elseIf);
            if_->region.end = elseIf->region.end;
        }
        else {
            expect(LexerTokenType::LCURLY, "{");
            while (lexer->front.type != LexerTokenType::RCURLY) {
                auto scopedStmt = parseScopedStmt();
                if (scopedStmt != nullptr) {
                    vector_append(if_->ifData.elseStmts, scopedStmt);
                }
            }
        }

        appendEndScope(if_->ifData.elseStmts);

        scopes.pop();

        if (isStatic) {
            this->staticIfScope = savedStaticIfScope;
        }

        if (!isElseIf) {
            if_->region.end = lexer->front.region.end;
            expect(LexerTokenType::RCURLY, "}");
        }
    }

    if (isStatic) {
        vector_append(this->staticIfScope->staticIfs, if_);
        this->imports = savedImports;
        this->contexts = savedContexts;
        this->contextInits = savedContextInits;
    }

    return if_;
}

Node *Parser::parseWhile() {
    auto saved = lexer->front.region.start;
    popFront();

    auto while_ = new Node(lexer->srcInfo, NodeType::WHILE, scopes.top());
    while_->region.start = saved;
    while_->whileData.condition = parseRvalue();

    while_->sourceMapStatement = true;
    while_->whileData.condition->sourceMapStatement = true;

    scopes.push(new Scope(scopes.top()));

    expect(LexerTokenType::LCURLY, "{");
    while (lexer->front.type != LexerTokenType::RCURLY) {
        auto scopedStmt = parseScopedStmt();
        if (scopedStmt) {
            vector_append(while_->whileData.stmts, scopedStmt);
        }
    }
    appendEndScope(while_->whileData.stmts);

    while_->region.end = lexer->front.region.end;
    expect(LexerTokenType::RCURLY, "}");

    scopes.pop();

    return while_;
}

Node *Parser::parseFor() {
    auto saved = lexer->front.region.start;

    auto isStatic = lexer->front.type == LexerTokenType::STATIC_FOR;
    popFront();

    // we get a new scope!
    scopes.push(new Scope(scopes.top()));

    auto for_ = new Node(lexer->srcInfo, NodeType::FOR, scopes.top());
    for_->region.start = saved;

    for_->forData.isStatic = isStatic;
    for_->forData.element_alias = parseSymbol();

    if (lexer->front.type == LexerTokenType::COMMA) {
        popFront();
        for_->forData.iterator_alias = parseSymbol();
    }

    expect(LexerTokenType::COLON, ":");

    for_->forData.target = parseRvalue();

    for_->sourceMapStatement = true;
    for_->forData.target->sourceMapStatement = true;

    expect(LexerTokenType::LCURLY, "{");
    while (lexer->front.type != LexerTokenType::RCURLY) {
        auto scopedStmt = parseScopedStmt();
        if (scopedStmt) {
            vector_append(for_->forData.stmts, scopedStmt);
        }
    }
    appendEndScope(for_->forData.stmts);

    for_->region.end = lexer->front.region.end;
    expect(LexerTokenType::RCURLY, "}");

    scopes.pop();

    return for_;
}

void Parser::possiblyInsertDeferreds(Node *node, bool isReturn) {
    if (node->scope->insertedDeferredStmts) {
        return;
    }
    node->scope->insertedDeferredStmts = true;

    auto scope = node->scope;
    if (isReturn) {
        while (!scope->isFunctionScope) {
            for (auto i = static_cast<int64_t>(scope->deferredStmts.length - 1); i >= 0; i--) {
                vector_append(node->preStmts, vector_at(scope->deferredStmts, static_cast<unsigned long>(i)));
            }
            scope = scope->parent;
        }
    }
    for (auto i = static_cast<int64_t>(scope->deferredStmts.length - 1); i >= 0; i--) {
        vector_append(node->preStmts, vector_at(scope->deferredStmts, static_cast<unsigned long>(i)));
    }
}

Node *Parser::parseReturn() {
    auto saved = lexer->front.region.start;
    expect(LexerTokenType::RETURN, "return");

    auto ret = new Node(lexer->srcInfo, NodeType::RETURN, scopes.top());

    if (lexer->front.type != LexerTokenType::SEMICOLON) {
        ret->retData.value = parseRvalue();
        ret->region = {lexer->srcInfo, saved, ret->retData.value->region.end};

        if (currentFnDecl) {
            vector_append(currentFnDecl->fnDeclData.returns, ret);
        }
    }
    else {
        ret->region = {lexer->srcInfo, saved, lexer->front.region.end};
    }

    expectSemicolon();

    ret->sourceMapStatement = true;

    possiblyInsertDeferreds(ret, true);

    return ret;
}

Node *Parser::buildDots(stack<Node *> rvalues) {
    auto top = rvalues.top();

    cpi_assert(!rvalues.empty());

    if (rvalues.size() == 1) {
        return top;
    }
    else if (top->type == NodeType::FN_CALL) {
        rvalues.pop();

        // create a new node (but reuse memory from top)
        top->fnCallData.fn = buildDots(rvalues);

        top->region = {lexer->srcInfo, top->fnCallData.fn->region.start, top->region.end};
        return top;
    } else if (top->type == NodeType::ARRAY_INDEX) {
        rvalues.pop();

        top->type = NodeType::ARRAY_INDEX;
        top->arrayIndexData.target = buildDots(rvalues);

        top->region = {lexer->srcInfo, top->arrayIndexData.target->region.start, top->region.end};
        return top;
    } else if (top->type == NodeType::SYMBOL || top->type == NodeType::INT_LITERAL) {
        rvalues.pop();

        auto dot = new Node(lexer->srcInfo,
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

void maybePushArrayIndexOrFnCalls(Parser *parser, stack<Node *> &rvalues) {
    while (parser->lexer->front.type == LexerTokenType::LPAREN
           || parser->lexer->front.type == LexerTokenType::NOT
           || parser->lexer->front.type == LexerTokenType::LSQUARE) {
        if (parser->lexer->front.type == LexerTokenType::LSQUARE) {
            auto arrIndex = parser->parseArrayIndex();
            rvalues.push(arrIndex);
        } else {
            auto fnCall = parser->parseFnCall();
            rvalues.push(fnCall);
        }
    }
}

Node *Parser::parseLvalueHelper(Node *symbol, Location saved) {
    stack<Node *> rvalues;
    rvalues.push(symbol);

    maybePushArrayIndexOrFnCalls(this, rvalues);

    // ('.' lvalue_simple)*
    while (lexer->front.type == LexerTokenType::DOT) {
        expect(LexerTokenType::DOT, ".");

        Node *sym;
        if (lexer->front.type == LexerTokenType::INT_LITERAL) {
            sym = parseIntLiteral();
        }
        else {
            sym = parseSymbol();
        }

        rvalues.push(sym);

        maybePushArrayIndexOrFnCalls(this, rvalues);
    }

    if (rvalues.size() == 1) {
        return rvalues.top();
    }

    auto dot = buildDots(rvalues);
    dot->region = {lexer->srcInfo, saved, last.region.end};
    return dot;
}

Node *Parser::parsePanic() {
    auto saved = lexer->front.region.start;

    popFront();
    auto panicNode = new Node(lexer->srcInfo, NodeType::PANIC, scopes.top());
    expect(LexerTokenType::LPAREN, "(");

    panicNode->sourceMapStatement = true;
    expect(LexerTokenType::RPAREN, ")");

    panicNode->region.start = saved;
    panicNode->region.end = lexer->front.region.start;

    return panicNode;
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
        auto deref = new Node(lexer->srcInfo, NodeType::DEREF, scopes.top());
        deref->nodeData = parseRvalueSimple();

        addLocal(deref->nodeData);

        return deref;
    }

    switch (lexer->front.type) {
        case LexerTokenType::SYMBOL: {
            auto symbol = parseSymbol();
            return parseLvalueHelper(symbol, saved);
        }
        case LexerTokenType::PANIC: {
            return parsePanic();
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

    if (saved.type == LexerTokenType::TYPEOF) {
        return parseTypeof();
    }

    if (saved.type == LexerTokenType::RETURNTYPEOF) {
        return parseReturnTypeof();
    }

    auto type = new Node(lexer->srcInfo, NodeType::TYPE, scopes.top());
    type->region = saved.region;

    switch (saved.type) {
        case LexerTokenType::BOOLEAN: {
            popFront();
            type->typeData.kind = NodeTypekind::BOOLEAN;
        } break;
        case LexerTokenType::U8: {
            popFront();
            type->typeData.kind = NodeTypekind::U8;
        } break;
        case LexerTokenType::I8: {
            popFront();
            type->typeData.kind = NodeTypekind::I8;
        } break;
        case LexerTokenType::U16: {
            popFront();
            type->typeData.kind = NodeTypekind::U16;
        } break;
        case LexerTokenType::I16: {
            popFront();
            type->typeData.kind = NodeTypekind::I16;
        } break;
        case LexerTokenType::U32: {
            popFront();
            type->typeData.kind = NodeTypekind::U32;
        } break;
        case LexerTokenType::I32: {
            popFront();
            type->typeData.kind = NodeTypekind::I32;
        } break;
        case LexerTokenType::U64: {
            popFront();
            type->typeData.kind = NodeTypekind::U64;
        } break;
        case LexerTokenType::I64: {
            popFront();
            type->typeData.kind = NodeTypekind::I64;
        } break;
        case LexerTokenType::F32: {
            popFront();
            type->typeData.kind = NodeTypekind::F32;
        } break;
        case LexerTokenType::F64: {
            popFront();
            type->typeData.kind = NodeTypekind::F64;
        } break;
        case LexerTokenType::NONE: {
            popFront();
            type->typeData.kind = NodeTypekind::NONE;
        } break;
        case LexerTokenType::FN: {
            popFront();
            initFnTypeData(type);
            expect(LexerTokenType::LPAREN, "(");

            type->typeData.fnTypeData.params = parseDeclParams();

            if (!noIppFlag) {
                addContextParameterForDecl(type->typeData.fnTypeData.params, scopes.top());
            }

            expect(LexerTokenType::RPAREN, ")");
            type->typeData.fnTypeData.returnType = parseType();
        } break;
        case LexerTokenType::SYMBOL: {
            auto typeName = parseLvalue();

            switch (typeName->type) {
                case NodeType::SYMBOL: {
                    type->typeData.kind = NodeTypekind::SYMBOL;
                    type->typeData.symbolTypeData.atomId = typeName->symbolData.atomId;
                } break;
                case NodeType::DOT: {
                    type->typeData.kind = NodeTypekind::DOT;
                    type->typeData.dotTypeData = typeName;
                } break;
                case NodeType::FN_CALL: {
                    type->typeData.kind = NodeTypekind::PARAMETERIZED;
                    type->typeData.polymorphTypeTypeData.value = typeName;
                } break;
                default: {
                    reportError("expected a symbol, dot or parameterized specialization for type name");
                }
            }
        } break;
        case LexerTokenType::STRUCT: {
            expect(LexerTokenType::STRUCT, "struct");

            expect(LexerTokenType::LCURLY, "{");

            initStructTypeData(type);
            type->typeData.structTypeData.params = parseDeclParams();

            type->region.end = lexer->front.region.end;
            expect(LexerTokenType::RCURLY, "}");
        } break;
        case LexerTokenType::ENUM: {
            popFront();

            initEnumTypeData(type);

            scopes.push(new Scope(scopes.top()));
            type->scope = scopes.top();

            if (lexer->front.type == LexerTokenType::LPAREN) {
                popFront(); // '('
                type->typeData.enumTypeData.type = parseType();
                expect(LexerTokenType::RPAREN, ")");
            }
            else {
                type->typeData.enumTypeData.type = new Node(NodeTypekind::I64);
            }

            expect(LexerTokenType::LCURLY, "{");
            type->typeData.enumTypeData.params = parseValueParams();

            for (auto p : type->typeData.enumTypeData.params) {
                if (p->paramData.name == nullptr) {
                    p->paramData.name = p->paramData.value;
                    p->paramData.value = nullptr;
                }

                scopeInsert(p->paramData.name->symbolData.atomId, p);
            }

            expect(LexerTokenType::RCURLY, "}");

            scopes.pop();
        } break;
        case LexerTokenType::UNION: {
            popFront();

            initStructTypeData(type);

            if (lexer->front.type == LexerTokenType::LPAREN) {
                popFront(); // '('
                type->typeData.structTypeData.unionTagType = parseType();
                expect(LexerTokenType::RPAREN, ")");
            }

            expect(LexerTokenType::LCURLY, "{");

            type->typeData.structTypeData.isSecretlyUnion = true;

            auto tagSymbol = new Node();
            tagSymbol->type = NodeType::SYMBOL;
            tagSymbol->symbolData.atomId = atomTable->insertStr("tag");

            auto tagParam = new Node(lexer->srcInfo, NodeType::DECL_PARAM, scopes.top());
            tagParam->paramData.index = 0;

            if (type->typeData.structTypeData.unionTagType != nullptr) {
                tagParam->paramData.type = type->typeData.structTypeData.unionTagType;
            }
            else {
                tagParam->paramData.type = new Node(NodeTypekind::I64);
            }
            tagParam->paramData.name = tagSymbol;

            vector_append(type->typeData.structTypeData.params, tagParam);

            auto declaredParams = parseDeclParams();
            for (auto dp : declaredParams) {
                dp->paramData.index += 1;
                vector_append(type->typeData.structTypeData.params, dp);
            }

            expect(LexerTokenType::RCURLY, "}");

            type->region.end = lexer->lastLoc;
            tagParam->region = type->region;
        } break;
        case LexerTokenType::MUL: {
            popFront();

            type->typeData.kind = NodeTypekind::POINTER;
            type->typeData.pointerTypeData.underlyingType = parseType();

            type->region.end = lexer->lastLoc;
        } break;
        case LexerTokenType::EXPOSED_AST: {
            type->region = lexer->front.region;
            popFront();
            type->typeData.kind = NodeTypekind::EXPOSED_AST;
        } break;
        case LexerTokenType::LSQUARE: {
            type->region.start = lexer->front.region.start;
            popFront(); // '['

            expect(LexerTokenType::RSQUARE, "]");

            auto elementType = parseType();
            type->region.end = elementType->region.end;

            auto arrayType = makeArrayType(elementType);
            type->typeData = arrayType->typeData;
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

    auto *node = new Node(lexer->srcInfo, NodeType::STRUCT_LITERAL, scopes.top());
    node->structLiteralData.params = parseValueParams();

    expect(LexerTokenType::RCURLY, "}");

    return node;
}

Node *Parser::parseArrayLiteral() {
    expect(LexerTokenType::LSQUARE, "[");

    Node *allocFn = nullptr;
    if (lexer->front.type != LexerTokenType::RSQUARE) {
        allocFn = parseRvalue();
    }

    expect(LexerTokenType::RSQUARE, "]");

    if (lexer->front.type == LexerTokenType::DOUBLE_QUOTE) {
        auto strLit = parseStringLiteral();
        strLit->stringLiteralData.allocFn = allocFn;

        return strLit;
    }

    auto lit = new Node(lexer->srcInfo, NodeType::ARRAY_LITERAL, scopes.top());
    lit->arrayLiteralData.allocFn = allocFn;

    if (lexer->front.type != LexerTokenType::LCURLY) {
        lit->arrayLiteralData.elementType = parseType();
    }

    expect(LexerTokenType::LCURLY, "{");
    if (lexer->front.type != LexerTokenType::RCURLY) {
        auto elem = parseRvalue();
        vector_append(lit->arrayLiteralData.elements, elem);
    }
    while (lexer->front.type != LexerTokenType::RCURLY) {
        expect(LexerTokenType::COMMA, ",");

        auto elem = parseRvalue();
        vector_append(lit->arrayLiteralData.elements, elem);
    }
    expect(LexerTokenType::RCURLY, "}");

    return lit;
}

Node *Parser::parseRun() {
    auto node = new Node(lexer->srcInfo, NodeType::RUN, scopes.top());
    node->region.start = lexer->front.region.start;
    popFront();

    auto savedFnDecl = currentFnDecl;
    currentFnDecl = nullptr;

    node->nodeData = parseRvalue();

    currentFnDecl = savedFnDecl;

    node->region.end = node->nodeData->region.end;
    return node;
}

Node *Parser::parseTypeof() {
    auto type = new Node(lexer->srcInfo, NodeType::TYPEOF, scopes.top());
    type->region.start = lexer->front.region.start;
    popFront();
    expect(LexerTokenType::LPAREN, "(");
    type->nodeData = parseRvalue();
    type->region.end = lexer->front.region.end;
    expect(LexerTokenType::RPAREN, ")");
    return type;
}

Node *Parser::parseReturnTypeof() {
    auto type = new Node(lexer->srcInfo, NodeType::RETURNTYPEOF, scopes.top());
    type->region.start = lexer->front.region.start;
    popFront();
    expect(LexerTokenType::LPAREN, "(");
    type->nodeData = parseRvalue();
    type->region.end = lexer->front.region.end;
    expect(LexerTokenType::RPAREN, ")");
    return type;
}

Node *Parser::parseSizeof() {
    auto type = new Node(lexer->srcInfo, NodeType::SIZEOF, scopes.top());
    type->region.start = lexer->front.region.start;
    popFront();
    expect(LexerTokenType::LPAREN, "(");
    type->nodeData = parseType();
    type->region.end = lexer->front.region.end;
    expect(LexerTokenType::RPAREN, ")");
    return type;
}

Node *Parser::parseIsKind() {
    auto type = new Node(lexer->srcInfo, NodeType::ISKIND, scopes.top());
    type->region.start = lexer->front.region.start;
    popFront();
    expect(LexerTokenType::LPAREN, "(");
    type->isKindData.type = parseType();
    expect(LexerTokenType::COMMA, ",");

    if (lexer->front.type == LexerTokenType::I8
        || lexer->front.type == LexerTokenType::I16
        || lexer->front.type == LexerTokenType::I32
        || lexer->front.type == LexerTokenType::I64
        || lexer->front.type == LexerTokenType::F32
        || lexer->front.type == LexerTokenType::F64
        || lexer->front.type == LexerTokenType::BOOLEAN
        || lexer->front.type == LexerTokenType::FN
        || lexer->front.type == LexerTokenType::STRUCT
        || lexer->front.type == LexerTokenType::UNION
        || lexer->front.type == LexerTokenType::NONE
        || lexer->front.type == LexerTokenType::ENUM
        || (lexer->front.type == LexerTokenType::LSQUARE && lexer->next.type == LexerTokenType::RSQUARE)
        || lexer->front.type == LexerTokenType::MUL) {
        type->isKindData.tokenType = lexer->front.type;
        popFront();
        if (lexer->front.type == LexerTokenType::RSQUARE) {
            popFront();
        }
    }
    else {
        lexer->reportError(Error{type->region, "invalid kind symbol for #iskind"});
    }

    expect(LexerTokenType::RPAREN, ")");

    return type;
}

Node *Parser::parseAttrof() {
    auto a = new Node(lexer->srcInfo, NodeType::ATTROF, scopes.top());
    a->region.start = lexer->front.region.start;
    popFront(); // '#attrof'
    expect(LexerTokenType::LPAREN, "(");
    a->attrofData.target = parseType();
    expect(LexerTokenType::COMMA, ",");
    a->attrofData.attr = parseSymbol();
    a->region.end = lexer->front.region.end;
    expect(LexerTokenType::RPAREN, ")");
    return a;
}

Node *Parser::parseHasAttr() {
    auto a = new Node(lexer->srcInfo, NodeType::HASATTR, scopes.top());
    a->region.start = lexer->front.region.start;
    popFront(); // '#hasattr'
    expect(LexerTokenType::LPAREN, "(");
    a->attrofData.target = parseType();
    expect(LexerTokenType::COMMA, ",");
    a->attrofData.attr = parseSymbol();
    a->region.end = lexer->front.region.end;
    expect(LexerTokenType::RPAREN, ")");
    return a;
}

Node *Parser::parseFieldsof() {
    auto type = new Node(lexer->srcInfo, NodeType::FIELDSOF, scopes.top());
    type->region.start = lexer->front.region.start;
    popFront(); // '#fieldsof'
    expect(LexerTokenType::LPAREN, "(");
    type->nodeData = parseType();
    type->region.end = lexer->front.region.end;
    expect(LexerTokenType::RPAREN, ")");
    return type;
}

Node *Parser::parsePuts() {
    auto value = new Node(lexer->srcInfo, NodeType::PUTS, scopes.top());
    value->sourceMapStatement = true;
    value->region.start = lexer->front.region.start;
    popFront();
    expect(LexerTokenType::LPAREN, "(");
    value->nodeData = parseRvalue();
    value->region.end = lexer->front.region.end;
    expect(LexerTokenType::RPAREN, ")");
    expectSemicolon();

    return value;
}

Node *Parser::parseTagCheck() {
    auto check = new Node(lexer->srcInfo, NodeType::TAGCHECK, scopes.top());
    check->region.start = lexer->front.region.start;
    popFront();
    expect(LexerTokenType::LPAREN, "(");
    check->nodeData = parseRvalue();
    check->region.end = lexer->front.region.end;
    expect(LexerTokenType::RPAREN, ")");
    return check;
}

Node *Parser::parseLvalueOrLiteral() {
    // comment
    while (lexer->front.type == LexerTokenType::COMMENT) {
        popFront();
    }

    if (lexer->front.type == LexerTokenType::U8 || lexer->front.type == LexerTokenType::I8
        || lexer->front.type == LexerTokenType::U16 || lexer->front.type == LexerTokenType::I16
        || lexer->front.type == LexerTokenType::U32 || lexer->front.type == LexerTokenType::I32
        || lexer->front.type == LexerTokenType::U64 || lexer->front.type == LexerTokenType::I64
        || lexer->front.type == LexerTokenType::F32 || lexer->front.type == LexerTokenType::F64
        || lexer->front.type == LexerTokenType::BOOLEAN) {
        return parseType();
    }

    if (lexer->front.type == LexerTokenType::SIZEOF) {
        return parseSizeof();
    }

    if (lexer->front.type == LexerTokenType::TAGCHECK) {
        return parseTagCheck();
    }

    if (lexer->front.type == LexerTokenType::ISKIND) {
        return parseIsKind();
    }

    if (lexer->front.type == LexerTokenType::HASATTR) {
        return parseHasAttr();
    }

    auto saved = lexer->front.region.start;
    Node *symbol = nullptr;

    if (lexer->front.type == LexerTokenType::LPAREN) {
        popFront();
        symbol = parseRvalue();
        expect(LexerTokenType::RPAREN, ")");
    } else if (lexer->front.type == LexerTokenType::SUB) {
        // negative of a literal
        // todo(chad): @robustness
        popFront();
        if (lexer->front.type == LexerTokenType::INT_LITERAL) {
            symbol = parseIntLiteral();
            symbol->intLiteralData.value = -symbol->intLiteralData.value;
        } else if (lexer->front.type == LexerTokenType::FLOAT_LITERAL) {
            symbol = parseFloatLiteral();
            symbol->floatLiteralData.value = -symbol->floatLiteralData.value;
        } else {
            auto target = parseRvalueSimple();
            symbol = new Node();
            symbol->type = NodeType::UNARY_NEG;
            symbol->unaryNegData.target = target;

            symbol->unaryNegData.rewritten = new Node(target->region.srcInfo, NodeType::BINOP, target->scope);
            symbol->unaryNegData.rewritten->binopData.type = LexerTokenType::SUB;
            symbol->unaryNegData.rewritten->binopData.lhs = new Node(target->region.srcInfo, NodeType::INT_LITERAL, target->scope);
            symbol->unaryNegData.rewritten->binopData.lhs->intLiteralData.value = 0;
            symbol->unaryNegData.rewritten->binopData.rhs = target;

            addLocal(symbol->unaryNegData.rewritten);

            addLocal(target);

            symbol->region.srcInfo = target->region.srcInfo;
            symbol->region.start = saved;
            symbol->region.end = target->region.end;

            symbol->unaryNegData.rewritten->region = symbol->region;
        }
    } else if (lexer->front.type == LexerTokenType::INT_LITERAL) {
        symbol = parseIntLiteral();
    } else if (lexer->front.type == LexerTokenType::FLOAT_LITERAL) {
        symbol = parseFloatLiteral();
    } else if (lexer->front.type == LexerTokenType::DOUBLE_QUOTE) {
        symbol = parseStringLiteral();
    } else if (lexer->front.type == LexerTokenType::FN) {
        symbol = parseFnDecl();
        symbol->fnDeclData.isLiteral = true;
    } else if (lexer->front.type == LexerTokenType::TRUE_) {
        symbol = new Node(lexer->srcInfo, NodeType::BOOLEAN_LITERAL, scopes.top());
        symbol->boolLiteralData.value = true;
        symbol->region = lexer->front.region;
        popFront();
    } else if (lexer->front.type == LexerTokenType::FALSE_) {
        symbol = new Node(lexer->srcInfo, NodeType::BOOLEAN_LITERAL, scopes.top());
        symbol->boolLiteralData.value = false;
        symbol->region = lexer->front.region;
        popFront();
    } else if (lexer->front.type == LexerTokenType::LCURLY) {
        symbol = parseStructLiteral();
    } else if (lexer->front.type == LexerTokenType::RUN) {
        symbol = parseRun();
    } else if (lexer->front.type == LexerTokenType::TYPEOF) {
        symbol = parseTypeof();
    } else if (lexer->front.type == LexerTokenType::NIL) {
        symbol = new Node();
        symbol->region = lexer->front.region;
        symbol->type = NodeType::NIL_LITERAL;
        popFront();
    } else if (lexer->front.type == LexerTokenType::LSQUARE) {
        symbol = parseArrayLiteral();
    } else if (lexer->front.type == LexerTokenType::FIELDSOF) {
        symbol = parseFieldsof();
    } else if (lexer->front.type == LexerTokenType::PANIC) {
        symbol = parsePanic();
    } else if (lexer->front.type == LexerTokenType::ATTROF) {
        symbol = parseAttrof();
    } else {
        symbol = parseSymbol();
    }

    // comment
    while (lexer->front.type == LexerTokenType::COMMENT) {
        popFront();
    }

    cpi_assert(symbol != nullptr);
    return parseLvalueHelper(symbol, saved);
}

Node *Parser::parseRvalueSimple() {
    auto saved = lexer->front.region.start;

    // & (rvalue_simple)
    if (lexer->front.type == LexerTokenType::AMP) {
        popFront();
        auto addrOf = new Node(lexer->srcInfo, NodeType::ADDRESS_OF, scopes.top());
        addrOf->nodeData = parseRvalueSimple();
        addrOf->region = Region{lexer->srcInfo, saved, addrOf->nodeData->region.end};

        addLocal(addrOf->nodeData);

        return addrOf;
    }

    // ^ (rvalue_simple)
    if (lexer->front.type == LexerTokenType::DEREF) {
        popFront();
        auto deref = new Node(lexer->srcInfo, NodeType::DEREF, scopes.top());

        deref->nodeData = parseRvalueSimple();

        addLocal(deref->nodeData);

        deref->region = Region{lexer->srcInfo, saved, deref->nodeData->region.end};
        return deref;
    }

    // ~!(rvalue_simple)
    if (lexer->front.type == LexerTokenType::BITNOT) {
        popFront();

        auto _not = new Node(lexer->srcInfo, NodeType::UNARY_BITNOT, scopes.top());
        _not->nodeData = parseRvalueSimple();

        addLocal(_not);

        _not->region = Region{lexer->srcInfo, saved, _not->nodeData->region.end};
        return _not;
    }

    // !(rvalue_simple)
    if (lexer->front.type == LexerTokenType::NOT) {
        popFront();

        auto _not = new Node(lexer->srcInfo, NodeType::UNARY_NOT, scopes.top());
        _not->nodeData = parseRvalueSimple();

        addLocal(_not);

        _not->region = Region{lexer->srcInfo, saved, _not->nodeData->region.end};
        return _not;
    }

    // cast
    if (lexer->front.type == LexerTokenType::CAST) {
        auto cast = new Node(lexer->srcInfo, NodeType::CAST, scopes.top());
        cast->region.start = lexer->front.region.start;

        popFront();

        expect(LexerTokenType::LPAREN, "(");
        if (lexer->front.type != LexerTokenType::RPAREN)
            cast->castData.type = parseType();
        expect(LexerTokenType::RPAREN, ")");

        cast->castData.value = parseRvalueSimple();
        addLocal(cast->castData.value);

        cast->region.end = cast->castData.value->region.end;

        addLocal(cast);

        return cast;
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
    auto node = new Node(lexer->srcInfo, NodeType::INT_LITERAL, scopes.top());
    node->region = front.region;

    ostringstream s("");
    for (auto i = node->region.start.byteIndex; i < node->region.end.byteIndex; i++) {
        if (node->region.srcInfo.source->at(i) != '_') {
            s << node->region.srcInfo.source->at(i);
        }
    }

    auto str = s.str();

    if (str.length() > 2 && str[0] == '0' && str[1] == 'x') {
        node->intLiteralData.value = stol(str.substr(2), nullptr, 16);
    }
    else if (str.length() > 2 && str[0] == '0' && str[1] == 'b') {
        node->intLiteralData.value = stol(str.substr(2), nullptr, 2);
    }
    else {
        node->intLiteralData.value = stol(str);
    }

    return node;
}

Node *Parser::parseFloatLiteral() {
    auto front = expect(LexerTokenType::FLOAT_LITERAL, "floating point literal");
    auto node = new Node(lexer->srcInfo, NodeType::FLOAT_LITERAL, scopes.top());
    node->region = front.region;

    ostringstream s("");
    for (auto i = node->region.start.byteIndex; i < node->region.end.byteIndex; i++) {
        if (lexer->srcInfo.source->at(i) != '_') {
            s << lexer->srcInfo.source->at(i);
        }
    }

    auto toConvert = s.str();
    node->floatLiteralData.value = stod(toConvert);

    return node;
}

Node *Parser::parseStringLiteral() {
    auto strTok = expect(LexerTokenType::DOUBLE_QUOTE, "\"");

    auto node = new Node(lexer->srcInfo, NodeType::STRING_LITERAL, scopes.top());
    node->region = strTok.region;

    ostringstream s("");
    for (auto i = node->region.start.byteIndex + 1; i < node->region.end.byteIndex - 1; i++) {
        if (node->region.srcInfo.source->at(i) == '\\') {
            cpi_assert(node->region.srcInfo.source->size() > i + 1);
            switch (node->region.srcInfo.source->at(i + 1)) {
                case 'n': {
                    s << '\n';
                } break;
                case '0': {
                    s << '\0';
                } break;
                case '"': {
                    s << '"';
                    i = i + 1;
                } break;
                default: {
                    reportError("unexpected escape sequence");
                } break;
            }
            i = i + 1;
        }
        else {
            s << node->region.srcInfo.source->at(i);
        }
    }

    node->stringLiteralData.value = new string(s.str());
    node->stringLiteralData.allocFn = nullptr;

    return node;
}

int8_t Parser::operatorPrecedence(LexerTokenType type) {
    switch (type) {
        case LexerTokenType::AND:
        case LexerTokenType::OR:
        case LexerTokenType::ADDEQ:
        case LexerTokenType::SUBEQ:
        case LexerTokenType::MULEQ:
        case LexerTokenType::DIVEQ:
            return 1;

        case LexerTokenType::EQ_EQ:
        case LexerTokenType::NE:
        case LexerTokenType::LE:
        case LexerTokenType::GE:
        case LexerTokenType::LT:
        case LexerTokenType::GT:
        case LexerTokenType::BITAND:
        case LexerTokenType::BITOR:
        case LexerTokenType::BITXOR:
        case LexerTokenType::BITSHL:
        case LexerTokenType::BITSHR:
        case LexerTokenType::MOD:
            return 2;

        case LexerTokenType::SUB:
        case LexerTokenType::ADD:
            return 3;

        case LexerTokenType::DIV:
        case LexerTokenType::MUL:
            return 4;

        case LexerTokenType::VERTICAL_BAR:
            return 5;

        default: cpi_assert(false);
    }
}

Node *Parser::unwindPolish(stack<ShuntingYard> *values) {
    auto top = values->top();
    values->pop();

    if (top.type == ShuntingYardType::NODE) {
        return top.data.node;
    } else {
        auto binop = new Node(lexer->srcInfo, NodeType::BINOP, scopes.top());

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
        vector_append(currentFnDecl->fnDeclData.locals, local);
    }
}

Node *Parser::parseArrayIndex() {
    auto node = new Node(lexer->srcInfo, NodeType::ARRAY_INDEX, scopes.top());

    expect(LexerTokenType::LSQUARE, "[");
    node->arrayIndexData.indexValue = parseRvalue();
    addLocal(node->arrayIndexData.indexValue);
    expect(LexerTokenType::RSQUARE, "]");

    node->region.end = last.region.end;

    return node;
}

Node *Parser::parseContext() {
    expect(LexerTokenType::CONTEXT, "#context");

    auto context = new Node(lexer->srcInfo, NodeType::CONTEXT, scopes.top());

    if (lexer->front.type == LexerTokenType::LCURLY) {
        popFront();

        while (lexer->front.type != LexerTokenType::RCURLY) {
            auto param = parseDeclParam();
            param->paramData.isContext = true;
            vector_append(context->contextData.decls, param);
        }

        expect(LexerTokenType::RCURLY, "}");
    }
    else {
        auto param = parseDeclParam();
        param->paramData.isContext = true;
        vector_append(context->contextData.decls, param);
    }

    context->region.start = vector_at(context->contextData.decls, 0)->region.start;
    context->region.end = vector_at(context->contextData.decls, context->contextData.decls.length - 1)->region.end;

    vector_append(*this->contexts, context);

    return context;
}

Node *Parser::parseContextInit() {
    expect(LexerTokenType::CONTEXT_INIT, "#contextinit");

    auto fn = parseRvalue();
//    vector_append(*this->contextInits, fn);
    return fn;
}

Node *Parser::parseFnCall() {
    auto call = new Node(lexer->srcInfo, NodeType::FN_CALL, scopes.top());
    call->sourceMapStatement = true;

    if (lexer->front.type == LexerTokenType::NOT) {
        expect(LexerTokenType::NOT, "!");

        if (lexer->front.type == LexerTokenType::LPAREN) {
            expect(LexerTokenType::LPAREN, "(");
            call->fnCallData.ctParams = parseValueParams();
            expect(LexerTokenType::RPAREN, ")");
        }
        else {
            vector_append(call->fnCallData.ctParams, wrapInValueParam(parseLvalueOrLiteral(), nullptr));
        }
    }

    if (lexer->front.type == LexerTokenType::LPAREN) {
        expect(LexerTokenType::LPAREN, "(");
        call->fnCallData.params = parseValueParams();
        expect(LexerTokenType::RPAREN, ")");
        call->fnCallData.hasRuntimeParams = true;
    } else {
        call->fnCallData.hasRuntimeParams = false;
    }

    call->region.end = last.region.end;

    return call;
}
