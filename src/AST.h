#pragma once

#include "Visitor.h"
#include <memory>
#include <optional>
#include <string>
#include <vector>

using std::optional;
using std::string;
using std::unique_ptr;
using std::vector;

enum class expr_context { Load, Store, Del };
enum class boolop { And, Or };
enum class operator_ {
    Add,
    Sub,
    Mult,
    MatMult,
    Div,
    Mod,
    Pow,
    LShift,
    RShift,
    BitOr,
    BitXor,
    BitAnd,
    FloorDiv
};
enum class unaryop { Invert, Not, UAdd, USub };
enum class cmpop { Eq, NotEq, Lt, LtE, Gt, GtE, Is, IsNot, In, NotIn };

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
    virtual void set_expr_context(expr_context) { throw "not implemented"; }
};

typedef unique_ptr<stmt> stmtP;
typedef unique_ptr<expr> exprP;
typedef vector<stmtP> stmtPs;
typedef vector<exprP> exprPs;
typedef unique_ptr<arg> argP;
typedef vector<unique_ptr<arg>> argPs;

class Module: public mod {
public:
    Module(stmtPs body): body(move(body)) {}
    void accept(Visitor& visitor) override { visitor.visit(*this); }

public:
    stmtPs body;
};

class Assign: public stmt {
public:
    Assign(exprPs targets, exprP value)
        : targets(move(targets)), value(move(value)) {}
    virtual void accept(Visitor& visitor) override { visitor.visit(*this); }

public:
    exprPs targets;
    exprP value;
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
    BinOp(exprP left, operator_ op, exprP right)
        : left(move(left)), op(op), right(move(right)) {}
    virtual void accept(Visitor& visitor) override { visitor.visit(*this); }

public:
    exprP left;
    operator_ op;
    exprP right;
};

class UnaryOp: public expr {
public:
    UnaryOp(unaryop op, exprP operand): op(op), operand(move(operand)) {}
    virtual void accept(Visitor& visitor) override { visitor.visit(*this); }

public:
    unaryop op;
    exprP operand;
};

class BoolOp: public expr {
public:
    BoolOp(boolop op, exprPs values): op(op), values(move(values)) {}
    virtual void accept(Visitor& visitor) override { visitor.visit(*this); }

public:
    boolop op;
    vector<exprP> values;
};

class Compare: public expr {
public:
    Compare(exprP left, vector<cmpop> ops, exprPs comparators)
        : left(move(left)), ops(ops), comparators(move(comparators)) {}
    virtual void accept(Visitor& visitor) override { visitor.visit(*this); }

public:
    exprP left;
    vector<cmpop> ops;
    exprPs comparators;
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
    Str(const string& value, const optional<string>& kind)
        : value(value), kind(kind) {}
    virtual void accept(Visitor& visitor) override { visitor.visit(*this); }

public:
    string value;
    optional<string> kind;
};

class Bool: public expr {
public:
    Bool(string value): value(value) {}
    virtual void accept(Visitor& visitor) override { visitor.visit(*this); }

public:
    string value;
};

class None: public expr {
public:
    None() {}
    virtual void accept(Visitor& visitor) override { visitor.visit(*this); }
};

class Name: public expr {
public:
    Name(const string& id, const expr_context& ctx): id(id), ctx(ctx) {}
    virtual void accept(Visitor& visitor) override { visitor.visit(*this); }
    virtual void set_expr_context(expr_context ctx) override {
        this->ctx = ctx;
    }

public:
    string id;
    expr_context ctx;
};

class Await: public expr {
public:
    Await(exprP value): value(move(value)) {}
    virtual void accept(Visitor& visitor) override { visitor.visit(*this); }

public:
    exprP value;
};

class Attribute: public expr {
public:
    Attribute(exprP value, const string& attr, expr_context ctx)
        : value(move(value)), attr(attr), ctx(ctx) {}
    virtual void accept(Visitor& visitor) override { visitor.visit(*this); }
    virtual void set_expr_context(expr_context ctx) override {
        this->ctx = ctx;
    }

public:
    exprP value;
    string attr;
    expr_context ctx;
};

class Subscript: public expr {
public:
    Subscript(exprP value, exprP slice, expr_context ctx)
        : value(move(value)), slice(move(slice)), ctx(ctx) {}
    virtual void accept(Visitor& visitor) override { visitor.visit(*this); }
    virtual void set_expr_context(expr_context ctx) override {
        this->ctx = ctx;
    }

public:
    exprP value;
    exprP slice;
    expr_context ctx;
};

