// CryptoLeq Enhanced Assembler
// New York University at Abu Dhabi, MoMa Lab
// Copyright 2014-2015
// Author: Oleg Mazonka
// File: nodes.cpp

#include <sstream>

#include "nodes.h"
#include "tokzer.h"
#include "utils.h"
#include "getptr.h"
#include "errex.h"


string Token::str() const
{
    switch (typk)
    {
        case tEof: return "";
        case tEol: return "";
        case tIgn: return "ignore";
        case tNull: return "null";

        case tNum: return un.str();
        case tLbr: return "(";
        case tRbr: return ")";
        case tMin: return "-";
        case tQmk: return "?";
        case tPls: return "+";
        case tTld: return "~";
        case tSmc: return ";";
        case tLsq: return "[";
        case tRsq: return "]";
        case tDot: return ".";
        case tCma: return ",";
        case tLcu: return "{";
        case tRcu: return "}";
        case tEql: return "=";
        case tCol: return ":";
        case tDol: return "$";

        case tDid:
        case tIdt: return s;

        case tStr:
        case tChr: return s;
        case tMac: return "." + s;
    }

    throw Err("Internal error - Token::str (" + bug::to_string(typk) + ")", line, pos, 0);
}

void Node::addChild(Pnode n)
{
    children.push_back(n);
    if (n) n->parent = this;
    else
        throw Err("Internal error - Node::addChild", n->line, n->pos, 0);
}


Litem::Litem(Token tk, Pnode lab, Pnode itm, Typ tp): Node(tk), typ(tp)
{
    addChild(itm);
    addChild(lab);
}

string Cnst::print() const
{
    return string("const ") + "nc?"[typ]
           + (typ == eChr ? string() + " '" + print_char(s[0]) + "'" : "");
}

string Item::print() const
{
    string str;
    if ( typ == eStr )
        for ( decltype(s.size()) i = 0; i < s.size(); i++ )
            str += print_char(s[i]);

    return string("item ") + "#enis"[typ] + (tilda ? "~" : "")
           + (typ == eStr ? string() + " \"" + str + "\"" : "");
}


Root * Node::root() const
{
    const Node * n = this;
    while ( n->parent != nullptr ) n = n->parent;
    const Root * r = dynamic_cast<const Root *>(n);
    if (r) return const_cast<Root *>(r);
    throw Err("Internal error - root");
}

const Litem * Node::litem() const
{
    const Node * n = this;
    while ( n )
    {
        const Litem * r = dynamic_cast<const Litem *>(n);
        if (r) return r;
        n = n->parent;
    }

    throw Err("Internal error - litem");
}

string Litem::print() const
{
    string sadd = root()->comp->proc.str(address);
    string sval = root()->comp->proc.str(value);
    return string("l-item ") + "#=i"[typ] + " [" + sadd + "," + sval + "]";
}


string Node::dumpr(std::vector<int> & ind, bool middle) const
{
    std::ostringstream os;
    os << "(" << line << "," << pos << ") ";

    string r;
    for ( size_t i = 0; i < ind.size(); i++ )
    {
        r += ind[i] ? '|' : ' ';
        r += "  ";
    }

    int siblings = 0;
    if ( parent ) siblings = parent->children.size();
    string ssibl = bug::to_string(siblings);

    r += os.str() + print() + " (" + ssibl + ")" + '\n';

    size_t sz = children.size();
    for ( size_t i = 0; i < sz; ++i )
    {
        ind.push_back(middle);
        r += children[i]->dumpr(ind, i + 1 != sz);
        ind.pop_back();
    }

    return r;
}


