#ifndef PARSER_H
#define PARSER_H

#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include "AST.h"
#include "Token.h"
#include "Tokenizer.h"

using std::make_unique;
using std::nullopt;
using std::optional;
using std::string;
using std::tuple;
using std::unique_ptr;
using std::unordered_map;
using std::unordered_set;
using std::vector;

class Token;

void initBindingPowerTables();

class BindingPower {
public:
    BindingPower(optional<int> left, optional<int> right)
        : left(left), right(right) {}

public:
    optional<int> left;
    optional<int> right;
};

enum class Fix { Pre, Post, In };
enum class Assoc { Non, Left, Right };

string fixToString();
string assocToString();

void initBindingPowerTables();

class Parser {
public:
    Parser() { initBindingPowerTables(); }

    unique_ptr<ast> parse(const string& input) {
        tokenizer.tokens = tokenizer.tokenize(input);
        return file();
    }

    stmtP parseWhile(const string& input) {
        tokenizer.tokens = tokenizer.tokenize(input);
        reset(0);
        printf("parseWhile, tokens size: %lu\n", tokenizer.tokens.size());
        for (auto& t : tokenizer.tokens) {
            printf("%s\n", t.toString().c_str());
        }
        return while_stmt();
    }

private:
    bool expect(Token::Type type) {
        const Token& t = peek();
        // printf("expect %s, actually: %s\n",
        // Token::typeToString(type).c_str(), t.toString().c_str());
        if (t.type == type) {
            next();
            return true;
        } else {
            return false;
        }
    }

    Token expectT(Token::Type type) {
        const Token& t = peek();
        // printf("expect %s, actually: %s\n",
        // Token::typeToString(type).c_str(), t.toString().c_str());
        if (t.type == type) {
            next();
            return t;
        } else {
            return Token();
        }
    }

    unique_ptr<Name> expectN() {
        const Token& t = peek();
        if (t.type == Token::Type::NAME) {
            next();
            return make_unique<Name>(t.raw, expr_context::Load);
        }
        return nullptr;
    }

    bool expect(const string& tok) {
        const Token& t = peek();
        if (t.raw == tok) {
            next();
            return true;
        } else {
            return false;
        }
    }

    bool lookahead(Token::Type type) {
        const Token& t = peek();
        if (t.type == type) {
            return true;
        } else {
            return false;
        }
    }

    unique_ptr<Module> file();
    optional<stmtPs> statements();
    optional<stmtPs> statement();

    stmtP compound_stmt();
    stmtP function_def();
    stmtP if_stmt();
    stmtP class_def();
    stmtP with_stmt();
    stmtP for_stmt();
    stmtP try_stmt();
    stmtP while_stmt();
    stmtP match_stmt();

    optional<stmtPs> block();

    optional<stmtPs> simple_stmts();
    stmtP simple_stmt();
    stmtP assignment();
    stmtP return_stmt();
    stmtP import_stmt();
    stmtP raise_stmt();
    stmtP pass_stmt();
    stmtP del_stmt();
    stmtP yield_stmt();
    stmtP assert_stmt();
    stmtP break_stmt();
    stmtP continue_stmt();
    stmtP global_stmt();
    stmtP nonlocal_stmt();

    exprP star_expressions();
    exprP star_expression();
    exprP expression();
    exprP bitwise_or();

    exprPs star_named_expressions();
    exprP star_named_expression();
    exprP named_expression();
    exprP assignment_expression();

    exprP atom();

    exprP t_primary();

    exprP annotated_rhs();
    exprP single_target();
    exprP single_subscript_attribute_target();
    exprP yield_expr();
    optional<operator_> augassign();

    exprP star_targets();
    exprP star_target();
    exprP target_with_star_atom();
    exprPs star_targets_list_seq();
    exprPs star_targets_tuple_seq();
    exprP star_atom();

    exprP pratt_parser();
    exprP pratt_parser_bp(int minBP);

    exprP slices();
    exprP slice();

private:
    int mark() { return tokenizer.mark(); }
    void reset(int p) { tokenizer.reset(p); }
    Token peek() { return tokenizer.peek(); }
    Token next() { return tokenizer.next(); }
    Tokenizer tokenizer;

private:
    static unordered_set<string> keywords;

public:
    // pratt related
    static unordered_map<Token, optional<BindingPower>> infixTable;
    static unordered_map<Token, optional<BindingPower>> prefixTable;
    static unordered_map<Token, optional<BindingPower>> postfixTable;
    static vector<tuple<Fix, Assoc, vector<Token>>> table;
    static optional<BindingPower> prefix_binding_power(const Token& t);
    static optional<BindingPower> post_binding_power(const Token& t);
    static optional<BindingPower> infix_binding_power(const Token& t);

    void initBindingPowerTables();
};

#endif /* PARSER_H */
