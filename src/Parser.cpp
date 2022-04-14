#include "Parser.h"
#include <memory>

using std::make_unique;

template<class Fn>
struct defer {
    Fn fn;
    ~defer() { fn(); }
};
template<class Fn> defer(Fn) -> defer<Fn>;

unordered_set<string> Parser::keywords = {
    "and",
    "or",
    "not",
    "is",
    "in",
    "yield",
    "from",
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
        exprP test = expression();
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
    if (exprP e = star_expressions()) {
        return make_unique<Expr>(move(e));
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

/*
case 1:
    NAME ':' expression ['=' annotated_rhs ] 
    a: int = xxx

*/

stmtP Parser::assignment() {
    int p = mark();
    reset(p);

    exprP lhs;
    if ((lhs = expression()) && expect(Token::Type::COLON)) {
        if (exprP anno = expression()) {
            int p2 = mark();
            exprP value;
            if (expect(Token::Type::EQUAL) && (value = annotated_rhs())) {
                return make_unique<AnnAssign>(move(lhs), move(anno), move(value), 0);
            }
            reset(p2);
            return make_unique<AnnAssign>(move(lhs), nullptr, move(value), 0);
        }
    }

    reset(p);
    return nullptr;
}

// annotated_rhs: yield_expr | star_expressions
exprP Parser::annotated_rhs() {
    int p = mark();
    if (exprP e = yield_expr()) {
        return e;
    }
    reset(p);
    if (exprP e = star_expressions()) {
        return e;
    }
    reset(p);
    return nullptr;
}

// single_target:
//     | single_subscript_attribute_target
//     | NAME 
//     | '(' single_target ')'
exprP Parser::single_target() {
    return nullptr;
}

// single_subscript_attribute_target:
//     | t_primary '.' NAME !t_lookahead 
//     | t_primary '[' slices ']' !t_lookahead 
exprP Parser::single_subscript_attribute_target() {
    return nullptr;
}

// yield_expr:
//     | 'yield' 'from' expression 
//     | 'yield' [star_expressions] 
exprP Parser::yield_expr() {
    int p = mark();
    if (expect("yield") && expect("from")) {
        if (exprP e = expression()) {
            return make_unique<YieldFrom>(move(e));
        }
    }
    reset(p);
    if (expect("yield")) {
        exprP e = star_expressions();
        return make_unique<Yield>(move(e));
    }
    reset(p);
    return nullptr;
}

// augassign:
//     | '+=' 
//     | '-=' 
//     | '*=' 
//     | '@=' 
//     | '/=' 
//     | '%=' 
//     | '&=' 
//     | '|=' 
//     | '^=' 
//     | '<<=' 
//     | '>>=' 
//     | '**=' 
//     | '//=' 
exprP Parser::augassign() {
    return nullptr;
}

// star_expressions:
//     | star_expression (',' star_expression )+ [','] 
//     | star_expression ',' 
//     | star_expression
exprP Parser::star_expressions() {
    int p = mark();
    exprPs elts;
    if (exprP e = star_expression()) {
        elts.push_back(move(e));
        int p1 = mark();
        while ((p1 = mark()) && expect(Token::Type::COMMA) && (e = star_expression())) {
            elts.push_back(move(e));
        }
        reset(p1);
        expect(Token::Type::COMMA);
        if (elts.size() > 1) {
            return make_unique<Tuple>(move(elts), expr_context::Load);
        }
        return move(elts[0]);
    }
    reset(p);
    return nullptr;
}

// star_expression:
//     | '*' bitwise_or 
//     | expression
exprP Parser::star_expression() {
    int p = mark();
    if (expect(Token::Type::STAR)) {
        const Token& t = Token(Token::Type::VBAR, "|");
        if (exprP e = pratt_parser()) {
            return make_unique<Starred>(move(e), expr_context::Load);
        }
    }
    reset(p);
    if (exprP e = expression()) {
        return e;
    }
    reset(p);
    return nullptr;
}

exprP Parser::expression() {
    return pratt_parser();
}

stmtP Parser::return_stmt() { return nullptr; }

stmtP Parser::import_stmt() { return nullptr; }

stmtP Parser::raise_stmt() { return nullptr; }

stmtP Parser::pass_stmt() { return nullptr; }

stmtP Parser::del_stmt() { return nullptr; }

stmtP Parser::yield_stmt() {
    printf("yield stmt\n");
    int p = mark();
    if (exprP e = yield_expr()) {
        return make_unique<Expr>(move(e));
    }
    reset(p);
    return nullptr;
}

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