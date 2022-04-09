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

unordered_map<Token, optional<BindingPower>> infixTable;
unordered_map<Token, optional<BindingPower>> prefixTable;
unordered_map<Token, optional<BindingPower>> postfixTable;

vector<tuple<Fix, Assoc, vector<Token>>> table = {
    // Precedence from low to high

    { Fix::In, Assoc::Left, { Token(Token::Type::COMMA, ",") }},
    { Fix::In, Assoc::Left, { Token(Token::Type::NAME, "if") }},
    { Fix::In, Assoc::Left, { Token(Token::Type::NAME, "or") }},
    { Fix::In, Assoc::Left, { Token(Token::Type::NAME, "and") }},

    { Fix::Pre, Assoc::Non, { Token(Token::Type::NAME, "not") }},

    { Fix::In, Assoc::Left, {
                Token(Token::Type::EQEQUAL, "=="),
                Token(Token::Type::NOTEQUAL, "!="),
                Token(Token::Type::LESSEQUAL, "<="),
                Token(Token::Type::LESS, "<"),
                Token(Token::Type::GREATEREQUAL, ">="),
                Token(Token::Type::GREATER, ">"),
                Token(Token::Type::NAME, "not in"),
                Token(Token::Type::NAME, "in"),
                Token(Token::Type::NAME, "is not"),
                Token(Token::Type::NAME, "is"),
                } },

    { Fix::In, Assoc::Left, { Token(Token::Type::VBAR, "|") } },       // |
    { Fix::In, Assoc::Left, { Token(Token::Type::CIRCUMFLEX, "^") } }, // ^
    { Fix::In, Assoc::Left, { Token(Token::Type::AMPER, "&") } },      // &

    { Fix::In, Assoc::Left, {
                Token(Token::Type::LEFTSHIFT, "<<"),
                Token(Token::Type::RIGHTSHIFT, ">>"),
                } },

    { Fix::In, Assoc::Left, {
                Token(Token::Type::PLUS, "+"),
                Token(Token::Type::MINUS, "-"),
                } },

    { Fix::In, Assoc::Left, {
                Token(Token::Type::STAR, "*"),
                Token(Token::Type::SLASH, "/"),
                Token(Token::Type::DOUBLESLASH, "//"),
                Token(Token::Type::PERCENT, "%"),
                Token(Token::Type::AT, "@"),
                } },

    { Fix::Pre, Assoc::Non, { Token(Token::Type::PLUS, "+") } },
    { Fix::Pre, Assoc::Non, { Token(Token::Type::MINUS, "-") } },
    { Fix::Pre, Assoc::Non, { Token(Token::Type::TILDE, "~") } },

    { Fix::In, Assoc::Right, { Token(Token::Type::DOUBLESTAR, "**") } },

    { Fix::Pre, Assoc::Non, { Token(Token::Type::NAME, "await") }},

    { Fix::In, Assoc::Left, { Token(Token::Type::DOT, ".") } },
    { Fix::In, Assoc::Left, { Token(Token::Type::LPAR, "(") } },
    { Fix::In, Assoc::Left, { Token(Token::Type::LSQB, ")") } },
};

void initBindingPowerTables() {
    printf("initBindingPowerTables\n");
    for (int i = 0; i < table.size(); i++) {
        int l = i + 1;
        Fix bt = std::get<0>(table[i]);
        Assoc at = std::get<1>(table[i]);
        vector<Token> ts = std::get<2>(table[i]);
        if (bt == Fix::In) {
            for (const Token& t : ts) {
                if (at == Assoc::Left) {
                    infixTable[t] = BindingPower(l * 2 - 1, l * 2);
                } else {
                    infixTable[t] = BindingPower(l * 2, l * 2 - 1);
                }
            }
        } else if (bt == Fix::Pre) {
            for (const Token& t : ts) {
                prefixTable[t] = BindingPower(nullopt, l * 2);
            }
        } else if (bt == Fix::Post) {
            for (const Token& t : ts) {
                postfixTable[t] = BindingPower(l * 2, nullopt);
            }
        }
    }
}

optional<BindingPower> prefix_binding_power(const Token& t) {
    if (prefixTable.count(t)) {
        return prefixTable[t];
    }
    return nullopt;
}

optional<BindingPower> post_binding_power(const Token& t) {
    if (postfixTable.count(t)) {
        return postfixTable[t];
    }
    return nullopt;
}

