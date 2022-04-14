#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include <string>
#include <memory>
#include <optional>
#include <unordered_set>


#include "AST.h"
#include "Tokenizer.h"
#include "Token.h"

using std::vector;
using std::string;
using std::unique_ptr;
using std::make_unique;
using std::nullopt;
using std::optional;
using std::unordered_set;

class Token;

void initBindingPowerTables();

class Parser {
public:
    Parser() {
        initBindingPowerTables();
    }
    
    unique_ptr<Module> parse(const string& input) {
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
        printf("expect %s, actually: %s\n", Token::typeToString(type).c_str(), t.toString().c_str());
        if (t.type == type) {
            next();
            return true;
        } else {
            return false;
        }
    }

    Token expectT(Token::Type type) {
        const Token& t = peek();
        printf("expect %s, actually: %s\n", Token::typeToString(type).c_str(), t.toString().c_str());
        if (t.type == type) {
            next();
            return t;
        } else {
            return Token();
        }
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
    stmtP statement();

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
    stmtP assignment()  ;
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
    exprP atom();

    exprP annotated_rhs();
    exprP single_target();
    exprP single_subscript_attribute_target();
    exprP yield_expr();
    exprP augassign();


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
};

#endif /* PARSER_H */
