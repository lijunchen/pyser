#include "Token.h"



string Token::typeToString(Token::Type tt) {
    switch (tt) {
        case Token::Type::ENDMARKER: return "ENDMARKER";
        case Token::Type::NAME: return "NAME";
        case Token::Type::NUMBER: return "NUMBER";
        case Token::Type::STRING: return "STRING";
        case Token::Type::NEWLINE: return "NEWLINE";
        case Token::Type::INDENT: return "INDENT";
        case Token::Type::DEDENT: return "DEDENT";
        case Token::Type::LPAR: return "LPAR";
        case Token::Type::RPAR: return "RPAR";
        case Token::Type::LSQB: return "LSQB";
        case Token::Type::RSQB: return "RSQB";
        case Token::Type::COLON: return "COLON";
        case Token::Type::COMMA: return "COMMA";
        case Token::Type::SEMI: return "SEMI";
        case Token::Type::PLUS: return "PLUS";
        case Token::Type::MINUS: return "MINUS";
        case Token::Type::STAR: return "STAR";
        case Token::Type::SLASH: return "SLASH";
        case Token::Type::VBAR: return "VBAR";
        case Token::Type::AMPER: return "AMPER";
        case Token::Type::LESS: return "LESS";
        case Token::Type::GREATER: return "GREATER";
        case Token::Type::EQUAL: return "EQUAL";
        case Token::Type::DOT: return "DOT";
        case Token::Type::PERCENT: return "PERCENT";
        case Token::Type::LBRACE: return "LBRACE";
        case Token::Type::RBRACE: return "RBRACE";
        case Token::Type::EQEQUAL: return "EQEQUAL";
        case Token::Type::NOTEQUAL: return "NOTEQUAL";
        case Token::Type::LESSEQUAL: return "LESSEQUAL";
        case Token::Type::GREATEREQUAL: return "GREATEREQUAL";
        case Token::Type::TILDE: return "TILDE";
        case Token::Type::CIRCUMFLEX: return "CIRCUMFLEX";
        case Token::Type::LEFTSHIFT: return "LEFTSHIFT";
        case Token::Type::RIGHTSHIFT: return "RIGHTSHIFT";
        case Token::Type::DOUBLESTAR: return "DOUBLESTAR";
        case Token::Type::PLUSEQUAL: return "PLUSEQUAL";
        case Token::Type::MINEQUAL: return "MINEQUAL";
        case Token::Type::STAREQUAL: return "STAREQUAL";
        case Token::Type::SLASHEQUAL: return "SLASHEQUAL";
        case Token::Type::PERCENTEQUAL: return "PERCENTEQUAL";
        case Token::Type::AMPEREQUAL: return "AMPEREQUAL";
        case Token::Type::VBAREQUAL: return "VBAREQUAL";
        case Token::Type::CIRCUMFLEXEQUAL: return "CIRCUMFLEXEQUAL";
        case Token::Type::LEFTSHIFTEQUAL: return "LEFTSHIFTEQUAL";
        case Token::Type::RIGHTSHIFTEQUAL: return "RIGHTSHIFTEQUAL";
        case Token::Type::DOUBLESTAREQUAL: return "DOUBLESTAREQUAL";
        case Token::Type::DOUBLESLASH: return "DOUBLESLASH";
        case Token::Type::DOUBLESLASHEQUAL: return "DOUBLESLASHEQUAL";
        case Token::Type::AT: return "AT";
        case Token::Type::ATEQUAL: return "ATEQUAL";
        case Token::Type::RARROW: return "RARROW";
        case Token::Type::ELLIPSIS: return "ELLIPSIS";
        case Token::Type::COLONEQUAL: return "COLONEQUAL";
        case Token::Type::OP: return "OP";
        case Token::Type::AWAIT: return "AWAIT";
        case Token::Type::ASYNC: return "ASYNC";
        case Token::Type::TYPE_IGNORE: return "TYPE_IGNORE";
        case Token::Type::TYPE_COMMENT: return "TYPE_COMMENT";
        case Token::Type::SOFT_KEYWORD: return "SOFT_KEYWORD";
        case Token::Type::ERRORTOKEN: return "ERRORTOKEN";
        case Token::Type::COMMENT: return "COMMENT";
        case Token::Type::NL: return "NL";
        case Token::Type::ENCODING: return "ENCODING";
    }
}