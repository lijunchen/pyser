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
    
    unique_ptr<ast> parse(const string& input) {
        tokenizer.tokens = tokenizer.tokenize(input);
        return while_stmt();
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
        Token t = peek();
        if (t.type == type) {
            next();
            return true;
        } else {
            return false;
        }
    }

    Token expectT(Token::Type type) {
        Token t = peek();
        if (t.type == type) {
            next();
            return t;
        } else {
            return Token();
        }
    }

    bool expect(const string& tok) {
        Token t = peek();
        if (t.type == Token::Type::NAME && t.raw == tok) {
            next();
            return true;
        } else {
            return false;
        }
    }
    unique_ptr<Module> file();
    optional<stmtPs> statements();
    stmtP statement();

    stmtP compound_stmt();
    optional<stmtPs> block();
    stmtP if_stmt();
    stmtP while_stmt();

    optional<stmtPs> simple_stmts();
    stmtP simple_stmt();
    exprPs star_expressions();
    exprP star_expression();
    exprP expression();
    exprP atom();

    exprP pratt_parser();
    exprP pratt_parser_bp(int minBP);

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
