#ifndef AST_H
#define AST_H

#include <vector>
#include <string>
#include <memory>
#include <optional>
#include "Visitor.h"

using std::vector;
using std::string;
using std::unique_ptr;
using std::optional;


enum class expr_context { Load, Store, Del };
enum class boolop { And, Or };
enum class operator_ { Add, Sub, Mult, MatMult, Div, Mod, Pow, LShift, RShift, BitOr, BitXor, BitAnd, FloorDiv };
enum class unaryop { Invert, Not, UAdd, USub };
enum class cmpop { Eq, NotEq, Lt, LtE, Gt, GtE, Is, IsNot, In, NotIn};

class ast {
public:
    virtual ~ast() = default;
    virtual void accept(Visitor& visitor) = 0;
};

class mod: public ast {
public:
    virtual ~mod() = default;
};

class stmt: public ast {
public:
    virtual ~stmt() = default;
};

class expr: public ast {
public:
    virtual ~expr() = default;
};

typedef unique_ptr<stmt> stmtP;
typedef unique_ptr<expr> exprP;
typedef vector<stmtP> stmtPs;
typedef vector<exprP> exprPs;

class Module: public mod {
public:
    Module(stmtPs body): body(move(body)) {}
    void accept(Visitor& visitor) override { visitor.visit(*this); }

public:
    stmtPs body;
};

class While: public stmt {
public:
    While(exprP test, stmtPs body, stmtPs orelse)
    : test(move(test)), body(move(body)), orelse(move(orelse)) {}
    virtual void accept(Visitor& visitor) override { visitor.visit(*this); }

public:
    exprP test;
    stmtPs body;
    stmtPs orelse;
};

class If: public stmt {
public:
    If(exprP test, stmtPs body, stmtPs orelse)
    : test(move(test)), body(move(body)), orelse(move(orelse)) {}
    virtual void accept(Visitor& visitor) override { visitor.visit(*this); }

public:
    exprP test;
    stmtPs body;
    stmtPs orelse;
};

class Expr: public stmt {
public:
    Expr(exprP value): value(move(value)) {}
    virtual void accept(Visitor& visitor) override { visitor.visit(*this); }

public:
    exprP value;
};

class BinOp: public expr {
public:
    BinOp(exprP left, operator_ op, exprP right): left(move(left)), op(op), right(move(right)) {}
    virtual void accept(Visitor& visitor) override { visitor.visit(*this); }

public:
    exprP left;
    operator_ op;
    exprP right;
};

class Num: public expr {
public:
    Num(const string& value): value(value) {}
    virtual void accept(Visitor& visitor) override { visitor.visit(*this); }

public:
    string value;
};

class Str: public expr {
public:
    Str(const string& value, const optional<string>& kind): value(value), kind(kind) {}
    virtual void accept(Visitor& visitor) override { visitor.visit(*this); }

public:
    string value;
    optional<string> kind;
};

class Name: public expr {
public:
    Name(const string& id, const expr_context& ctx): id(id), ctx(ctx) {}
    virtual void accept(Visitor& visitor) override { visitor.visit(*this); }

public:
    string id;
    expr_context ctx;
};

#endif /* AST_H */