string Root::dump_names()
{
    string r;
    for ( auto i : names_pending )
    {
        r += i->dump();
    }

    for (auto i : names_defined)
    {
        r += i.first + "\t=\t" + comp->proc.str(i.second) + '\n';
    }

    for (auto i : names_arraysz)
    {
        r += i.first + "\tline\t" + bug::to_string(i.second->tok().line) + '\n';
    }

    if ( !macdefs.empty() )
        r += "Macro definitions\n";

    for ( auto i : macdefs )
    {
        const Macdef * pm = get<Macdef>(LNFUN, i.second);
        const Macdef & m = *pm;

        r += ".def " + m.name;

        for (auto j : m.argnams)
            r += ' ' + get<Idn>(LNFUN, j)->s;

        if (!m.globals.empty())
            r += ":";

        for (auto j : m.globals)
            r += ' ' + get<Idn>(LNFUN, j)->s;

        r += '\n';
        r += i.second->dump();
        r += ".end\n";
    }

    return r;
}

Cell Tsnum::val() const
{
    if (children.empty())
        return Cell();

    Unumber t = get<Unum>(LNFUN, children[0])->un;

    if (children.size() == 1)
        return Cell(t, 0);

    Unumber s = get<Unum>(LNFUN, children[1])->un;

    return Cell(t, s);
}

bool Root::getvalue(const string & s, Cell & x, Token tk)
{
    auto i = names_defined.find(s);

    if (i != names_defined.end())
    {
        x = i->second;
        return true;
    }

    // now look in expression definition
    Nodes::const_iterator ili = find_pending(s);

    if (ili == names_pending.end())
        return false;

    Expr * ex = get<Expr>(LNFUN, (*ili)->children[0]);
    x = ex->val();

    // workaround for non-C14 compiler
    names_pending.erase(*reinterpret_cast<Nodes::iterator *>(&ili));
    //names_pending.erase(ili);

    setvalue(s, x);
    return true;
}

void Root::resolve_pending()
{
    for (auto i : names_pending )
    {
        Expr * ex = get<Expr>(LNFUN, i->children[0]);
        Idn * id = get<Idn>(LNFUN, i->children[1]);
        setvalue(id->s, ex->val());
    }

    names_pending.clear();
}

Nodes::const_iterator Root::find_pending(const string & s) const
{
    for (auto i = names_pending.begin(); i != names_pending.end(); ++i )
    {
        Idn * id = get<Idn>(LNFUN, (*i)->children[1]);
        if (id->s == s)
            return i;
    }
    return names_pending.end();
}

Cell Expr::val() const
{
    switch (typ)
    {
        case eTerm: return get<Term>(LNFUN, children[0])->val();
        case ePlus:
        case eMinus:
        {
            Cell x = get<HasTsValue>(LNFUN, children[0])->val();
            Cell y = get<Term>(LNFUN, children[1])->val();
            Cell z = y;

            if (typ == ePlus)
                z = root()->comp->proc.BAm1(y, Cell(0, 0));

            return root()->comp->proc.BAm1(z, x);
        }
    }
    throw Err(LNFUN);
}

Cell Term::val() const
{
    switch (typ)
    {
        case eCnst: return get<Cnst>(LNFUN, children[0])->val();
        case eId:
        {
            Idn * id = get<Idn>(LNFUN, children[0]);
            Cell x;
            bool k = root()->getvalue(id->s, x, tok());
            if ( !k )
                throw Err("Cannot resolve '" + id->s + "'", tok());
            return x;
        }
        case eExpr: return get<Expr>(LNFUN, children[0])->val();
        case eMinus:
        {
            Cell x = get<Term>(LNFUN, children[0])->val();
            return root()->comp->proc.BAm1(x, Cell(0, 0));
        }
        case eFunc: return func_call();
        case eNull:;
    }
    throw Err(LNFUN);
}

Cell Cnst::val() const
{
    switch (typ)
    {
        case eTsnum: return get<Tsnum>(LNFUN, children[0])->val();
        case eChr: return Cell(int(s[0]), 0);
        case eQmk:
        {
            Cell x(litem()->address);
            ++x;
            return x;
        }
    }
    throw Err(LNFUN);
}