class Call: public expr {
public:
    Call(exprP func, exprPs args, vector<unique_ptr<keyword>> keywords)
        : func(move(func)), args(move(args)), keywords(move(keywords)) {}
    virtual void accept(Visitor& visitor) override { visitor.visit(*this); }

public:
    exprP func;
    exprPs args;
    vector<unique_ptr<keyword>> keywords;
};

class List: public expr {
public:
    List(exprPs elts, expr_context ctx): elts(move(elts)), ctx(ctx) {}
    virtual void accept(Visitor& visitor) override { visitor.visit(*this); }
    virtual void set_expr_context(expr_context ctx) override {
        this->ctx = ctx;
    }

public:
    exprPs elts;
    expr_context ctx;
};

class Tuple: public expr {
public:
    Tuple(exprPs elts, expr_context ctx): elts(move(elts)), ctx(ctx) {}
    virtual void accept(Visitor& visitor) override { visitor.visit(*this); }
    virtual void set_expr_context(expr_context ctx) override {
        this->ctx = ctx;
    }

public:
    exprPs elts;
    expr_context ctx;
};

class Slice: public expr {
public:
    Slice(exprP lower, exprP upper, exprP step)
        : lower(move(lower)), upper(move(upper)), step(move(step)) {}
    virtual void accept(Visitor& visitor) override { visitor.visit(*this); }

public:
    exprP lower;
    exprP upper;
    exprP step;
};

class FunctionDef: public stmt {
public:
    FunctionDef(const string& name, unique_ptr<arguments> args, stmtPs body,
                exprPs decorator_list, exprP returns)
        : name(name), args(move(args)), body(move(body)),
          decorator_list(move(decorator_list)) {}
    virtual void accept(Visitor& visitor) override { visitor.visit(*this); }

public:
    string name;
    unique_ptr<arguments> args;
    stmtPs body;
    exprPs decorator_list;
    exprP returns;
};

class ClassDef: public stmt {
public:
    ClassDef(const string& name, exprPs bases, stmtPs body,
             exprPs decorator_list)
        : name(name), bases(move(bases)), body(move(body)),
          decorator_list(move(decorator_list)) {}
    virtual void accept(Visitor& visitor) override { visitor.visit(*this); }

public:
    string name;
    exprPs bases;
    vector<unique_ptr<keyword>> keywords;
    stmtPs body;
    exprPs decorator_list;
};

class Return: public stmt {
public:
    Return(exprP value): value(move(value)) {}
    virtual void accept(Visitor& visitor) override { visitor.visit(*this); }

public:
    exprP value;
};

class Delete: public stmt {
public:
    Delete(exprPs targets): targets(move(targets)) {}
    virtual void accept(Visitor& visitor) override { visitor.visit(*this); }

public:
    exprPs targets;
};

class AugAssign: public stmt {
public:
    AugAssign(exprP target, operator_ op, exprP value)
        : target(move(target)), op(op), value(move(value)) {}
    virtual void accept(Visitor& visitor) override { visitor.visit(*this); }

public:
    exprP target;
    operator_ op;
    exprP value;
};

class AnnAssign: public stmt {
public:
    AnnAssign(exprP target, exprP annotation, exprP value, int simple)
        : target(move(target)), annotation(move(annotation)),
          value(move(value)), simple(simple) {}
    virtual void accept(Visitor& visitor) override { visitor.visit(*this); }

public:
    exprP target;
    exprP annotation;
    exprP value;
    int simple;
};

class For: public stmt {
public:
    For(exprP target, exprP iter, stmtPs body, stmtPs orelse)
        : target(move(target)), iter(move(iter)), body(move(body)),
          orelse(move(orelse)) {}
    virtual void accept(Visitor& visitor) override { visitor.visit(*this); }

public:
    exprP target;
    exprP iter;
    stmtPs body;
    stmtPs orelse;
};

class With: public stmt {
public:
    virtual void accept(Visitor& visitor) override { visitor.visit(*this); }

public:
};

class Raise: public stmt {
public:
    virtual void accept(Visitor& visitor) override { visitor.visit(*this); }

public:
};

class Try: public stmt {
public:
    virtual void accept(Visitor& visitor) override { visitor.visit(*this); }

public:
    exprP exc;
    exprP cause;
};

class Assert: public stmt {
public:
    virtual void accept(Visitor& visitor) override { visitor.visit(*this); }
    Assert(exprP test, exprP msg): test(move(test)), msg(move(msg)) {}

public:
    exprP test;
    exprP msg;
};

