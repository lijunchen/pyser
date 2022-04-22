#include "AST.h"
#include "Parser.h"
#include "PrettyPrinter.h"
#include <cmath>
#include <cstdio>
#include <iostream>
#include <memory>
#include <string>

using namespace std;

int main() {
    stmtPs body;
    stmtP s0 = make_unique<Expr>(make_unique<BinOp>(
        make_unique<BinOp>(make_unique<Name>("x", expr_context::Load),
                           operator_::Mult,
                           make_unique<Name>("y", expr_context::Load)),
        operator_::Add,
        make_unique<BinOp>(make_unique<Num>("123"), operator_::Mult,
                           make_unique<Name>("y", expr_context::Load))));
    body.push_back(move(s0));
    stmtPs orelse;

    stmtP tree = make_unique<If>(make_unique<Name>("x", expr_context::Load),
                                 move(body), move(orelse));

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
    input = "while x:\n    a.b.c\n";
    input = "while x:\n    a[a, a:b, a:b:c]\n";
    input = "1,2,3\n";
    input = "1 if 2 else a, b, c\n";
    input = "*a is b\n";
    input = "a, b, c\n";
    input = "yield from a\n";
    input = "a: int = 1\n";
    input = "*a | b\n";
    input = "a[b].c: int = 1\n";
    input = "a = b = c = d + e\na += b * d\n";
    // input = "a = b\n";
    Parser parser;
    unique_ptr<ast> t = nullptr;
    try {
        t = parser.parse(input);
    } catch (runtime_error& e) {
        printf("error: %s\n", e.what());
    }
    printf("Module: %p\n", t.get());
    PrettyPrinter pprint0;
    if (t) {
        t->accept(pprint0);
    }
    return 0;
}