void Root::resolve_arraysz()
{
    for ( auto i : names_arraysz )
    {
        string nm = i.first;
        Pnode p = i.second;

        // assert that p is Instruction
        get<Instruction>(LNFUN, p);

        auto sz = p->children.size();

        if ( sz < 1 )
            throw Err("Instruction zero size used", p->tok());

        names_defined[nm] = Cell(Unumber(sz), Cell::TS);
    }

    names_arraysz.clear();
}

Cell Term::func_unit() const
{
    if (children.size() != 2)
        throw Err("Function 'unit' requires one argument", tok());

    Expr * ex = get<Expr>(LNFUN, children[1]);
    Cell ts = ex->val();
    Unumber s1 = ts.ts().s + 1;
    Unumber x;

    bool ik = ma::invert(s1, root()->comp->proc.N, &x);
    if (!ik) throw Err("Cannot invert " + s1.str(), tok());

    return Cell(x, 0);
}

Cell Term::func_enc() const
{
    if (children.size() != 2)
        throw Err("Function 'enc' requires one argument", tok());

    Expr * ex = get<Expr>(LNFUN, children[1]);
    Cell ts = ex->val();

    if (!ts.ts().s.iszero())
        throw Err("Function 'enc' requires unencrypted argument", tok());

    Unumber t = ts.ts().t;
    return root()->comp->encrypt(t);
}

Cell Term::func_X() const
{
    if (children.size() != 2)
        throw Err("Function 'X' requires one argument", tok());

    Expr * ex = get<Expr>(LNFUN, children[1]);
    Cell ts = ex->val();
    return Cell(ts.ts().t, Cell::X);
}

Cell Term::func_T() const
{
    if (children.size() != 2)
        throw Err("Function 'T' requires one argument", tok());

    Expr * ex = get<Expr>(LNFUN, children[1]);
    Cell ts = ex->val();
    return Cell(ts.ts().t, 0);
}

Cell Term::func_S() const
{
    if (children.size() != 2)
        throw Err("Function 'S' requires one argument", tok());

    Expr * ex = get<Expr>(LNFUN, children[1]);
    Cell ts = ex->val();
    return Cell(ts.ts().s, 0);
}

Cell Term::func_Ts() const
{
    if (children.size() != 3)
        throw Err("Function 'TS' requires two arguments", tok());

    Expr * ex1 = get<Expr>(LNFUN, children[1]);
    Cell ts1 = ex1->val();
    Expr * ex2 = get<Expr>(LNFUN, children[2]);
    Cell ts2 = ex2->val();

    if ( !ts1.ts().s.iszero() || !ts2.ts().s.iszero() )
        throw Err("Arguments of 'TS' should evaluate to "
                  "open values: (" + ts1.str() + "," + ts2.str() + ")", tok());

    return Cell(ts1.ts().t, ts2.ts().t);
}

Cell Term::func_invN() const
{
    if (children.size() != 2)
        throw Err("Function 'invN' requires one argument", tok());

    Expr * ex = get<Expr>(LNFUN, children[1]);
    Cell ts = ex->val();
    Unumber s1 = ts.ts().t;
    Unumber x;

    if ( !ts.ts().s.iszero() )
        throw Err("Argument of 'invN' should evaluate to "
                  "open value: (" + ts.str() + ")", tok());

    bool ik = ma::invert(s1, root()->comp->proc.N, &x);
    if (!ik) throw Err("Cannot invert " + s1.str(), tok());

    return Cell(x, 0);
}

Cell Term::func_powN() const
{
    if (children.size() != 3)
        throw Err("Function 'powN' requires two arguments", tok());

    Expr * ex1 = get<Expr>(LNFUN, children[1]);
    Cell ts1 = ex1->val();
    Expr * ex2 = get<Expr>(LNFUN, children[2]);
    Cell ts2 = ex2->val();

    if ( !ts1.ts().s.iszero() || !ts2.ts().s.iszero() )
        throw Err("Arguments of 'powN' should evaluate to "
                  "open values: (" + ts1.str() + "," + ts2.str() + ")", tok());

    Unumber r = ts1.ts().t;
    Unumber n = ts2.ts().t;
    r.pow(n, root()->comp->proc.N);

    return Cell(r, 0);
}

