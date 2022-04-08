#include "Parser.h"
#include "Token.h"
#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <unordered_map>

using std::make_unique;
using std::optional;
using std::pair;
using std::string;
using std::unique_ptr;
using std::unordered_map;
using std::tuple;

class BindingPower {
public:
    BindingPower(optional<int> left, optional<int> right) : left(left), right(right) {}

public:
    optional<int> left;
    optional<int> right;
};

enum BindingType { PREFIX, POSTFIX, INFIX };
enum AssocType { LEFT, RIGHT };

string assocTypeToString(AssocType at) {
    if (at == LEFT) {
        return "LEFT";
    }
    return "RIGHT";
}

unordered_map<TokenType, optional<BindingPower>> infixTable;
unordered_map<TokenType, optional<BindingPower>> prefixTable;
unordered_map<TokenType, optional<BindingPower>> postfixTable;

vector<tuple<BindingType, AssocType, vector<TokenType>>> table = {
    // Precedence from low to high

    { INFIX, LEFT, { TokenType::EQEQUAL,
                TokenType::NOTEQUAL,
                TokenType::LESSEQUAL,
                TokenType::LESS,
                TokenType::GREATEREQUAL,
                TokenType::GREATER } },

    { INFIX, LEFT, { TokenType::VBAR } },       // |
    { INFIX, LEFT, { TokenType::CIRCUMFLEX } }, // ^
    { INFIX, LEFT, { TokenType::AMPER } },      // &

    { INFIX, LEFT, { TokenType::LEFTSHIFT,
               TokenType::RIGHTSHIFT } },

    { INFIX, LEFT, { TokenType::PLUS,
                TokenType::MINUS } },

    { INFIX, LEFT, { TokenType::STAR,
                TokenType::SLASH,
                TokenType::DOUBLESLASH,
                TokenType::PERCENT,
                TokenType::AT } },

    { PREFIX, LEFT, { TokenType::PLUS } },
    { PREFIX, LEFT, { TokenType::MINUS } },
    { PREFIX, LEFT, { TokenType::TILDE } },

    { INFIX, RIGHT, { TokenType::DOUBLESTAR } },

    { POSTFIX, LEFT, { TokenType::DOT } },
    { POSTFIX, LEFT, { TokenType::LPAR } },
    { POSTFIX, LEFT, { TokenType::LSQB } },
};

void initBindingPowerTables() {
    printf("initBindingPowerTables\n");
    for (int i = 0; i < table.size(); i++) {
        BindingType bt = std::get<0>(table[i]);
        AssocType at = std::get<1>(table[i]);
        vector<TokenType> tts = std::get<2>(table[i]);
        if (bt == INFIX) {
            for (TokenType tt : tts) {
                if (at == LEFT) {
                    infixTable[tt] = BindingPower(i, i + 1);
                    printf("infix %s %s %d %d\n", tokenTypeToString(tt).c_str(), assocTypeToString(at).c_str(), i, i + 1);
                } else {
                    infixTable[tt] = BindingPower(i + 1, i);
                    printf("infix %s %s %d %d\n", tokenTypeToString(tt).c_str(), assocTypeToString(at).c_str(), i + 1, i);
                }
            }
        } else if (bt == PREFIX) {
            for (TokenType tt : tts) {
                prefixTable[tt] = BindingPower(nullopt, i);
                printf("prefix %s %s %d %d\n", tokenTypeToString(tt).c_str(), assocTypeToString(at).c_str(), i, i + 1);
            }
        } else if (bt == POSTFIX) {
            for (TokenType tt : tts) {
                postfixTable[tt] = BindingPower(i, nullopt);
                printf("postfix %s %s %d %d\n", tokenTypeToString(tt).c_str(), assocTypeToString(at).c_str(), i, i + 1);
            }
        }
    }
}

optional<BindingPower> prefix_binding_power(const Token& t) {
    if (prefixTable.count(t.type)) {
        return prefixTable[t.type];
    }
    return nullopt;
}

optional<BindingPower> post_binding_power(const Token& t) {
    if (postfixTable.count(t.type)) {
        return postfixTable[t.type];
    }
    return nullopt;
}

optional<BindingPower> infix_binding_power(const Token& t) {
    if (infixTable.count(t.type)) {
        return infixTable[t.type];
    }
    return nullopt;
}

exprP Parser::pratt_parser() { return pratt_parser_bp(0); }

exprP Parser::pratt_parser_bp(int minBP) {
    int p = mark();
    const Token& tok = peek();
    exprP lhs;
    if (optional<BindingPower> bp = prefix_binding_power(tok)) {
        next();
        exprP rhs = pratt_parser_bp(*bp->right);
        if (tok.type == TokenType::PLUS) {
            lhs = make_unique<UnaryOp>(unaryop::UAdd, move(rhs));
        } else if (tok.type == TokenType::MINUS) {
            lhs = make_unique<UnaryOp>(unaryop::USub, move(rhs));
        } else if (tok.type == TokenType::TILDE) {
            lhs = make_unique<UnaryOp>(unaryop::Invert, move(rhs));
        }
    } else {
        lhs = atom();
    }
    if (!lhs) {
        reset(p);
        return nullptr;
    }

    while (true) {
        const Token& t = peek();
        optional<BindingPower> bp = infix_binding_power(t);
        if (!bp || !bp->right) {
            break;
        }
        if (bp->left.value() < minBP) {
            break;
        }
        next();
        exprP rhs = pratt_parser_bp(*bp->right);
        operator_ op{};
        bool ok = true;
        switch (t.type) {
        case TokenType::PLUS: op = operator_::Add; break;
        case TokenType::MINUS: op = operator_::Sub; break;
        case TokenType::STAR: op = operator_::Mult; break;
        case TokenType::SLASH: op = operator_::Div; break;
        case TokenType::DOUBLESTAR: op = operator_::Pow; break;
        default:
            ok = false;
            break;
        }
        if (!ok) {
            break;
        }
        lhs = make_unique<BinOp>(std::move(lhs), op, std::move(rhs));
    }

    if (lhs) {
        return lhs;
    }

    reset(p);
    return nullptr;
}