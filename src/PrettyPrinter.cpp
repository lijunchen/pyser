#include <cstdio>
#include "AST.h"
#include "PrettyPrinter.h"

// Add, Sub, Mult, MatMult, Div, Mod, Pow, LShift, RShift, BitOr, BitXor, BitAnd, FloorDiv 
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

string PrettyPrinter::unaryopToString(unaryop op) {
    switch (op) {
        case unaryop::Invert: return "Invert()";
        case unaryop::Not: return "Not()";
        case unaryop::UAdd: return "UAdd()";
        case unaryop::USub: return "USub()";
        default: break;
    }
    return "InvalidUnaryOperator";
}

void PrettyPrinter::visit(Module& node) {
    string s = "Module(\n";
    {
        ctx.level++;
        s += indent() + "body=[\n";
        {
            ctx.level++;
            for (size_t i = 0; i < node.body.size(); i++) {
                node.body[i]->accept(*this);
                s += ctx.s;
                s += ",\n";
            }
            ctx.level--;
        }
        s += indent() + "],\n";

        s += indent() + "type_ignores=[\n";
        {
            ctx.level++;
            ctx.level--;
        }
        s += indent() + "],\n";
        ctx.level--;
    }
    s += indent() + ")";
    ctx.s = s;
}

void PrettyPrinter::visit(While& node) {
    string s = indent() + "While(\n";
    {
        ctx.level++;
        node.test->accept(*this);
        s += indent() + "test=" + ctx.s + ",\n";
        s += indent() + "body=[\n";
        {
            ctx.level++;
            for (size_t i = 0; i < node.body.size(); i++) {
                node.body[i]->accept(*this);
                s += ctx.s;
                s += ",\n";
            }
            ctx.level--;
        }
        s += indent() + "]\n";

        s += indent() + "orelse=[\n";
        {
            ctx.level++;
            for (size_t i = 0; i < node.orelse.size(); i++) {
                node.orelse[i]->accept(*this);
                s += ctx.s;
                s += ",\n";
            }
            ctx.level--;
        }
        s += indent() + "],\n";
        ctx.level--;
    }
    s += indent() + ")";
    ctx.s = s;
    printf("ast:\n%s\n", s.c_str());
}

void PrettyPrinter::visit(If& node) {
    string s = indent() + "If(\n";
    {
        ctx.level++;
        node.test->accept(*this);
        s += indent() + "test=" + ctx.s + ",\n";
        s += indent() + "body=[\n";
        {
            ctx.level++;
            for (size_t i = 0; i < node.body.size(); i++) {
                node.body[i]->accept(*this);
                s += ctx.s;
                s += ",\n";
            }
            ctx.level--;
        }
        s += indent() + "]\n";

        s += indent() + "orelse=[\n";
        {
            ctx.level++;
            for (size_t i = 0; i < node.orelse.size(); i++) {
                node.orelse[i]->accept(*this);
                s += ctx.s;
                s += ",\n";
            }
            ctx.level--;
        }
        s += indent() + "],\n";
        ctx.level--;
    }
    s += indent() + ")";
    ctx.s = s;
}

void PrettyPrinter::visit(Expr& node) {
    string s = indent() + "Expr(\n";
    {
        ctx.level++;
        s += indent() + "value=";
        node.value->accept(*this);
        s += ctx.s + "\n";
        ctx.level--;
    }
    s += indent() + ")";
    ctx.s = s;
}

void PrettyPrinter::visit(BinOp& node) {
    string s = "BinOp(\n";
    {
        ctx.level++;
        node.left->accept(*this);
        s += indent() + "left=" + ctx.s + ",\n";
        s += indent() + "op=" + operatorToString(node.op) + ",\n";
        node.right->accept(*this);
        s += indent() + "right=" + ctx.s + "\n";
        ctx.level--;
    }
    s += indent() + ")";
    ctx.s = s;
}

void PrettyPrinter::visit(UnaryOp& node) {
    string s = "UnaryOp(\n";
    {
        ctx.level++;
        s += indent() + "op=" + unaryopToString(node.op) + ",\n";
        node.operand->accept(*this);
        s += indent() + "operand=" + ctx.s + "\n";
        ctx.level--;
    }
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
