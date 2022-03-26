#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include <string>
#include <memory>


#include "AST.h"
#include "Tokenizer.h"
#include "Token.h"

using std::vector;
using std::string;
using std::unique_ptr;
using std::make_unique;

class Token;

class Parser {
public:
    Parser() {}
    unique_ptr<ast> parse(const string& input) {
        tokenizer.tokenize(input);
        return nullptr;
    }

private:

    bool expect(TokenType type) {
        Token t = peek();
        if (t.type == type) {
            next();
            return true;
        } else {
            return false;
        }
    }

    bool expect(const string& tok) {
        Token t = peek();
        if (t.type == TokenType::NAME && t.raw == tok) {
            next();
            return true;
        } else {
            return false;
        }
    }

private:
    int mark() { return tokenizer.mark(); }
    Token peek() { return tokenizer.peek(); }
    Token next() { return tokenizer.next(); }
    Tokenizer tokenizer;
};


#endif /* PARSER_H */
