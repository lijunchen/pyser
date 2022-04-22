#include "Parser.h"
#include <memory>

using std::make_unique;

template <class Fn> struct defer {
    Fn fn;
    ~defer() { fn(); }
};
template <class Fn> defer(Fn) -> defer<Fn>;

unordered_set<string> Parser::keywords = {
    "and", "or", "not", "is", "in", "yield", "from",
};

unique_ptr<Module> Parser::file() {
    int p = mark();
    optional<stmtPs> stmts = statements();
    if (stmts) {
        while (expect(Token::Type::NEWLINE))
            ;
        if (!expect(Token::Type::ENDMARKER)) {
            printf("expect ENDMARKER, but got %s\n", peek().toString().c_str());
            goto parse_error;
        }
        return std::make_unique<Module>(move(*stmts));
    }
parse_error:
    throw std::runtime_error("SyntaxError: invalid syntax");
    reset(p);
    return nullptr;
}

// statements: statement+
optional<stmtPs> Parser::statements() {
    printf("statements\n");
    int p = mark();

    stmtPs stmts;
    if (optional<stmtPs> xs = statement()) {
        printf("!!!1match one statm\n");
        for (size_t i = 0; i < xs->size(); i++) {
            stmts.push_back(move(xs->operator[](i)));
        }

        printf("try nexst token: %s\n", peek().toString().c_str());
        while (optional<stmtPs> xs = statement()) {
            for (size_t i = 0; i < xs->size(); i++) {
                stmts.push_back(move(xs->operator[](i)));
            }
        }
        return stmts;
    }

    reset(p);
    return nullopt;
}

// statement: compound_stmt  | simple_stmts
optional<stmtPs> Parser::statement() {
    printf("statement\n");
    int p = mark();

    if (stmtP s = compound_stmt()) {
        stmtPs ret;
        ret.push_back(move(s));
        return ret;
    }

    reset(p);
    if (optional<stmtPs> stmts = simple_stmts()) {
        return stmts;
    }

    reset(p);
    return nullopt;
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
                return make_unique<While>(move(test), move(*body),
                                          move(orelse));
            }
            printf("parse while fail\n");
        }
    }
    reset(p);
    return nullptr;
}

stmtP Parser::match_stmt() { return nullptr; }