Cell Term::testB2beta(Cell t, Token k)
{
    auto ts = t.ts();
    if (ts.s.iszero() && ts.t.iszero())
        throw Err("Using undefined B2 or beta, set beta or N", k);

    return t;
}

Unumber Term::testFkf(Unumber u, Token k)
{
    if (u.iszero())
        throw Err("Function fkf requires crypto key, set PQ", k);

    return u;
}

Unumber Term::testRnd(Unumber u, Token k, Unumber N)
{
    if (u.iszero() && N.iszero() )
        throw Err("Function requires N!=0", k);

    return u;
}

Cell Term::func_call() const
{
    Idn * id = get<Idn>(LNFUN, children[0]);

    if (0) {}

    else if (id->s == "unit") return func_unit();
    else if (id->s == "X") return func_X();
    else if (id->s == "TS") return func_Ts();
    else if (id->s == "T") return func_T();
    else if (id->s == "S") return func_S();
    else if (id->s == "invN") return func_invN();
    else if (id->s == "powN") return func_powN();
    else if (id->s == "peekrnd") return func_comp(1, id->tok());
    else if (id->s == "random") return func_comp(2, id->tok());
    else if (id->s == "B2") return func_comp(3, id->tok());
    else if (id->s == "beta") return func_comp(4, id->tok());
    else if (id->s == "fkf") return func_comp(5, id->tok());
    else if (id->s == "enc") return func_enc();
    else if (id->s == "halfN") return func_comp(6, id->tok());
    else if (id->s == "phi") return func_comp(7, id->tok());
    else if (id->s == "k") return func_comp(8, id->tok());

    throw Err("Function unknown: '" + id->s + "'", tok());
}

Cell Term::func_comp(int x, Token k) const
{
    if (children.size() != 1)
        throw Err("Function needs no arguments", tok());

    Compiler & comp = *root()->comp;

    Unumber r;

    switch (x)
    {
        case 1: r = testRnd(comp.peek_rndN(), k, root()->comp->proc.N); break;
        case 2: r = testRnd(comp.random(), k, root()->comp->proc.N); break;
        case 3: return testB2beta(Cell(comp.proc.B2, 0), k);
        case 4: return testB2beta(Cell(comp.proc.beta, 0), k);
        case 5: return Cell(testFkf(comp.fkf(), k), Cell::X);
        case 6: return Cell(comp.proc.N / 2, Cell::X);
        case 7: return Cell(comp.access_phi(), 0);
        case 8: return Cell(comp.access_g(), Cell::X);
        default:
            throw Err(LNFUN);
    }

    return Cell(r, Cell::X);
}

string Macdef::print() const
{
    string r = "macdef " + name;

    for (auto i : argnams)
        r += " " + get<Idn>(LNFUN, i)->s;

    if (globals.empty())
        return r;

    r += " :";
    for (auto i : globals)
        r += " " + get<Idn>(LNFUN, i)->s;

    return r;
}

void Root::addMacdef(Pnode i)
{
    Macdef * p = get<Macdef>(LNFUN, i); // assert

    auto f = macdefs.find(p->name);

    if (f != macdefs.end())
        throw Err("Macro redefinition", p->tok(), f->second->tok());

    macdefs[p->name] = i;

    p->parent = this;
}

int Macdef::idx(const string & n, const Nodes & nodes)
{
    int cntr = 0;
    for ( auto i : nodes )
    {
        Idn * d = get<Idn>(LNFUN, i);
        if (d->s == n)
            return cntr;
        cntr++;
    }
    return -1;
}

void Root::addarrsz(const string & name, Pnode inst)
{
    if ( names_arraysz.find(name) != names_arraysz.end() )
        throw Err("Name '" + name + "' has previously been used", inst->tok());

    names_arraysz[name] = inst;
}
