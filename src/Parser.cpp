#include "Parser.h"
#include <memory>

using std::make_unique;

unique_ptr<Module> Parser::file() {
    int p = mark();
    optional<stmtPs> stmts = statements();
    if (stmts) {
        return std::make_unique<Module>(move(*stmts));
    }
    return nullptr;
}

optional<stmtPs> Parser::statements() {
    printf("statements\n");
    int p = mark();

    stmtPs stmts;
    if (stmtP stmt = statement()) {
        stmts.push_back(move(stmt));
        while (stmtP stmt = statement()) {
            stmts.push_back(move(stmt));
        }
        return stmts;
    }

    reset(p);
    return nullopt;
}

stmtP Parser::statement() {
    printf("statement\n");
    int p = mark();
    stmtP s;

    if ((s = compound_stmt())) {
        return s;
    }

    reset(p);
    if ((s = simple_stmt())) {
        return s;
    }

    reset(p);
    return nullptr;
}

/*
block:
    | NEWLINE INDENT statements DEDENT 
    | simple_stmts
*/
optional<stmtPs> Parser::block() {
    printf("block\n");
    int p = mark();
    optional<stmtPs> stmts;

    if (expect(TokenType::NEWLINE)) {
        if (!expect(TokenType::INDENT)) {
            printf("expect indent after newline in block\n");
            reset(p);
            return nullopt;
        }
        stmts = statements();
        expect(TokenType::DEDENT);
        return stmts;
    }

    reset(p);
    if ((stmts = simple_stmts())) {
        return stmts;
    }

    reset(p);
    return nullopt;
}

stmtP Parser::compound_stmt() {
    printf("compound stmt\n");
    int p = mark();
    optional<stmtPs> stmts;

    reset(p);
    return nullptr;
}

stmtP Parser::if_stmt() {
    printf("if stmt\n");
    return nullptr;
}

/*
while_stmt:
    | 'while' named_expression ':' block [else_block] 
*/
stmtP Parser::while_stmt() {
    printf("while stmt\n");
    int p = mark();
    if (expect("while")) {
        exprP test = atom();
        if (expect(TokenType::COLON)) {
            optional<stmtPs> body = block();
            stmtPs orelse;
            if (body) {
                printf("parse while succ\n");
                return make_unique<While>(move(test), move(*body), move(orelse));
            }
            printf("parse while fail\n");
        }
    }
    reset(p);
    return nullptr;
}

optional<stmtPs> Parser::simple_stmts() {
    printf("simple stmts\n");
    int p = mark();

    stmtPs stmts;
    stmtP stmt;
    if ((stmt = simple_stmt())) {
        stmts.push_back(move(stmt));
        int p1 = mark();
        while ((p = mark()) && expect(TokenType::SEMI) && (stmt = simple_stmt())) {
            stmts.push_back(move(stmt));
        }
        reset(p1);
        expect(TokenType::SEMI);
        if (!expect(TokenType::NEWLINE)) {
            reset(p);
            return nullopt;
        }
        return stmts;
    }

    reset(p);
    return nullopt;
}

stmtP Parser::simple_stmt() {
    int p = mark();
    if (exprP e = pratt_parser()) {
        return make_unique<Expr>(move(e));
    }
    reset(p);
    return nullptr;
}

exprP Parser::atom() {
    int p = mark();
    const Token& t = peek();
    printf("atom rule\n");
    if (const Token& t = expectT(TokenType::NUMBER)) {
        return make_unique<Num>(t.raw);
    }
    if (const Token& t = expectT(TokenType::STRING)) {
        return make_unique<Str>(t.raw, nullopt);
    }
    if (const Token& t = expectT(TokenType::NAME)) {
        return make_unique<Name>(t.raw, expr_context::Load);
    }
    reset(p);
    return nullptr;
}
