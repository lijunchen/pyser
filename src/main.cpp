#include <cstdio>
#include <string>
#include <cmath>
#include <iostream>
#include "AST.h"
#include "PrettyPrinter.h"

using namespace std;

int main() {
    stmtPs body;
    stmtP _x_plus_123_mult_y = make_unique<Expr>(
        make_unique<BinOp>(
            make_unique<Name>("x", expr_context::Load),
            operator_::Add,
            make_unique<BinOp>(
                make_unique<Num>("123"),
                operator_::Mult,
                make_unique<Str>("y", nullopt))
        )
    );
    body.push_back(move(_x_plus_123_mult_y));
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
    return 0;
}