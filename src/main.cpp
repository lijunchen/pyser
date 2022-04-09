#include <cstdio>
#include <string>
#include <cmath>
#include <iostream>
#include "AST.h"
#include "PrettyPrinter.h"
#include "Parser.h"
#include <memory>

using namespace std;

int main() {
    stmtPs body;
    stmtP s0 = make_unique<Expr>(
        make_unique<BinOp>(
            make_unique<BinOp>(
                make_unique<Name>("x", expr_context::Load),
                operator_::Mult,
                make_unique<Name>("y", expr_context::Load)),
            operator_::Add,
            make_unique<BinOp>(
                make_unique<Num>("123"),
                operator_::Mult,
                make_unique<Name>("y", expr_context::Load))
        )
    );
    body.push_back(move(s0));
    stmtPs orelse;

    stmtP tree = make_unique<If>(
        make_unique<Name>("x", expr_context::Load),
        move(body),
        move(orelse)
    );

    stmtPs body2;
    body2.push_back(move(tree));

    unique_ptr<Module> module = make_unique<Module>(move(body2));

    PrettyPrinter pprint;
    try {
        module->accept(pprint);
        printf("Tree:\n%s\n", pprint.ctx.s.c_str());
    } catch (runtime_error& e) {
        printf("error: %s\n", e.what());
    }
    string input = "while x:\n    -x-1 + 2**y**z and u\n";
    input = "while x:\n    a and b and c or c and not d or f\n";
    input = "while x:\n    a < b > c == d is not e in f\n";
    input = "while x:\n    True is None\n";
    input = "while x:\n    1 + 'a' + None + True\n";
    input = "while x:\n    await True\n";
    Parser parser;
    unique_ptr<stmt> t = parser.parseWhile(input);
    printf("while: %p\n", t.get());
    PrettyPrinter pprint0;
    t->accept(pprint0);
    return 0;
}