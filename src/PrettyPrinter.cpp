#include <cstdio>
#include "AST.h"
#include "PrettyPrinter.h"

void PrettyPrinter::visit(Module& node) {
    string s = "Module(\n";
    ctx.level++;
    s += indent() + "body=[\n";
    ctx.level++;
    for (size_t i = 0; i < node.body.size(); i++) {
        node.body[i]->accept(*this);
        s += ctx.s;
        if (i != node.body.size() - 1) {
            s += ",\n";
        }
    }
    ctx.level--;
    s += indent() + "]\n";
    s += indent() + ")";
    ctx.level--;
    ctx.s = s;
}

void PrettyPrinter::visit(While& node) {

}

void PrettyPrinter::visit(If& node) {
    string s = indent() + "If(\n";
    ctx.level++;
    printf("indent level: %d\n", ctx.level);
    node.test->accept(*this);
    s += indent() + "test=" + ctx.s + "\n";
    s += indent() + "body=[\n";
    ctx.level++;
    for (size_t i = 0; i < node.body.size(); i++) {
        node.body[i]->accept(*this);
        s += ctx.s;
        if (i != node.body.size() - 1) {
            s += ",\n";
        }
    }
    ctx.level--;
    ctx.level--;
    ctx.s = s;
}

void PrettyPrinter::visit(Expr& node) {
    string s = indent() + "Expr(value=\n";
    ctx.level++;
    node.value->accept(*this);
    s += ctx.s;
    ctx.level--;
    s += indent() + ")";
    ctx.s = s;
}

void PrettyPrinter::visit(BinOp& node) {
    string s = indent() + "BinOp(\n";
    int level = ctx.level;
    ctx.level = 0;
    node.left->accept(*this);
    ctx.level = level;
    s += indent() + "left=" + ctx.s + ",\n";
    s += indent() + "operator=" + std::to_string((int)node.op) + ",\n";
    node.right->accept(*this);
    s += indent() + "right=" + ctx.s + "\n";
    s += indent() + ")";
    ctx.s = s;
}

void PrettyPrinter::visit(Str& node) {
    ctx.s = "Str(value='";
    ctx.s += node.value + "', ";
    ctx.s += "kind=";
    if (node.kind) {
        ctx.s += *node.kind;
    } else {
        ctx.s += "None";
    }
    ctx.s += ")";
}

void PrettyPrinter::visit(Num& node) {
    ctx.s = "Num(value=" + node.value + ")";
}

void PrettyPrinter::visit(Name& node) {
    ctx.s = "Name(id='" + node.id + "')";
}
