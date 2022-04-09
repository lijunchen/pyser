#ifndef TOKEN_H
#define TOKEN_H

#include <functional>
#include <string>

using std::string;

class Token {
public:
    enum class Type {
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

public:
    Token() : Token(Token::Type::ENDMARKER, "") {}
    Token(Token::Type type) : type(type) {}
    Token(Token::Type type, const string& raw) : type(type), raw(raw) {}

    Token::Type type;
    string raw;

    explicit operator bool() const { return type != Token::Type::ENDMARKER; }

    bool operator==(const Token& other) const {
        if (type != other.type)
            return false;
        if (type == Token::Type::NAME || type == Token::Type::NUMBER || type == Token::Type::STRING) {
            return raw == other.raw;
        }
        return true;
    }

    bool is_operator() const {
        return type == Token::Type::PLUS ||
               type == Token::Type::MINUS ||
               type == Token::Type::STAR ||
               type == Token::Type::AT ||
               type == Token::Type::SLASH ||
               type == Token::Type::PERCENT ||
               type == Token::Type::DOUBLESTAR ||
               type == Token::Type::LEFTSHIFT ||
               type == Token::Type::RIGHTSHIFT ||
               type == Token::Type::VBAR ||
               type == Token::Type::CIRCUMFLEX ||
               type == Token::Type::AMPER ||
               type == Token::Type::DOUBLESLASH ||
               type == Token::Type::DOT ||
               type == Token::Type::LPAR ||
               type == Token::Type::LSQB;
    }

    bool is_boolop() const {
        return type == Token::Type::NAME && (raw == "and" || raw == "or");
    }

    bool is_unaryop() const {
        return type == Token::Type::PLUS ||
               type == Token::Type::MINUS ||
               type == Token::Type::TILDE ||
               type == Token::Type::CIRCUMFLEX;
    }

    bool is_cmpop() const {
        return type == Token::Type::EQEQUAL ||
               type == Token::Type::NOTEQUAL ||
               type == Token::Type::LESS ||
               type == Token::Type::LESSEQUAL ||
               type == Token::Type::GREATER ||
               type == Token::Type::GREATEREQUAL ||
               type == Token::Type::NAME && (raw == "not" || raw == "is" || raw == "in");
    }

    static string typeToString(Token::Type tt);

    string toString() const {
        string s = "Token(" + typeToString(type) + ", '" + raw + "')";
        return s;
    }
};

namespace std {

template <> struct hash<Token> {
    std::size_t operator()(const Token& t) const {
        if (t.type == Token::Type::NAME || t.type == Token::Type::NUMBER || t.type == Token::Type::STRING) {
            return hash<string>()(t.raw) ^ hash<Token::Type>()(t.type);
        }
        return hash<Token::Type>()(t.type);
    }
};

} // namespace std

#endif /* TOKEN_H */
