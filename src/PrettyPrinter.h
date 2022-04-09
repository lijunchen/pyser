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
public:
    string contextToString(expr_context ctx);
    string operatorToString(operator_ op);
    string unaryopToString(unaryop op);
    string boolopToString(boolop op);
    string cmpopToString(cmpop op);

public:
    virtual void visit(Module&) override;
    virtual void visit(Interactive&) override { throw runtime_error("not imp yet"); };
    virtual void visit(Expression&) override { throw runtime_error("not imp yet"); };
    virtual void visit(FunctionType&) override { throw runtime_error("not imp yet"); };
    virtual void visit(FunctionDef&) override;
    virtual void visit(AsyncFunctionDef&) override { throw runtime_error("not imp yet"); };
    virtual void visit(ClassDef&) override;
    virtual void visit(Return&) override;
    virtual void visit(Delete&) override;
    virtual void visit(Assign&) override;
    virtual void visit(AugAssign&) override;
    virtual void visit(AnnAssign&) override;
    virtual void visit(For&) override;
    virtual void visit(AsyncFor&) override { throw runtime_error("not imp yet"); };
    virtual void visit(While&) override;
    virtual void visit(If&) override;
    virtual void visit(With&) override;
    virtual void visit(AsyncWith&) override { throw runtime_error("not imp yet"); };
    virtual void visit(Match&) override { throw runtime_error("not imp yet"); };
    virtual void visit(Raise&) override;
    virtual void visit(Try&) override;
    virtual void visit(Assert&) override;
    virtual void visit(Import&) override;
    virtual void visit(ImportFrom&) override;
    virtual void visit(Global&) override;
    virtual void visit(Nonlocal&) override;
    virtual void visit(Expr&) override;
    virtual void visit(Pass&) override;
    virtual void visit(Break&) override;
    virtual void visit(Continue&) override;
    virtual void visit(BoolOp&) override;
    virtual void visit(NamedExpr&) override { throw runtime_error("not imp yet"); };
    virtual void visit(BinOp&) override;
    virtual void visit(UnaryOp&) override;
    virtual void visit(Lambda&) override;
    virtual void visit(IfExp&) override;
    virtual void visit(Dict&) override;
    virtual void visit(Set&) override;
    virtual void visit(ListComp&) override { throw runtime_error("not imp yet"); };
    virtual void visit(SetComp&) override { throw runtime_error("not imp yet"); };
    virtual void visit(DictComp&) override { throw runtime_error("not imp yet"); };
    virtual void visit(GeneratorExp&) override { throw runtime_error("not imp yet"); };
    virtual void visit(Await&) override;
    virtual void visit(Yield&) override;
    virtual void visit(YieldFrom&) override;
    virtual void visit(Compare&) override;
    virtual void visit(Call&) override;
    virtual void visit(FormattedValue&) override { throw runtime_error("not imp yet"); };
    virtual void visit(JoinedStr&) override { throw runtime_error("not imp yet"); };
    virtual void visit(Constant&) override { throw runtime_error("not imp yet"); };
    virtual void visit(Str&) override;
    virtual void visit(Num&) override;
    virtual void visit(Bool&) override;
    virtual void visit(None&) override;
    virtual void visit(Attribute&) override;
    virtual void visit(Subscript&) override;
    virtual void visit(Starred&) override;
    virtual void visit(Name&) override;
    virtual void visit(List&) override;
    virtual void visit(Tuple&) override;
    virtual void visit(Slice&) override;
    virtual void visit(comprehension&) override { throw runtime_error("not imp yet"); };
    virtual void visit(exceptHandler&) override { throw runtime_error("not imp yet"); };
    virtual void visit(arguments&) override;
    virtual void visit(arg&) override;
    virtual void visit(keyword&) override;
    virtual void visit(alias&) override;
    virtual void visit(withitem&) override;
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
