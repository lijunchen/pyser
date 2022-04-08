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

enum class Fix { Pre, Post, In };
enum class Assoc { Non, Left, Right };

string fixToString(Fix f) {
    switch (f) {
    case Fix::Pre:
        return "prefix";
    case Fix::Post:
        return "postfix";
    case Fix::In:
        return "infix";
    }
}

string assocToString(Assoc at) {
    switch (at) {
    case Assoc::Non:
        return "non";
    case Assoc::Left:
        return "left";
    case Assoc::Right:
        return "right";
    }
}

unordered_map<TokenType, optional<BindingPower>> infixTable;
unordered_map<TokenType, optional<BindingPower>> prefixTable;
unordered_map<TokenType, optional<BindingPower>> postfixTable;

vector<tuple<Fix, Assoc, vector<TokenType>>> table = {
    // Precedence from low to high

    { Fix::In, Assoc::Left, { TokenType::EQEQUAL,
                TokenType::NOTEQUAL,
                TokenType::LESSEQUAL,
                TokenType::LESS,
                TokenType::GREATEREQUAL,
                TokenType::GREATER } },

    { Fix::In, Assoc::Left, { TokenType::VBAR } },       // |
    { Fix::In, Assoc::Left, { TokenType::CIRCUMFLEX } }, // ^
    { Fix::In, Assoc::Left, { TokenType::AMPER } },      // &

    { Fix::In, Assoc::Left, { TokenType::LEFTSHIFT,
               TokenType::RIGHTSHIFT } },

    { Fix::In, Assoc::Left, { TokenType::PLUS,
                TokenType::MINUS } },

    { Fix::In, Assoc::Left, { TokenType::STAR,
                TokenType::SLASH,
                TokenType::DOUBLESLASH,
                TokenType::PERCENT,
                TokenType::AT } },

    { Fix::Pre, Assoc::Non, { TokenType::PLUS } },
    { Fix::Pre, Assoc::Non, { TokenType::MINUS } },
    { Fix::Pre, Assoc::Non, { TokenType::TILDE } },

    { Fix::In, Assoc::Right, { TokenType::DOUBLESTAR } },

    { Fix::Post, Assoc::Non, { TokenType::DOT } },
    { Fix::Post, Assoc::Non, { TokenType::LPAR } },
    { Fix::Post, Assoc::Non, { TokenType::LSQB } },
};

void initBindingPowerTables() {
    printf("initBindingPowerTables\n");
    for (int i = 0; i < table.size(); i++) {
        Fix bt = std::get<0>(table[i]);
        Assoc at = std::get<1>(table[i]);
        vector<TokenType> tts = std::get<2>(table[i]);
        if (bt == Fix::In) {
            for (TokenType tt : tts) {
                if (at == Assoc::Left) {
                    infixTable[tt] = BindingPower(i, i + 1);
                    printf("%s %s %s %d %d\n", fixToString(bt).c_str(), tokenTypeToString(tt).c_str(), assocToString(at).c_str(), i, i + 1);
                } else {
                    infixTable[tt] = BindingPower(i + 1, i);
                    printf("%s %s %s %d %d\n", fixToString(bt).c_str(), tokenTypeToString(tt).c_str(), assocToString(at).c_str(), i + 1, i);
                }
            }
        } else if (bt == Fix::Pre) {
            for (TokenType tt : tts) {
                prefixTable[tt] = BindingPower(nullopt, i);
                printf("%s %s %s %d %d\n", fixToString(bt).c_str(), tokenTypeToString(tt).c_str(), assocToString(at).c_str(), i, i + 1);
            }
        } else if (bt == Fix::Post) {
            for (TokenType tt : tts) {
                postfixTable[tt] = BindingPower(i, nullopt);
                printf("%s %s %s %d %d\n", fixToString(bt).c_str(), tokenTypeToString(tt).c_str(), assocToString(at).c_str(), i, i + 1);
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