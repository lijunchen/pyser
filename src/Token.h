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

string tokenTypeToString(TokenType tt) {
    switch (tt) {
        case TokenType::ENDMARKER: return "ENDMARKER";
        case TokenType::NAME: return "NAME";
        case TokenType::NUMBER: return "NUMBER";
        case TokenType::STRING: return "STRING";
        case TokenType::NEWLINE: return "NEWLINE";
        case TokenType::INDENT: return "INDENT";
        case TokenType::DEDENT: return "DEDENT";
        case TokenType::LPAR: return "LPAR";
        case TokenType::RPAR: return "RPAR";
        case TokenType::LSQB: return "LSQB";
        case TokenType::RSQB: return "RSQB";
        case TokenType::COLON: return "COLON";
        case TokenType::COMMA: return "COMMA";
        case TokenType::SEMI: return "SEMI";
        case TokenType::PLUS: return "PLUS";
        case TokenType::MINUS: return "MINUS";
        case TokenType::STAR: return "STAR";
        case TokenType::SLASH: return "SLASH";
        case TokenType::VBAR: return "VBAR";
        case TokenType::AMPER: return "AMPER";
        case TokenType::LESS: return "LESS";
        case TokenType::GREATER: return "GREATER";
        case TokenType::EQUAL: return "EQUAL";
        case TokenType::DOT: return "DOT";
        case TokenType::PERCENT: return "PERCENT";
        case TokenType::LBRACE: return "LBRACE";
        case TokenType::RBRACE: return "RBRACE";
        case TokenType::EQEQUAL: return "EQEQUAL";
        case TokenType::NOTEQUAL: return "NOTEQUAL";
        case TokenType::LESSEQUAL: return "LESSEQUAL";
        case TokenType::GREATEREQUAL: return "GREATEREQUAL";
        case TokenType::TILDE: return "TILDE";
        case TokenType::CIRCUMFLEX: return "CIRCUMFLEX";
        case TokenType::LEFTSHIFT: return "LEFTSHIFT";
        case TokenType::RIGHTSHIFT: return "RIGHTSHIFT";
        case TokenType::DOUBLESTAR: return "DOUBLESTAR";
        case TokenType::PLUSEQUAL: return "PLUSEQUAL";
        case TokenType::MINEQUAL: return "MINEQUAL";
        case TokenType::STAREQUAL: return "STAREQUAL";
        case TokenType::SLASHEQUAL: return "SLASHEQUAL";
        case TokenType::PERCENTEQUAL: return "PERCENTEQUAL";
        case TokenType::AMPEREQUAL: return "AMPEREQUAL";
        case TokenType::VBAREQUAL: return "VBAREQUAL";
        case TokenType::CIRCUMFLEXEQUAL: return "CIRCUMFLEXEQUAL";
        case TokenType::LEFTSHIFTEQUAL: return "LEFTSHIFTEQUAL";
        case TokenType::RIGHTSHIFTEQUAL: return "RIGHTSHIFTEQUAL";
        case TokenType::DOUBLESTAREQUAL: return "DOUBLESTAREQUAL";
        case TokenType::DOUBLESLASH: return "DOUBLESLASH";
        case TokenType::DOUBLESLASHEQUAL: return "DOUBLESLASHEQUAL";
        case TokenType::AT: return "AT";
        case TokenType::ATEQUAL: return "ATEQUAL";
        case TokenType::RARROW: return "RARROW";
        case TokenType::ELLIPSIS: return "ELLIPSIS";
        case TokenType::COLONEQUAL: return "COLONEQUAL";
        case TokenType::OP: return "OP";
        case TokenType::AWAIT: return "AWAIT";
        case TokenType::ASYNC: return "ASYNC";
        case TokenType::TYPE_IGNORE: return "TYPE_IGNORE";
        case TokenType::TYPE_COMMENT: return "TYPE_COMMENT";
        case TokenType::SOFT_KEYWORD: return "SOFT_KEYWORD";
        case TokenType::ERRORTOKEN: return "ERRORTOKEN";
        case TokenType::COMMENT: return "COMMENT";
        case TokenType::NL: return "NL";
        case TokenType::ENCODING: return "ENCODING";
    }
}

class Token {
public:
    Token(): Token(TokenType::ENDMARKER, "") {}
    Token(TokenType type) : type(type) {}
    Token(TokenType type, const string& raw) : type(type), raw(raw) {}

    TokenType type;
    string raw;

    explicit operator bool() const { return type != TokenType::ENDMARKER; }

    string toString() {
        string s = tokenTypeToString(type) + ", " + raw;
        return s;
    }
};
