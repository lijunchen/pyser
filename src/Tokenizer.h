#ifndef TOKENIZER_H
#define TOKENIZER_H

#include "Token.h"
#include <vector>

using std::vector;

class Tokenizer{
public:
    vector<Token> tokenize(const string& input);
    int mark() { return p; }
    void reset(int p) { this->p = p; }
    Token peek() {
        if (p < tokens.size()) {
            return tokens[p];
        } else {
            return Token(TokenType::ENDMARKER);
        }
    }
    Token next() {
        p++;
        return peek();
    }
private:
    vector<Token> tokens;
    int p;
};



#endif /* TOKENIZER_H */