optional<BindingPower> infix_binding_power(const Token& t) {
    if (infixTable.count(t)) {
        return infixTable[t];
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
        if (tok.type == Token::Type::PLUS) {
            lhs = make_unique<UnaryOp>(unaryop::UAdd, move(rhs));
        } else if (tok.type == Token::Type::MINUS) {
            lhs = make_unique<UnaryOp>(unaryop::USub, move(rhs));
        } else if (tok.type == Token::Type::TILDE) {
            lhs = make_unique<UnaryOp>(unaryop::Invert, move(rhs));
        } else if (tok.type == Token::Type::NAME) {
            if (tok.raw == "not") {
                lhs = make_unique<UnaryOp>(unaryop::Not, move(rhs));
            } else if (tok.raw == "await") {
                lhs = make_unique<Await>(move(rhs));
            }
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
        if (!bp) {
            printf("bp of %s is nullopt\n", t.toString().c_str());
            break;
        }
        printf("while next token: %s, bp: %d, %d\n", t.toString().c_str(), bp->left.value(), bp->right.value());
        if (*bp->left < minBP) {
            break;
        }
        next();
        if (t.is_operator()) {
            operator_ op{};
            bool done = false;
            switch (t.type) {
                case Token::Type::PLUS: op = operator_::Add; break;
                case Token::Type::MINUS: op = operator_::Sub; break;
                case Token::Type::STAR: op = operator_::Mult; break;
                case Token::Type::SLASH: op = operator_::Div; break;
                case Token::Type::DOUBLESTAR: op = operator_::Pow; break;
                case Token::Type::DOT: {
                    if (const Token& attr = expectT(Token::Type::NAME)) {
                        lhs = make_unique<Attribute>(move(lhs), attr.raw, expr_context::Load);
                        done = true;
                    }
                }
                case Token::Type::LPAR: {
                    break;
                }
                case Token::Type::LSQB: {
                    exprP rhs = slices();
                    if (rhs) {
                        lhs = make_unique<Subscript>(move(lhs), move(rhs), expr_context::Load);
                        if (expect(Token::Type::RSQB)) {
                            done = true;
                            break;
                        } else {
                            throw std::runtime_error("expect ]");
                        }
                    }
                    break;
                }
                default:
                    break;
            }
            if (!done) {
                exprP rhs = pratt_parser_bp(*bp->right);
                lhs = make_unique<BinOp>(std::move(lhs), op, std::move(rhs));
            }
        } else if (t.is_boolop()) {
            boolop op{};
            if (t.raw == "and") {
                op = boolop::And;
            } else if (t.raw == "or") {
                op = boolop::Or;
            } else {
                ; // impossible
            }

            exprP rhs = pratt_parser_bp(*bp->right);
            BoolOp* p = dynamic_cast<BoolOp*>(lhs.get());
            if (p && p->op == op) {
                p->values.push_back(move(rhs));
            } else {
                vector<exprP> values;
                values.push_back(move(lhs));
                values.push_back(move(rhs));
                lhs = make_unique<BoolOp>(op, move(values));
            }
        } else if (t.is_cmpop()) {
            // { Eq, NotEq, Lt, LtE, Gt, GtE, Is, IsNot, In, NotIn};
            cmpop op{};
            switch (t.type) {
                case Token::Type::EQUAL: op = cmpop::Eq; break;
                case Token::Type::NOTEQUAL: op = cmpop::NotEq; break;
                case Token::Type::LESS: op = cmpop::Lt; break;
                case Token::Type::LESSEQUAL: op = cmpop::LtE; break;
                case Token::Type::GREATER: op = cmpop::Gt; break;
                case Token::Type::GREATEREQUAL: op = cmpop::GtE; break;
                case Token::Type::NAME: {
                    const Token& t2 = peek();
                    if (t.raw == "not" && t2.raw == "in") {
                        op = cmpop::NotIn;
                        break;
                    }
                    if (t.raw == "in") {
                        op = cmpop::In;
                        break;
                    }
                    if (t.raw == "is") {
                        if (t2.raw == "not") {
                            next();
                            op = cmpop::IsNot;
                            break;
                        }
                        op = cmpop::Is;
                        break;
                    }
                }
                default:
                    break;
            }
            exprP rhs = pratt_parser_bp(*bp->right);
            Compare* p = dynamic_cast<Compare*>(lhs.get());
            if (p) {
                p->ops.push_back(op);
                p->comparators.push_back(move(rhs));
            } else {
                vector<cmpop> ops;
                ops.push_back(op);
                vector<exprP> comparators;
                comparators.push_back(move(rhs));
                lhs = make_unique<Compare>(move(lhs), ops, move(comparators));
            }
        } else if (t.type == Token::Type::COMMA) {
            exprP rhs = pratt_parser_bp(*bp->right);
            if (Tuple* p = dynamic_cast<Tuple*>(lhs.get())) {
                p->elts.push_back(move(rhs));
            } else {
                vector<exprP> elts;
                elts.push_back(move(lhs));
                elts.push_back(move(rhs));
                lhs = make_unique<Tuple>(move(elts), expr_context::Load);
            }
        } else if (t.type == Token::Type::NAME && t.raw == "if") {
            exprP test = pratt_parser_bp(*bp->right);
            if (!expect("else")) {
                throw std::runtime_error("expect else in if expr");
            }
            exprP orelse = pratt_parser_bp(*bp->right);
            lhs = make_unique<IfExp>(move(test), move(lhs), move(orelse));
        } else {
            break;
        }

    }

    if (lhs) {
        return lhs;
    }

    reset(p);
    return nullptr;
}