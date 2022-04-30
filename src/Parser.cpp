#include "Parser.h"
#include "Logger.h"
#include <memory>
#include <iostream>

using std::make_unique;

template <class Fn> struct defer {
    Fn fn;
    ~defer() { fn(); }
};
template <class Fn> defer(Fn) -> defer<Fn>;

unordered_set<string> Parser::keywords = {
    "and", "or", "not", "is", "in", "yield", "assert", "import", "as", "from", "pass"};

unique_ptr<Module> Parser::file() {
    int p = mark();
    optional<stmtPs> stmts = statements();
    if (stmts) {
        while (expect(Token::Type::NEWLINE))
            ;
        if (!expect(Token::Type::ENDMARKER)) {
            Logger::debug("expect ENDMARKER, but got %s\n", peek().toString().c_str());
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
    Logger::debug("statements\n");
    int p = mark();

    stmtPs stmts;
    if (optional<stmtPs> xs = statement()) {
        for (size_t i = 0; i < xs->size(); i++) {
            stmts.push_back(move(xs->operator[](i)));
        }

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
    Logger::debug("statement\n");
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
    Logger::debug("block\n");
    int p = mark();
    optional<stmtPs> stmts;

    if (expect(Token::Type::NEWLINE)) {
        if (!expect(Token::Type::INDENT)) {
            Logger::debug("expect indent after newline in block\n");
            reset(p);
            return nullopt;
        }
        stmts = statements();
        if (!stmts) {
            reset(p);
            return nullopt;
        }
        if (!expect(Token::Type::DEDENT)) {
            reset(p);
            return nullopt;
        }
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
    Logger::debug("compound stmt\n");
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
    Logger::debug("if stmt\n");
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
    Logger::debug("while stmt\n");
    int p = mark();
    if (expect("while")) {
        exprP test = named_expression();
        if (expect(Token::Type::COLON)) {
            optional<stmtPs> body = block();
            stmtPs orelse;
            if (body) {
                Logger::debug("parse while succ\n");
                return make_unique<While>(move(test), move(*body),
                                          move(orelse));
            }
            Logger::debug("parse while fail\n");
        }
    }
    reset(p);
    return nullptr;
}

stmtP Parser::match_stmt() { return nullptr; }

// simple_stmts:
//	   | simple_stmt !';' NEWLINE  # Not needed, there for speedup
//	   | ';'.simple_stmt+ [';'] NEWLINE
optional<stmtPs> Parser::simple_stmts() {
    Logger::debug("simple stmts\n");
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
        while (expect(Token::Type::NEWLINE));
        Logger::debug("simple stmts succ, next: %s\n", peek().toString().c_str());
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
//	   | NAME ':' expression ['=' annotated_rhs ]
//	   | ('(' single_target ')'
//			| single_subscript_attribute_target) ':' expression ['='
//			annotated_rhs ]
//	   | (star_targets '=' )+ (yield_expr | star_expressions) !'='
//	   [TYPE_COMMENT] | single_target augassign ~ (yield_expr |
//	   star_expressions)
stmtP Parser::assignment() {
    Logger::debug("assignment\n");
    int p = mark();
    exprP lhs;
    // case 1:
    //     NAME ':' expression ['=' annotated_rhs ]
    Logger::debug("assignment case 1\n");
    unique_ptr<Name> name;
    if ((name = expectN()) && expect(Token::Type::COLON)) {
        name->set_expr_context(expr_context::Store);
        if (exprP anno = expression()) {
            int p2 = mark();
            exprP value;
            if (expect(Token::Type::EQUAL) && (value = annotated_rhs())) {
                return make_unique<AnnAssign>(move(name), move(anno),
                                              move(value), 1);
            }
            reset(p2);
            return make_unique<AnnAssign>(move(name), nullptr, move(value), 1);
        }
    }
    reset(p);

    // case 2:
    //	   ('(' single_target ')' | single_subscript_attribute_target) ':'
    //	   expression ['=' annotated_rhs ]
    //	   =>
    //	   '(' single_target ')' ':' expression ['=' annotated_rhs ]
    //	   single_subscript_attribute_target ':' expression ['=' annotated_rhs ]
    Logger::debug("assignment case 2\n");
    if (expect(Token::Type::LPAR) && (lhs = single_target()) &&
        expect(Token::Type::RPAR)) {
        lhs->set_expr_context(expr_context::Store);
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
        lhs->set_expr_context(expr_context::Store);
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
    Logger::debug("assignment case 3\n");
    reset(p);
    exprPs ts;
    exprP t;
    if ((t = star_targets()) && expect(Token::Type::EQUAL)) {
        t->set_expr_context(expr_context::Store);
        ts.push_back(move(t));
        int p2 = mark();
        while ((p2 = mark()) && (t = star_targets()) &&
               expect(Token::Type::EQUAL)) {
            t->set_expr_context(expr_context::Store);
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
    //	   single_target augassign ~ (yield_expr | star_expressions)
    reset(p);
    Logger::debug("assignment case 4\n");
    if (exprP t = single_target()) {
        t->set_expr_context(expr_context::Store);
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
//	   | single_subscript_attribute_target
//	   | NAME
//	   | '(' single_target ')'
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
//	   | t_primary '.' NAME !t_lookahead
//	   | t_primary '[' slices ']' !t_lookahead
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
//	   | 'yield' 'from' expression
//	   | 'yield' [star_expressions]
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
//	   | '+='
//	   | '-='
//	   | '*='
//	   | '@='
//	   | '/='
//	   | '%='
//	   | '&='
//	   | '|='
//	   | '^='
//	   | '<<='
//	   | '>>='
//	   | '**='
//	   | '//='
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
//	   | star_expression (',' star_expression )+ [',']
//	   | star_expression ','
//	   | star_expression
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
//	   | '*' bitwise_or
//	   | expression
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

stmtP Parser::import_stmt() {
    auto p = mark();
    if (auto alias = import_name()) {
        return make_unique<Import>(move(*alias));
    }
    if (auto import_from_stmt = import_from()) {
        return import_from_stmt;
    }
    reset(p);
    return nullptr;
}

optional<std::vector<alias>> Parser::import_name() {
    auto p = mark();
    if (expect("import")) {
        if (auto alias = dotted_as_names()) {
            return alias;
        }
    }
    reset(p);
    return std::nullopt;
}

optional<string> Parser::dotted_name() {
    // dotted_name := dotted_name . NAME | Name
    // which is equavalent with
    // dotted_name := NAME dotted_name_1
    // dotted_name_1 := . NAME dotted_name_1 | ENDMARKER
    auto p = mark();
    string dot_name;
    do {
        if (auto name = expectN()) {
            if (name->id == "import") {
                // FIXME
                reset(p);
                return nullopt;
            }
            dot_name += name->id;
            dot_name.push_back('.');
        } else {
            reset(p);
            return nullopt;
        }
    } while (expect("."));
    dot_name.pop_back();
    return dot_name;
}

optional<vector<alias>> Parser::dotted_as_names() {
    auto p = mark();
    vector<alias> aliases;
    auto dotted_as_name = [this]() -> optional<alias> {
        // dotted_as_name := dotted_name ['as' NAME]
        auto p = mark();
        if (auto dn = dotted_name()) {
            if (expect("as")) {
                if (auto name = expectN()) {
                    return alias(*dn, name->id);
                }
                reset(p);
                return nullopt;
            }
            return alias(*dn, nullopt);
        }
        reset(p);
        return nullopt;
    };
    do {
        if (auto alias = dotted_as_name()) {
            aliases.push_back(*alias);
        } else {
            reset(p);
            return nullopt;
        }
    } while (expect(Token::Type::COMMA));
    return aliases;
}

stmtP Parser::import_from() {
    // import_from:
    //     | 'from' ('.' | '...')* dotted_name 'import' import_from_targets
    //     | 'from' ('.' | '...')+ 'import' import_from_targets
    auto p = mark();
    auto count_level = [this]() -> size_t {
        auto dot_count = 0ull;
        auto ellipsis_count = 0ull;
        do {
            if (expect(Token::Type::DOT)) {
                dot_count++;
            } else if (expect(Token::Type::ELLIPSIS)) {
                ellipsis_count++;
            } else {
                break;
            }
        } while (true);
        return dot_count + ellipsis_count * 3;
    };
    if (expect("from")) {
        auto level = count_level();
        if (level == 0) {
            // ('.' | '...')*
            if (auto module = dotted_name()) {
                if (expect("import")) {
                    if (auto alias = import_from_targets()) {
                        return make_unique<ImportFrom>(module, move(*alias),
                                                       level);
                    }
                    reset(p);
                    return nullptr;
                }
                reset(p);
                return nullptr;
            }
            reset(p);
            return nullptr;
        } else {
            // ('.' | '...')+
            if (auto module = dotted_name()) {
                if (expect("import")) {
                    if (auto alias = import_from_targets()) {
                        return make_unique<ImportFrom>(module, move(*alias),
                                                       level);
                    }
                    reset(p);
                    return nullptr;
                }
                reset(p);
                return nullptr;
            }
            if (expect("import")) {
                if (auto alias = import_from_targets()) {
                    return make_unique<ImportFrom>(nullopt, move(*alias),
                                                   level);
                }
                reset(p);
                return nullptr;
            }
            reset(p);
            return nullptr;
        }
    }
    reset(p);
    return nullptr;
}

optional<vector<alias>> Parser::import_from_targets() {
    // import_from_targets:
    //     | '(' import_from_as_names [','] ')'
    //     | import_from_as_names !','
    //     | '*'
    auto p = mark();
    if (expect(Token::Type::LPAR)) {
        if (auto from_as_names = import_from_as_names()) {
            if (expect(Token::Type::COMMA)) {
                if (expect(Token::Type::RPAR)) {
                    return from_as_names;
                }
                reset(p);
                return nullopt;
            }
            if (expect(Token::Type::RPAR)) {
                return from_as_names;
            }
            reset(p);
            return nullopt;
        }
        reset(p);
        return nullopt;
    }
    if (auto from_as_names = import_from_as_names()) {
        if (peek() == Token::Type::COMMA) {
            reset(p);
            return nullopt;
        }
        return from_as_names;
    }
    if (expect("*")) {
        return vector<alias>{alias("*", nullopt)};
    }
    reset(p);
    return nullopt;
}

optional<vector<alias>> Parser::import_from_as_names() {
    auto import_from_as_name = [this]() -> optional<alias> {
        // import_from_as_name:= NAME ['as' NAME]
        auto p = mark();
        if (auto n1 = expectN()) {
            if (expect("as")) {
                if (auto n2 = expectN()) {
                    return alias(n1->id, n2->id);
                }
                reset(p);
                return nullopt;
            }
            return alias(n1->id, nullopt);
        }
        reset(p);
        return nullopt;
    };
    auto p = mark();
    vector<alias> aliases;
    do {
        if (auto as_name = import_from_as_name()) {
            aliases.push_back(*as_name);
        } else {
            reset(p);
            return nullopt;
        }
    } while (expect(Token::Type::COMMA));
    return aliases;
}

stmtP Parser::raise_stmt() { return nullptr; }

stmtP Parser::pass_stmt() {
    int p = mark();
    if (expect("pass")) {
        return make_unique<Pass>();
    }
    return nullptr;
}

stmtP Parser::del_stmt() { return nullptr; }

stmtP Parser::yield_stmt() {
    Logger::debug("yield stmt\n");
    int p = mark();
    if (exprP e = yield_expr()) {
        return make_unique<Expr>(move(e));
    }
    reset(p);
    return nullptr;
}

stmtP Parser::assert_stmt() {
    // assert_stmt: 'assert' expression [, expreesion]
    auto p = mark();
    if (expect("assert")) {
        if (auto test = expression()) {
            if (expectT(Token::Type::COMMA)) {
                if (auto msg = expression()) {
                    return make_unique<Assert>(move(test), move(msg));
                }
                reset(p);
                return nullptr;
            }
            return make_unique<Assert>(move(test), nullptr);
        }
    }
    reset(p);
    return nullptr;
}

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
    Logger::debug("atom rule\n");
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

        Logger::debug("atom name: %s\n", t.raw.c_str());
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
//	   | star_target !','
//	   | star_target (',' star_target )* [',']

exprP Parser::star_targets() {
    Logger::debug("star_targets\n");
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
//	   | '*' (!'*' star_target)
//	   | target_with_star_atom
exprP Parser::star_target() {
    Logger::debug("star_target\n");
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
//	   | t_primary '.' NAME !t_lookahead
//	   | t_primary '[' slices ']' !t_lookahead
//	   | star_atom
exprP Parser::target_with_star_atom() {
    Logger::debug("target_with_star_atom\n");
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
    Logger::debug("star_targets_list_seq\n");
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
//	   | star_target (',' star_target )+ [',']
//	   | star_target ','
exprPs Parser::star_targets_tuple_seq() {
    Logger::debug("star_targets_tuple_seq\n");
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
//	   | NAME
//	   | '(' target_with_star_atom ')'
//	   | '(' [star_targets_tuple_seq] ')'
//	   | '[' [star_targets_list_seq] ']'
exprP Parser::star_atom() {
    Logger::debug("star_atom\n");
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
