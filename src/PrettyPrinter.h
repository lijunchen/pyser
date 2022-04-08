#ifndef PRETTYPRINTER_H
#define PRETTYPRINTER_H
#include "Visitor.h"
#include <exception>
#include <stdexcept>

using std::runtime_error;

struct PPContext {
    int level = 0;
    string s;
};

class PrettyPrinter: public Visitor {
public:
    PPContext ctx;
public:
    string indent() { return string(ctx.level * 4, ' '); }
    string operatorToString(operator_ op);
    string unaryopToString(unaryop op);
    virtual void visit(Module&) override;
    virtual void visit(Interactive&) override { throw runtime_error("not imp yet"); };
    virtual void visit(Expression&) override { throw runtime_error("not imp yet"); };
    virtual void visit(FunctionType&) override { throw runtime_error("not imp yet"); };
    virtual void visit(FunctionDef&) override { throw runtime_error("not imp yet"); };
    virtual void visit(AsyncFunctionDef&) override { throw runtime_error("not imp yet"); };
    virtual void visit(ClassDef&) override { throw runtime_error("not imp yet"); };
    virtual void visit(Return&) override { throw runtime_error("not imp yet"); };
    virtual void visit(Delete&) override { throw runtime_error("not imp yet"); };
    virtual void visit(Assign&) override { throw runtime_error("not imp yet"); };
    virtual void visit(AugAssign&) override { throw runtime_error("not imp yet"); };
    virtual void visit(AnnAssign&) override { throw runtime_error("not imp yet"); };
    virtual void visit(For&) override { throw runtime_error("not imp yet"); };
    virtual void visit(AsyncFor&) override { throw runtime_error("not imp yet"); };
    virtual void visit(While&) override;
    virtual void visit(If&) override;
    virtual void visit(With&) override { throw runtime_error("not imp yet"); };
    virtual void visit(AsyncWith&) override { throw runtime_error("not imp yet"); };
    virtual void visit(Match&) override { throw runtime_error("not imp yet"); };
    virtual void visit(Raise&) override { throw runtime_error("not imp yet"); };
    virtual void visit(Try&) override { throw runtime_error("not imp yet"); };
    virtual void visit(Assert&) override { throw runtime_error("not imp yet"); };
    virtual void visit(Import&) override { throw runtime_error("not imp yet"); };
    virtual void visit(ImportFrom&) override { throw runtime_error("not imp yet"); };
    virtual void visit(Global&) override { throw runtime_error("not imp yet"); };
    virtual void visit(Nonlocal&) override { throw runtime_error("not imp yet"); };
    virtual void visit(Expr&) override;
    virtual void visit(Pass&) override { throw runtime_error("not imp yet"); };
    virtual void visit(Break&) override { throw runtime_error("not imp yet"); };
    virtual void visit(Continue&) override { throw runtime_error("not imp yet"); };
    virtual void visit(BoolOp&) override { throw runtime_error("not imp yet"); };
    virtual void visit(NamedExpr&) override { throw runtime_error("not imp yet"); };
    virtual void visit(BinOp&) override;
    virtual void visit(UnaryOp&) override;
    virtual void visit(Lambda&) override { throw runtime_error("not imp yet"); };
    virtual void visit(IfExp&) override { throw runtime_error("not imp yet"); };
    virtual void visit(Dict&) override { throw runtime_error("not imp yet"); };
    virtual void visit(Set&) override { throw runtime_error("not imp yet"); };
    virtual void visit(ListComp&) override { throw runtime_error("not imp yet"); };
    virtual void visit(SetComp&) override { throw runtime_error("not imp yet"); };
    virtual void visit(DictComp&) override { throw runtime_error("not imp yet"); };
    virtual void visit(GeneratorExp&) override { throw runtime_error("not imp yet"); };
    virtual void visit(Await&) override { throw runtime_error("not imp yet"); };
    virtual void visit(Yield&) override { throw runtime_error("not imp yet"); };
    virtual void visit(YieldFrom&) override { throw runtime_error("not imp yet"); };
    virtual void visit(Compare&) override { throw runtime_error("not imp yet"); };
    virtual void visit(Call&) override { throw runtime_error("not imp yet"); };
    virtual void visit(FormattedValue&) override { throw runtime_error("not imp yet"); };
    virtual void visit(JoinedStr&) override { throw runtime_error("not imp yet"); };
    virtual void visit(Constant&) override { throw runtime_error("not imp yet"); };
    virtual void visit(Str&) override;
    virtual void visit(Num&) override;
    virtual void visit(Attribute&) override { throw runtime_error("not imp yet"); };
    virtual void visit(Subscript&) override { throw runtime_error("not imp yet"); };
    virtual void visit(Starred&) override { throw runtime_error("not imp yet"); };
    virtual void visit(Name&) override;
    virtual void visit(List&) override { throw runtime_error("not imp yet"); };
    virtual void visit(Tuple&) override { throw runtime_error("not imp yet"); };
    virtual void visit(Slice&) override { throw runtime_error("not imp yet"); };
    virtual void visit(comprehension&) override { throw runtime_error("not imp yet"); };
    virtual void visit(exceptHandler&) override { throw runtime_error("not imp yet"); };
    virtual void visit(arguments&) override { throw runtime_error("not imp yet"); };
    virtual void visit(arg&) override { throw runtime_error("not imp yet"); };
    virtual void visit(keyword&) override { throw runtime_error("not imp yet"); };
    virtual void visit(alias&) override { throw runtime_error("not imp yet"); };
    virtual void visit(withitem&) override { throw runtime_error("not imp yet"); };
    virtual void visit(match_case&) override { throw runtime_error("not imp yet"); };
    virtual void visit(MatchValue&) override { throw runtime_error("not imp yet"); };
    virtual void visit(MatchSingleton&) override { throw runtime_error("not imp yet"); };
    virtual void visit(MatchSequence&) override { throw runtime_error("not imp yet"); };
    virtual void visit(MatchMapping&) override { throw runtime_error("not imp yet"); };
    virtual void visit(MatchClass&) override { throw runtime_error("not imp yet"); };
    virtual void visit(MatchStar&) override { throw runtime_error("not imp yet"); };
    virtual void visit(MatchAs&) override { throw runtime_error("not imp yet"); };
    virtual void visit(MatchOr&) override { throw runtime_error("not imp yet"); };
    virtual void visit(type_ignore&) override { throw runtime_error("not imp yet"); };
};

#endif /* PRETTYPRINTER_H */
