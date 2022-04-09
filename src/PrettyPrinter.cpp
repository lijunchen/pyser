#include <cstdio>
#include "AST.h"
#include "PrettyPrinter.h"

string PrettyPrinter::contextToString(expr_context ctx) {
    switch (ctx) {
        case expr_context::Load:
            return "Load()";
        case expr_context::Store:
            return "Store()";
        case expr_context::Del:
            return "Del()";
        default:
            break;
    }
    return "InvalidContext";
}

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

string PrettyPrinter::boolopToString(boolop op) {
    switch (op) {
        case boolop::And: return "And()";
        case boolop::Or: return "Or()";
        default: break;
    }
}

string PrettyPrinter::cmpopToString(cmpop op) {
    switch (op) {
        case cmpop::Eq: return "Eq()";
        case cmpop::NotEq: return "NotEq()";
        case cmpop::Lt: return "Lt()";
        case cmpop::LtE: return "LtE()";
        case cmpop::Gt: return "Gt()";
        case cmpop::GtE: return "GtE()";
        case cmpop::Is: return "Is()";
        case cmpop::IsNot: return "IsNot()";
        case cmpop::In: return "In()";
        case cmpop::NotIn: return "NotIn()";
        default: break;
    }
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

void PrettyPrinter::visit(BoolOp& node) {
    string s = "BoolOp(\n";
    {
        ctx.level++;
        s += indent() + "op=" + boolopToString(node.op) + ",\n";
        s += indent() + "values=[\n";
        {
            ctx.level++;
            for (size_t i = 0; i < node.values.size(); i++) {
                node.values[i]->accept(*this);
                s += indent() + ctx.s + ",\n";
            }
            ctx.level--;
        }
        s += indent() + "]\n";
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

void PrettyPrinter::visit(Compare& node) {
    string s = "Compare(\n";
    {
        ctx.level++;
        node.left->accept(*this);
        s += indent() + "left=" + ctx.s + ",\n";
        s += indent() + "ops=[\n";
        {
            ctx.level++;
            for (size_t i = 0; i < node.ops.size(); i++) {
                s += indent() + cmpopToString(node.ops[i]) + ",\n";
            }
            ctx.level--;
        }
        s += indent() + "],\n";
        s += indent() + "comparators=[\n";
        {
            ctx.level++;
            for (size_t i = 0; i < node.comparators.size(); i++) {
                node.comparators[i]->accept(*this);
                s += indent() + ctx.s + ",\n";
            }
            ctx.level--;
        }
        s += indent() + "],\n";
        ctx.level--;
    }
    s += indent() + ")";
    ctx.s = s;
}

void PrettyPrinter::visit(Str& node) {
    ctx.s = "Constant(value=";
    ctx.s += node.value + ", ";
    ctx.s += "kind=";
    if (node.kind) {
        ctx.s += *node.kind;
    } else {
        ctx.s += "None";
    }
    ctx.s += ")";
}

void PrettyPrinter::visit(Num& node) {
    ctx.s = "Constant(value=" + node.value + ", kind=None)";
}

void PrettyPrinter::visit(Bool& node) {
    ctx.s = "Constant(value=" + node.value + ", kind=None)";
}

void PrettyPrinter::visit(None& node) {
    ctx.s = "Constant(value=None, kind=None)";
}

void PrettyPrinter::visit(Name& node) {
    ctx.s = "Name(id='" + node.id+ "', ctx=" + contextToString(node.ctx) + ")";
}

void PrettyPrinter::visit(Await& node) {
    string s = "Await(\n";
    {
        ctx.level++;
        node.value->accept(*this);
        s += indent() + "value=" + ctx.s + "\n";
        ctx.level--;
    }
    s += indent() + ")";
    ctx.s = s;
}

void PrettyPrinter::visit(Attribute& node) {
    string s = "Attribute(\n";
    {
        ctx.level++;
        node.value->accept(*this);
        s += indent() + "value=" + ctx.s + ",\n";
        s += indent() + "attr='" + node.attr + "',\n";
        s += indent() + "ctx=" + contextToString(node.ctx) + "\n";
        ctx.level--;
    }
    s += indent() + ")";
    ctx.s = s;
}

void PrettyPrinter::visit(Subscript& node) {
    string s = "Subscript(\n";
    {
        ctx.level++;
        node.value->accept(*this);
        s += indent() + "value=" + ctx.s + ",\n";
        node.slice->accept(*this);
        s += indent() + "slice=" + ctx.s + "\n";
        s += indent() + "ctx=" + contextToString(node.ctx) + "\n";
        ctx.level--;
    }
    s += indent() + ")";
    ctx.s = s;
}

void PrettyPrinter::visit(Call& node) {
    string s = "Call(\n";
    {
        ctx.level++;
        ctx.level--;
    }
    s += indent() + ")";
    ctx.s = s;
}

void PrettyPrinter::visit(keyword& node) {
    string s = "keyword(\n";
    {
        ctx.level++;
        ctx.level--;
    }
    s += indent() + ")";
    ctx.s = s;
}

void PrettyPrinter::visit(List& node) {
    string s = "List(\n";
    {
        ctx.level++;
        s += indent() + "elts=[\n";
        {
            ctx.level++;
            for (size_t i = 0; i < node.elts.size(); i++) {
                node.elts[i]->accept(*this);
                s += indent() + ctx.s + ",\n";
            }
            ctx.level--;
        }
        s += indent() + "]\n";
        s += indent() + "ctx=" + contextToString(node.ctx) + "\n";
        ctx.level--;
    }
    s += indent() + ")";
    ctx.s = s;
}

void PrettyPrinter::visit(Tuple& node) {
    string s = "Tuple(\n";
    {
        ctx.level++;
        s += indent() + "elts=[\n";
        {
            ctx.level++;
            for (size_t i = 0; i < node.elts.size(); i++) {
                node.elts[i]->accept(*this);
                s += indent() + ctx.s + ",\n";
            }
            ctx.level--;
        }
        s += indent() + "]\n";
        s += indent() + "ctx=" + contextToString(node.ctx) + "\n";
        ctx.level--;
    }
    s += indent() + ")";
    ctx.s = s;
}

void PrettyPrinter::visit(Slice& node) {
    string s = "Slice(\n";
    {
        ctx.level++;
        string lo = "None";
        if (node.lower) { node.lower->accept(*this); lo = ctx.s; }
        s += indent() + "lower=" + lo + ",\n";
        string hi = "None";
        if (node.upper) { node.upper->accept(*this); hi = ctx.s; }
        s += indent() + "upper=" + hi + ",\n";
        string st = "None";
        if (node.step) { node.step->accept(*this); st = ctx.s;}
        s += indent() + "step=" + st + "\n";
        ctx.level--;
    }
    s += indent() + ")";
    ctx.s = s;
}

void PrettyPrinter::visit(FunctionDef& node) {}
void PrettyPrinter::visit(ClassDef& node) {}
void PrettyPrinter::visit(Return& node) {}
void PrettyPrinter::visit(Delete& node) {}
void PrettyPrinter::visit(Assign& node) {}
void PrettyPrinter::visit(AugAssign& node) {}
void PrettyPrinter::visit(AnnAssign& node) {}
void PrettyPrinter::visit(For& node) {}
void PrettyPrinter::visit(With& node) {}
void PrettyPrinter::visit(Raise& node) {}
void PrettyPrinter::visit(Try& node) {}
void PrettyPrinter::visit(Assert& node) {}
void PrettyPrinter::visit(Import& node) {}
void PrettyPrinter::visit(ImportFrom& node) {}
void PrettyPrinter::visit(Global& node) {}
void PrettyPrinter::visit(Nonlocal& node) {}
void PrettyPrinter::visit(Pass& node) {}
void PrettyPrinter::visit(Break& node) {}
void PrettyPrinter::visit(Continue& node) {}
void PrettyPrinter::visit(Lambda& node) {}
void PrettyPrinter::visit(IfExp& node) {}
void PrettyPrinter::visit(Dict& node) {}
void PrettyPrinter::visit(Set& node) {}
void PrettyPrinter::visit(Yield& node) {}
void PrettyPrinter::visit(YieldFrom& node) {}
void PrettyPrinter::visit(Starred& node) {}
void PrettyPrinter::visit(arguments& node) {}
void PrettyPrinter::visit(arg& node) {}