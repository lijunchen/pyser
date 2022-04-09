#include "Parser.h"
#include <memory>

using std::make_unique;

unordered_set<string> Parser::keywords = {
    "and",
    "or",
    "not",
    "is",
    "in",
};

unique_ptr<Module> Parser::file() {
    int p = mark();
    optional<stmtPs> stmts = statements();
    if (stmts) {
        return std::make_unique<Module>(move(*stmts));
    }
    reset(p);
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

    if (expect(Token::Type::NEWLINE)) {
        if (!expect(Token::Type::INDENT)) {
            printf("expect indent after newline in block\n");
            reset(p);
            return nullopt;
        }
        stmts = statements();
        expect(Token::Type::DEDENT);
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
    stmtP stmt;
    if ((stmt = function_def())) {
        return stmt;
    }
    reset(p);
    if ((stmt = if_stmt())) {
        return stmt;
    }
    reset(p);
    if ((stmt = class_def())) {
        return stmt;
    }
    reset(p);
    if ((stmt = with_stmt())) {
        return stmt;
    }
    reset(p);
    if ((stmt = for_stmt())) {
        return stmt;
    }
    reset(p);
    if ((stmt = try_stmt())) {
        return stmt;
    }
    reset(p);
    if ((stmt = while_stmt())) {
        return stmt;
    }
    reset(p);
    if ((stmt = match_stmt())) {
        return stmt;
    }
    reset(p);
    return nullptr;
}

stmtP Parser::function_def() { return nullptr; }

stmtP Parser::if_stmt() {
    printf("if stmt\n");
    return nullptr;
}

stmtP Parser::class_def() { return nullptr; }
stmtP Parser::with_stmt() { return nullptr; }
stmtP Parser::for_stmt() { return nullptr; }
stmtP Parser::try_stmt() { return nullptr; }

/*
while_stmt:
    | 'while' named_expression ':' block [else_block] 
*/
stmtP Parser::while_stmt() {
    printf("while stmt\n");
    int p = mark();
    if (expect("while")) {
        exprP test = atom();
        if (expect(Token::Type::COLON)) {
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

stmtP Parser::match_stmt() { return nullptr; }

optional<stmtPs> Parser::simple_stmts() {
    printf("simple stmts\n");
    int p = mark();

    stmtPs stmts;
    stmtP stmt;
    if ((stmt = simple_stmt())) {
        stmts.push_back(move(stmt));
        int p1 = mark();
        while ((p = mark()) && expect(Token::Type::SEMI) && (stmt = simple_stmt())) {
            stmts.push_back(move(stmt));
        }
        reset(p1);
        expect(Token::Type::SEMI);
        if (!expect(Token::Type::NEWLINE)) {
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

    stmtP stmt;
    if ((stmt = assignment())) {
        return stmt;
    }
    reset(p);
    if ((stmt = star_expressions())) {
        return stmt;
    }
    reset(p);
    if ((stmt = return_stmt())) {
        return stmt;
    }
    reset(p);
    if ((stmt = import_stmt())) {
        return stmt;
    }
    reset(p);
    if ((stmt = raise_stmt())) {
        return stmt;
    }
    reset(p);
    if ((stmt = pass_stmt())) {
        return stmt;
    }
    reset(p);
    if ((stmt = del_stmt())) {
        return stmt;
    }
    reset(p);
    if ((stmt = yield_stmt())) {
        return stmt;
    }
    reset(p);
    if ((stmt = assert_stmt())) {
        return stmt;
    }
    reset(p);
    if ((stmt = break_stmt())) {
        return stmt;
    }
    reset(p);
    if ((stmt = continue_stmt())) {
        return stmt;
    }
    reset(p);
    if ((stmt = global_stmt())) {
        return stmt;
    }
    reset(p);
    if ((stmt = nonlocal_stmt())) {
        return stmt;
    }
    reset(p);
    return nullptr;
}

// assignment:
//     | NAME ':' expression ['=' annotated_rhs ] 
//     | ('(' single_target ')' 
//          | single_subscript_attribute_target) ':' expression ['=' annotated_rhs ] 
//     | (star_targets '=' )+ (yield_expr | star_expressions) !'=' [TYPE_COMMENT] 
//     | single_target augassign ~ (yield_expr | star_expressions) 
stmtP Parser::assignment() {
    int p = mark();

    reset(p);
    return nullptr;
}

// star_expressions:
//     | star_expression (',' star_expression )+ [','] 
//     | star_expression ',' 
//     | star_expression
stmtP Parser::star_expressions() {
    int p = mark();
    if (exprP e = pratt_parser()) {
        return make_unique<Expr>(move(e));
    }
    reset(p);
    return nullptr;
}

// star_expression:
//     | '*' bitwise_or 
//     | expression
// stmtP Parser::star_expression() {}

stmtP Parser::return_stmt() { return nullptr; }

stmtP Parser::import_stmt() { return nullptr; }

stmtP Parser::raise_stmt() { return nullptr; }

stmtP Parser::pass_stmt() { return nullptr; }

stmtP Parser::del_stmt() { return nullptr; }

stmtP Parser::yield_stmt() { return nullptr; }

stmtP Parser::assert_stmt() { return nullptr; }

stmtP Parser::break_stmt() { return nullptr; }

stmtP Parser::continue_stmt() { return nullptr; }

stmtP Parser::global_stmt() { return nullptr; }

stmtP Parser::nonlocal_stmt() { return nullptr; }

exprP Parser::atom() {
    int p = mark();
    const Token& t = peek();
    if (keywords.count(t.raw)) {
        return nullptr;
    }
    printf("atom rule\n");
    if (const Token& t = expectT(Token::Type::NAME)) {
        if (t.raw == "True") {
            return make_unique<Bool>("True");
        }
        if (t.raw == "False") {
            return make_unique<Bool>("False");
        }
        if (t.raw == "None") {
            return make_unique<None>();
        }

        printf("atom name: %s\n", t.raw.c_str());
        return make_unique<Name>(t.raw, expr_context::Load);
    }
    if (const Token& t = expectT(Token::Type::STRING)) {
        return make_unique<Str>(t.raw, nullopt);
    }
    if (const Token& t = expectT(Token::Type::NUMBER)) {
        return make_unique<Num>(t.raw);
    }
    reset(p);
    return nullptr;
}

exprP Parser::slices() {
    int p = mark();
    exprP s;

    if ((s = slice()) && !lookahead(Token::Type::COMMA)) {
        return s;
    }

    reset(p);
    exprPs ss;
    if ((s = slice()) && lookahead(Token::Type::COMMA)) {
        ss.push_back(move(s));
        int p1 = mark();
        while ((p1 = mark()) && expect(Token::Type::COMMA) && (s = slice())) {
            ss.push_back(move(s));
        }
        reset(p1);
        expect(Token::Type::COMMA);
        return make_unique<Tuple>(move(ss), expr_context::Load);
    }

    reset(p);
    return nullptr;
}

exprP Parser::slice() {
    int p = mark();
    exprP lower;
    exprP upper;
    exprP step;

    if ((lower = pratt_parser(), true) && expect(Token::Type::COLON)) {
        if ((upper = pratt_parser(), true)) {
            if (expect(Token::Type::COLON) && (step = pratt_parser(), true)) {
                return make_unique<Slice>(move(lower), move(upper), move(step));
            }
            return make_unique<Slice>(move(lower), move(upper), nullptr);
        }
        return make_unique<Slice>(move(lower), nullptr, nullptr);
    }
    reset(p);

    exprP e;
    if (exprP e = pratt_parser()) {
        return e;
    }

    reset(p);
    return nullptr;
}