#ifndef TOKEN_H
#define TOKEN_H

#include <string>

using std::string;

enum class TokenType
{
    ENDMARKER,
    NAME,
    NUMBER,
    STRING,
    NEWLINE,
    INDENT,
    DEDENT,

    LPAR,
    RPAR,
    LSQB,
    RSQB,
    COLON,
    COMMA,
    SEMI,
    PLUS,
    MINUS,
    STAR,
    SLASH,
    VBAR,
    AMPER,
    LESS,
    GREATER,
    EQUAL,
    DOT,
    PERCENT,
    LBRACE,
    RBRACE,
    EQEQUAL,
    NOTEQUAL,
    LESSEQUAL,
    GREATEREQUAL,
    TILDE,
    CIRCUMFLEX,
    LEFTSHIFT,
    RIGHTSHIFT,
    DOUBLESTAR,
    PLUSEQUAL,
    MINEQUAL,
    STAREQUAL,
    SLASHEQUAL,
    PERCENTEQUAL,
    AMPEREQUAL,
    VBAREQUAL,
    CIRCUMFLEXEQUAL,
    LEFTSHIFTEQUAL,
    RIGHTSHIFTEQUAL,
    DOUBLESTAREQUAL,
    DOUBLESLASH,
    DOUBLESLASHEQUAL,
    AT,
    ATEQUAL,
    RARROW,
    ELLIPSIS,
    COLONEQUAL,

    OP,
    AWAIT,
    ASYNC,
    TYPE_IGNORE,
    TYPE_COMMENT,
    SOFT_KEYWORD,
    ERRORTOKEN,

    COMMENT,
    NL,
    ENCODING,
};

string tokenTypeToString(TokenType tt);

class Token {
public:
    Token(): Token(TokenType::ENDMARKER, "") {}
    Token(TokenType type) : type(type) {}
    Token(TokenType type, const string& raw) : type(type), raw(raw) {}

    TokenType type;
    string raw;

    explicit operator bool() const { return type != TokenType::ENDMARKER; }

    string toString() const {
        string s = tokenTypeToString(type) + ", " + raw;
        return s;
    }
};


#endif /* TOKEN_H */
