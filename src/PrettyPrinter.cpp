#include <cstdio>
#include "AST.h"
#include "PrettyPrinter.h"

//Add, Sub, Mult, MatMult, Div, Mod, Pow, LShift, RShift, BitOr, BitXor, BitAnd, FloorDiv 
string PrettyPrinter::operatorToString(operator_ op) {
    switch (op) {
        case operator_::Add: return "Add()";
        case operator_::Sub: return "Sub()";
        case operator_::Mult: return "Mult()";
        case operator_::MatMult: return "MatMult()";
        case operator_::Div: return "Div()";
        case operator_::Mod: return "Mod()";
        case operator_::Pow: return "Pow()";
        case operator_::LShift: return "LShift()";
        case operator_::RShift: return "RShift()";
        case operator_::BitOr: return "BitOr()";
        case operator_::BitXor: return "BitXor()";
        case operator_::BitAnd: return "BitAnd()";
        case operator_::FloorDiv: return "FloorDiv()";
        default: break;
    }
    return "InvalidOperator";
}

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
    s += indent() + "test=" + ctx.s + ",\n";
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
    string s = indent() + "Expr(\n";
    ctx.level++;
    s += indent() + "value=";
    node.value->accept(*this);
    s += ctx.s;
    s += indent() + ")";
    ctx.level--;
    ctx.s = s;
}

void PrettyPrinter::visit(BinOp& node) {
    string s = "BinOp(\n";
    int level = ctx.level;
    ctx.level = 0;
    node.left->accept(*this);
    ctx.level = level;
    ctx.level++;
    s += indent() + "left=" + ctx.s + ",\n";
    s += indent() + "op=" + operatorToString(node.op) + ",\n";
    node.right->accept(*this);
    s += indent() + "right=" + ctx.s + "\n";
    s += indent() + ")";
    ctx.level--;
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
    string context;
    if (node.ctx == expr_context::Load) {
        context = "Load()";
    } else if (node.ctx == expr_context::Store) {
        context = "Store()";
    } else {
        context = "Del()";
    }
    ctx.s = "Name(id='" + node.id+ "', ctx=" + context + ")";
}