class Import: public stmt {
public:
    virtual void accept(Visitor& visitor) override { visitor.visit(*this); }
    Import(std::vector<alias> alias): names(move(alias)) {}

public:
    std::vector<alias> names;
};

class ImportFrom: public stmt {
public:
    ImportFrom(optional<string> module, std::vector<alias> alias, int level)
        : module(move(module)), alias(move(alias)), level(std::move(level)) {}
    virtual void accept(Visitor& visitor) override { visitor.visit(*this); }

public:
    optional<string> module;
    std::vector<alias> alias;
    int level;
};

class Global: public stmt {
public:
    virtual void accept(Visitor& visitor) override { visitor.visit(*this); }

public:
};

class Nonlocal: public stmt {
public:
    virtual void accept(Visitor& visitor) override { visitor.visit(*this); }

public:
};

class Pass: public stmt {
public:
    virtual void accept(Visitor& visitor) override { visitor.visit(*this); }

public:
};

class Break: public stmt {
public:
    virtual void accept(Visitor& visitor) override { visitor.visit(*this); }

public:
};

class Continue: public stmt {
public:
    virtual void accept(Visitor& visitor) override { visitor.visit(*this); }

public:
};

class Lambda: public expr {
public:
    virtual void accept(Visitor& visitor) override { visitor.visit(*this); }

public:
};

class IfExp: public expr {
public:
    IfExp(exprP test, exprP body, exprP orelse)
        : test(move(test)), body(move(body)), orelse(move(orelse)) {}
    virtual void accept(Visitor& visitor) override { visitor.visit(*this); }

public:
    exprP test;
    exprP body;
    exprP orelse;
};

class Dict: public expr {
public:
    virtual void accept(Visitor& visitor) override { visitor.visit(*this); }

public:
};

class Set: public expr {
public:
    virtual void accept(Visitor& visitor) override { visitor.visit(*this); }

public:
};

class Yield: public expr {
public:
    Yield(exprP value): value(move(value)) {}
    virtual void accept(Visitor& visitor) override { visitor.visit(*this); }

public:
    exprP value;
};

class YieldFrom: public expr {
public:
    YieldFrom(exprP value): value(move(value)) {}
    virtual void accept(Visitor& visitor) override { visitor.visit(*this); }

public:
    exprP value;
};

class Starred: public expr {
public:
    Starred(exprP value, expr_context ctx): value(move(value)), ctx(ctx) {}
    virtual void accept(Visitor& visitor) override { visitor.visit(*this); }
    virtual void set_expr_context(expr_context ctx) override {
        this->ctx = ctx;
    }

public:
    exprP value;
    expr_context ctx;
};

class keyword {
public:
    keyword(const string& arg, exprP value): arg(arg), value(move(value)) {}
    virtual void accept(Visitor& visitor) { visitor.visit(*this); }

public:
    optional<string> arg;
    exprP value;
};

class arguments {
public:
    arguments(argPs posonlyargs, argPs args, argP vararg, argPs kwonlyargs,
              exprPs kw_defaults, argP kwarg, exprPs defaults)
        : posonlyargs(move(posonlyargs)), args(move(args)),
          vararg(move(vararg)), kwonlyargs(move(kwonlyargs)),
          kw_defaults(move(kw_defaults)), kwarg(move(kwarg)),
          defaults(move(defaults)) {}
    virtual void accept(Visitor& visitor) { visitor.visit(*this); }

public:
    argPs posonlyargs;
    argPs args;
    argP vararg;
    argPs kwonlyargs;
    exprPs kw_defaults;
    argP kwarg;
    exprPs defaults;
};

class arg {
public:
    arg(const string& argu, exprP annotation)
        : argu(argu), annotation(move(annotation)) {}
    virtual void accept(Visitor& visitor) { visitor.visit(*this); }

public:
    string argu;
    exprP annotation;
};

class alias {
public:
    alias(const string& name, optional<string> asname)
        : name(name), asname(asname) {}
    virtual void accept(Visitor& visitor) { visitor.visit(*this); }

public:
    string name;
    optional<string> asname;
};

class withitem {
public:
    withitem(exprP context_expr, exprP optional_vars)
        : context_expr(move(context_expr)), optional_vars(move(optional_vars)) {
    }
    virtual void accept(Visitor& visitor) { visitor.visit(*this); }

public:
    exprP context_expr;
    exprP optional_vars;
};

class NamedExpr: public expr {
public:
    NamedExpr(exprP target, exprP value)
        : target(move(target)), value(move(value)) {}
    virtual void accept(Visitor& visitor) { visitor.visit(*this); }

public:
    exprP target;
    exprP value;
};