// simple_stmts:
//     | simple_stmt !';' NEWLINE  # Not needed, there for speedup
//     | ';'.simple_stmt+ [';'] NEWLINE
optional<stmtPs> Parser::simple_stmts() {
    printf("simple stmts\n");
    int p = mark();

    stmtPs stmts;
    stmtP stmt;
    if ((stmt = simple_stmt())) {
        stmts.push_back(move(stmt));
        int p1 = mark();
        while ((p1 = mark()) && expect(Token::Type::SEMI) &&
               (stmt = simple_stmt())) {
            stmts.push_back(move(stmt));
        }
        reset(p1);
        expect(Token::Type::SEMI);
        if (!expect(Token::Type::NEWLINE)) {
            reset(p);
            return nullopt;
        }
        printf("simple stmts succ, next: %s\n", peek().toString().c_str());
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
//          | single_subscript_attribute_target) ':' expression ['='
//          annotated_rhs ]
//     | (star_targets '=' )+ (yield_expr | star_expressions) !'='
//     [TYPE_COMMENT] | single_target augassign ~ (yield_expr |
//     star_expressions)
stmtP Parser::assignment() {
    printf("assignment\n");
    int p = mark();
    exprP lhs;
    // case 1:
    //     NAME ':' expression ['=' annotated_rhs ]
    printf("assignment case 1\n");
    if ((lhs = expression()) && expect(Token::Type::COLON)) {
        if (exprP anno = expression()) {
            int p2 = mark();
            exprP value;
            if (expect(Token::Type::EQUAL) && (value = annotated_rhs())) {
                return make_unique<AnnAssign>(move(lhs), move(anno),
                                              move(value), 0);
            }
            reset(p2);
            return make_unique<AnnAssign>(move(lhs), nullptr, move(value), 0);
        }
    }
    reset(p);

    // case 2:
    //     ('(' single_target ')' | single_subscript_attribute_target) ':'
    //     expression ['=' annotated_rhs ]
    //     =>
    //     '(' single_target ')' ':' expression ['=' annotated_rhs ]
    //     single_subscript_attribute_target ':' expression ['=' annotated_rhs ]
    printf("assignment case 2\n");
    if (expect(Token::Type::LPAR) && (lhs = single_target()) &&
        expect(Token::Type::RPAR)) {
        if (expect(Token::Type::COLON)) {
            if (exprP anno = expression()) {
                int p2 = mark();
                exprP rhs;
                if (expect(Token::Type::EQUAL) && (rhs = annotated_rhs())) {
                    return make_unique<AnnAssign>(move(lhs), move(anno),
                                                  move(rhs), 0);
                }
                reset(p2);
                return make_unique<AnnAssign>(move(lhs), move(anno), nullptr,
                                              0);
            }
        }
    }
    reset(p);
    if ((lhs = single_subscript_attribute_target())) {
        if (expect(Token::Type::COLON)) {
            if (exprP anno = expression()) {
                int p2 = mark();
                exprP rhs;
                if (expect(Token::Type::EQUAL) && (rhs = annotated_rhs())) {
                    return make_unique<AnnAssign>(move(lhs), move(anno),
                                                  move(rhs), 0);
                }
                reset(p2);
                return make_unique<AnnAssign>(move(lhs), move(anno), nullptr,
                                              0);
            }
        }
    }

    // case 3:
    //     (star_targets '=' )+ (yield_expr | star_expressions) !'='
    //     [TYPE_COMMENT]
    printf("assignment case 3\n");
    reset(p);
    exprPs ts;
    exprP t;
    if ((t = star_targets()) && expect(Token::Type::EQUAL)) {
        ts.push_back(move(t));
        int p2 = mark();
        while ((p2 = mark()) && (t = star_targets()) &&
               expect(Token::Type::EQUAL)) {
            ts.push_back(move(t));
        }
        reset(p2);
        int p1 = mark();
        exprP rhs;
        if ((p1 = mark()) && (rhs = yield_expr()) &&
            !lookahead(Token::Type::EQUAL)) {
            return make_unique<Assign>(move(ts), move(rhs));
        }
        reset(p1);
        int p0 = mark();
        if ((p0 = mark()) && (rhs = star_expressions()) &&
            !lookahead(Token::Type::EQUAL)) {
            return make_unique<Assign>(move(ts), move(rhs));
        }
        reset(p0);
    }

    // case 4:
    //     single_target augassign ~ (yield_expr | star_expressions)
    reset(p);
    printf("assignment case 4\n");
    if (exprP t = single_target()) {
        if (optional<operator_> op = augassign()) {
            int p1 = mark();
            if (exprP rhs = yield_expr()) {
                return make_unique<AugAssign>(move(t), *op, move(rhs));
            }
            reset(p1);
            if (exprP rhs = star_expressions()) {
                return make_unique<AugAssign>(move(t), *op, move(rhs));
            }
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
    int p = mark();
    if (exprP e = single_subscript_attribute_target()) {
        return e;
    }
    reset(p);
    if (unique_ptr<Name> name = expectN()) {
        name->ctx = expr_context::Store;
        return name;
    }
    reset(p);
    if (expect(Token::Type::LPAR)) {
        if (exprP e = single_target()) {
            if (expect(Token::Type::RPAR)) {
                return e;
            }
        }
    }
    reset(p);
    return nullptr;
}

// single_subscript_attribute_target:
//     | t_primary '.' NAME !t_lookahead
//     | t_primary '[' slices ']' !t_lookahead
exprP Parser::single_subscript_attribute_target() {
    int p = mark();
    if (exprP t = t_primary()) {
        expr* target = t.get();
        if (Attribute* attr = dynamic_cast<Attribute*>(target)) {
            return t;
        } else if (Subscript* sub = dynamic_cast<Subscript*>(target)) {
            return t;
        }
    }
    reset(p);
    return nullptr;
}

exprP Parser::t_primary() {
    int p = mark();
    static const Token& t = Token(Token::Type::DOT, ".");
    optional<BindingPower> bp = infix_binding_power(t);
    if (exprP e = pratt_parser_bp(*bp->left)) {
        return e;
    }
    reset(p);
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
optional<operator_> Parser::augassign() {
    int p = mark();
    const Token& t = peek();
    optional<operator_> op = nullopt;
    switch (t.type) {
    case Token::Type::PLUSEQUAL:
        op = operator_::Add;
        break;
    case Token::Type::MINEQUAL:
        op = operator_::Sub;
        break;
    case Token::Type::STAREQUAL:
        op = operator_::Mult;
        break;
    case Token::Type::ATEQUAL:
        op = operator_::MatMult;
        break;
    case Token::Type::SLASHEQUAL:
        op = operator_::Div;
        break;
    case Token::Type::PERCENTEQUAL:
        op = operator_::Mod;
        break;
    case Token::Type::AMPEREQUAL:
        op = operator_::BitAnd;
        break;
    case Token::Type::VBAREQUAL:
        op = operator_::BitOr;
        break;
    case Token::Type::CIRCUMFLEXEQUAL:
        op = operator_::BitXor;
        break;
    case Token::Type::LEFTSHIFTEQUAL:
        op = operator_::LShift;
        break;
    case Token::Type::RIGHTSHIFTEQUAL:
        op = operator_::RShift;
        break;
    case Token::Type::DOUBLESTAREQUAL:
        op = operator_::Pow;
        break;
    case Token::Type::DOUBLESLASHEQUAL:
        op = operator_::FloorDiv;
        break;
    default:
        op = nullopt;
    }
    if (op) {
        next();
        return op;
    }
    reset(p);
    return nullopt;
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
        while ((p1 = mark()) && expect(Token::Type::COMMA) &&
               (e = star_expression())) {
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
        exprP e = bitwise_or();
        if (e) {
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
    exprP e = pratt_parser();
    return e;
}

exprP Parser::bitwise_or() {
    int p = mark();
    static const Token& t = Token(Token::Type::VBAR, "|");
    optional<BindingPower> bp = infix_binding_power(t);
    if (exprP e = pratt_parser_bp(*bp->left)) {
        return e;
    }
    reset(p);
    return nullptr;
}

exprPs Parser::star_named_expressions() {
    int p = mark();
    exprPs elts;
    if (exprP e = star_named_expression()) {
        elts.push_back(move(e));
        int p1 = mark();
        while ((p1 = mark()) && expect(Token::Type::COMMA) &&
               (e = star_named_expression())) {
            elts.push_back(move(e));
        }
        reset(p1);
        expect(Token::Type::COMMA);
        return elts;
    }
    reset(p);
    return {};
}

exprP Parser::star_named_expression() {
    int p = mark();
    if (expect(Token::Type::STAR)) {
        exprP e = bitwise_or();
        if (e) {
            return make_unique<Starred>(move(e), expr_context::Load);
        }
    }
    reset(p);
    if (exprP e = named_expression()) {
        return e;
    }
    reset(p);
    return nullptr;
}

exprP Parser::named_expression() {
    int p = mark();
    exprP e;
    if ((e = assignment_expression())) {
        return e;
    }
    reset(p);
    if ((e = expression()) && !lookahead(Token::Type::COLONEQUAL)) {
        return e;
    }
    reset(p);
    return nullptr;
}

exprP Parser::assignment_expression() {
    int p = mark();
    exprP target;
    exprP value;
    if ((target = expectN()) && (value = expression())) {
        return make_unique<NamedExpr>(move(target), move(value));
    }
    reset(p);
    return nullptr;
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

// # NOTE: star_targets may contain *bitwise_or, targets may not.
// star_targets:
//     | star_target !','
//     | star_target (',' star_target )* [',']

exprP Parser::star_targets() {
    printf("star_targets\n");
    int p = mark();
    if (exprP t = star_target()) {
        if (!lookahead(Token::Type::COMMA)) {
            return t;
        }
    }
    reset(p);
    exprPs ts;
    if (exprP t = star_target()) {
        int p1 = mark();
        while ((p1 = mark()) && expect(Token::Type::COMMA) &&
               (t = star_target())) {
            ts.push_back(move(t));
        }
        reset(p1);
        return make_unique<Tuple>(move(ts), expr_context::Store);
    }

    reset(p);
    return nullptr;
}

// star_target:
//     | '*' (!'*' star_target)
//     | target_with_star_atom
exprP Parser::star_target() {
    printf("star_target\n");
    int p = mark();
    if (expect(Token::Type::STAR) && !lookahead(Token::Type::STAR)) {
        exprP e = star_target();
        if (e) {
            return make_unique<Starred>(move(e), expr_context::Store);
        }
    }

    reset(p);
    if (exprP e = target_with_star_atom()) {
        return e;
    }

    reset(p);
    return nullptr;
}

// target_with_star_atom:
//     | t_primary '.' NAME !t_lookahead
//     | t_primary '[' slices ']' !t_lookahead
//     | star_atom
exprP Parser::target_with_star_atom() {
    printf("target_with_star_atom\n");
    int p = mark();
    if (exprP t = t_primary()) {
        expr* target = t.get();
        if (Attribute* attr = dynamic_cast<Attribute*>(target)) {
            return t;
        } else if (Subscript* sub = dynamic_cast<Subscript*>(target)) {
            return t;
        }
    }
    reset(p);
    if (exprP t = star_atom()) {
        return t;
    }
    reset(p);
    return nullptr;
}

// star_targets_list_seq: ','.star_target+ [',']
exprPs Parser::star_targets_list_seq() {
    printf("star_targets_list_seq\n");
    int p = mark();
    exprP t;
    exprPs ts;
    if ((t = star_target())) {
        ts.push_back(move(t));
        int p1 = mark();
        while ((p1 = mark()) && expect(Token::Type::COMMA) &&
               (t = star_target())) {
            ts.push_back(move(t));
        }
        reset(p1);
        expect(Token::Type::COMMA);
        return ts;
    }
    reset(p);
    return {};
}

// star_targets_tuple_seq:
//     | star_target (',' star_target )+ [',']
//     | star_target ','
exprPs Parser::star_targets_tuple_seq() {
    printf("star_targets_tuple_seq\n");
    int p = mark();
    exprPs xs;
    exprP x;
    if ((x = star_target())) {
        xs.push_back(move(x));
        int p2 = mark();
        while ((p2 = mark()) && expect(Token::Type::COMMA) &&
               (x = star_target())) {
            xs.push_back(move(x));
        }
        reset(p2);
        expect(Token::Type::COMMA);
        return xs;
    }

    reset(p);
    xs.clear();
    if ((x = star_target()) && expect(Token::Type::COMMA)) {
        xs.push_back(move(x));
        return xs;
    }

    reset(p);
    return {};
}

// star_atom:
//     | NAME
//     | '(' target_with_star_atom ')'
//     | '(' [star_targets_tuple_seq] ')'
//     | '[' [star_targets_list_seq] ']'
exprP Parser::star_atom() {
    printf("star_atom\n");
    int p = mark();

    if (unique_ptr<Name> name = expectN()) {
        name->ctx = expr_context::Store;
        return name;
    }

    exprP a;
    reset(p);
    if (expect(Token::Type::LPAR) && (a = target_with_star_atom()) &&
        expect(Token::Type::RPAR)) {
        return a;
    }

    reset(p);
    if (expect(Token::Type::LPAR)) {
        exprPs ts = star_targets_tuple_seq();
        if (expect(Token::Type::RPAR)) {
            return make_unique<Tuple>(move(ts), expr_context::Store);
        }
    }

    reset(p);
    if (expect(Token::Type::LSQB)) {
        exprPs ts = star_targets_list_seq();
        if (expect(Token::Type::RSQB)) {
            return make_unique<List>(move(ts), expr_context::Store);
        }
    }

    reset(p);
    return nullptr;
}
