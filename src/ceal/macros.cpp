// CryptoLeq Enhanced Assembler
// New York University at Abu Dhabi, MoMa Lab
// Copyright 2014-2015
// Author: Oleg Mazonka
// File: macros.cpp

#include "utils.h"
#include "tokzer.h"
#include "getptr.h"
#include "errex.h"

#include "macros.h"

int Macros::unique_counter = 0;


void Macros::build_id_table(Pnode n)
{
    Idn * id = get<Idn>(NOTHR, n);
    if (id)
    {
        // if the parent is function, then ignore the name
        bool ignore = false;
        {
            Term * par = dynamic_cast<Term *>(id->parent);
            if (par && par->typ == Term::eFunc)
                ignore = true;
        }

        if (!ignore)
        {
            macdef_instance->undefs.insert(id->s);
            macdef_instance->iduse[id->s].push_back(n);
        }
    }
    else
        for (auto i : n->children)
            build_id_table(i);
}

void Macros::build_id_table(Idn & d, Pnode n)
{
    if (macdef_instance->idxGlobal(d.s) < 0)
        macdef_instance->locdefs[d.s] = n;
    else
        macdef_instance->entries.insert(d.s);
}

void Macros::build_id_table(Labels & l)
{
    for (auto i : l.children)
    {
        Idn * id = get<Idn>(NOTHR, i);
        if (!id) // address definition in macro - suspicious - maybe warning is necessary
            continue;

        build_id_table(*id, i);
    }
}

void Macros::build_id_table(Instruction & i)
{
    for (auto pli : i.children)
    {
        Litem * li = get<Litem>(LNFUN, pli);
        if (li->children.size() != 2) throw Err(LNFUN);

        if ( li->typ == Litem::eDef)
        {
            Pnode nid = li->children[1];
            Idn * id = get<Idn>(LNFUN, nid);
            build_id_table(*id, nid);
        }
        else
        {
            Labels * labs = get<Labels>(LNFUN, li->children[1]);
            build_id_table(*labs);

            Item * itm = get<Item>(LNFUN, li->children[0]);
            if (itm->typ == Item::eId)
            {
                Pnode nid = itm->children[0];
                Idn * id = get<Idn>(LNFUN, nid);
                build_id_table(*id, nid);
            }
        }

        build_id_table(li->children[0]); // process expressions
    }
}

void Macros::build_id_table(Macuse & u)
{
    if (u.children.empty()) throw Err(LNFUN);

    Labels * labs = get<Labels>(LNFUN, u.children[0]);
    build_id_table(*labs);

    auto sz = u.children.size();

    for (decltype(sz) i = 1; i < sz; i++)
        build_id_table(u.children[i]);
}

void Macros::instantiate(Macdef & md, Macuse & mu)
{
    // first setup argvals
    md.argvals = mu.children;
    md.prelabs = md.argvals.front();
    md.argvals.erase(md.argvals.begin()); // remove prelabs

    if (md.argnams.size() < md.argvals.size())
        throw Err("Too many parameters used", mu.tok(), md.tok());

    if (md.argnams.size() > md.argvals.size())
        throw Err("Too few parameters used", mu.tok(), md.tok());

    // enter recursive processing
    macdef_instance = &md;

    for (auto i : md.children)
    {
        if (Instruction * pi = get<Instruction>(NOTHR, i))
            build_id_table(*pi);
        else if (Macuse * pm = get<Macuse>(NOTHR, i))
            build_id_table(*pm);
        else
            throw Err(LNFUN);
    }

    // exit recursive processing

    // at this moment locdefs and undefs are populated
    // all undefs can be either
    // 1 args
    // 2 globals
    // 3 locdefs

    // check that local defines are used
    const bool STRICT_LOCAL_DEFS = true;
    if (STRICT_LOCAL_DEFS)
        for (auto i : md.locdefs)
            if (md.undefs.find(i.first) == md.undefs.end())
                throw Err("Local defined but unused", i.second->tok());

    // check that global defines are used or are actually defined in macro
    const bool STRICT_GLOBAL_DEFS = true;
    if (STRICT_GLOBAL_DEFS)
        for (auto i : md.globals)
        {
            string name = get<Idn>(LNFUN, i)->s;
            if (md.undefs.find(name) == md.undefs.end()
                    && md.entries.find(name) == md.entries.end())
                throw Err("Global unused", i->tok());
        }

    // now process all undefs
    for (auto name : md.undefs)
    {
        if (md.idxGlobal(name) >= 0)
            continue;

        int idx = md.idxArgs(name);
        if (idx < 0)
            instance_process_locs(name);
        else
            instance_process_args(name, idx);
    }

    macdef_instance = nullptr;

    // process prelabs
    if (md.prelabs->children.empty())
        return; // nothing else to do

    if (md.children.empty())
        throw Err("Label on empty macro", mu.tok(), md.tok());

    Pnode first = md.children[0];
    Macuse * imu = get<Macuse>(NOTHR, first);
    Pnode labs = first->children[0];
    if (imu) {}
    else if (get<Instruction>(LNFUN, first))
    {
        Litem * lit = get<Litem>(LNFUN, labs);
        labs = lit->children[1];
    }

    for (auto i : md.prelabs->children)
        labs->addChild(i);
}

Nodes Macros::process_macuse(Macuse & mu)
{
    // make clone, resolve names, extract instructions

    string macname = mu.name();

    if (mu.name() == "_table")
        return Confinement(root, mu).table();

    if (mu.name() == "_autobits")
    {
        Autobits ab(root, mu);
        process_autobits(mu, ab);
        macname = ab.name();
    }

    Pnode macdef = root->macdefs[macname];
    if (!macdef)
        throw Err("Macro [" + macname + "] is not defined", mu.tok());

    Pnode clone = macdef->clone();
    Macdef * pm = get<Macdef>(LNFUN, clone);
    instantiate(*pm, mu);

    return pm->children;
}

void Macros::instance_process_args(const string & name, int idx)
{
    Pnode pval = macdef_instance->argvals[idx];

    const Nodes & usage = macdef_instance->iduse[name];
    for (auto i : usage)
    {
        // i is id
        Node * parent = i->parent;
        //find in parent references to this id
        for (auto & j : parent->children)
        {
            if (j.get() != i.get())
                continue;

            // this is our reference - j is Pnode in the parent's list
            j = pval;
            j->parent = parent;

            if (Item * itm = dynamic_cast<Item *>(parent))
            {
                if (itm->typ != Item::eId) throw Err(LNFUN);
                throw Err("Parameter names in array definitions, use [(...)]", i->tok());
            }

            // now change parents type
            Term * term = dynamic_cast<Term *>(parent);
            if (!term) throw Err(LNFUN);
            // pval is either Idn or Expr
            Idn * idn = get<Idn>(NOTHR, pval);

            if (idn)
                continue; // all good term should be eId

            // otherwise pval is expr
            term->typ = Term::eExpr;
        }
    }
}

string Macros::unique_name()
{
    std::ostringstream os;
    os << ++unique_counter;
    return os.str();
}

void Macros::instance_process_locs(const string & name)
{
    // if not locdefs - error
    Macdef & m = *macdef_instance;
    auto i = m.locdefs.find(name);
    const Nodes & usage = m.iduse[name];

    if (usage.empty())
        throw Err(LNFUN);

    if (i == m.locdefs.end())
        throw Err("Undeclared '" + name + "' in macro definition", m.tok(), usage[0]->tok());

    string newname = name + "@" + unique_name();

    // change name in def
    Idn * idn = get<Idn>(LNFUN, i->second);
    idn->s = newname;

    for (auto j : usage)
    {
        Idn * idn = get<Idn>(LNFUN, j);
        idn->s = newname;
    }
}

string Autobits::gen_name(Unumber n, string b0, string b1)
{
    return "_ab@" + n.str() + "@" + b0 + "@" + b1;
}

string Autobits::gen_name(Macuse & mu)
{
    Unumber v = val(mu);
    if (mu.children.size() != 4) throw Err(LNFUN);
    string bit0 = getid(mu.children[2]);
    string bit1 = getid(mu.children[3]);
    return gen_name(v, bit0, bit1);
}

Unumber Autobits::val(Macuse & mu)
{
    if (mu.children.size() != 4)
        throw Err("Macro _autobits require 3 arguments", mu.tok());

    Expr * ex = get<Expr>(LNFUN, mu.children[1]);
    Cell ts;
    try
    {
        ts = ex->val();
    }
    catch (Err e)
    {
        throw Err(e.str() + " - autobits expect constant expression");
    }

    const Unumber & N = root->comp->proc.N;
    if ( !N.iszero() )
        return ts.x();

    auto tsts = ts.ts();
    if (!tsts.s.iszero())
        throw Err("Macro _autobits: N is not set encrypted values", mu.children[0]->tok());

    return tsts.t;
}

string Autobits::getid(Pnode n)
{
    Expr * ex = get<Expr>(LNFUN, n);
    if (ex->children.size() != 1) goto bad;

    {
        n = ex->children[0];
        Term * tm = get<Term>(NOTHR, n);
        if (!tm) goto bad;

        {
            if (tm->children.size() != 1) goto bad;
            n = tm->children[0];
            Idn * id = get<Idn>(NOTHR, n);
            if (!id) goto bad;

            return id->s;
        }
    }

bad:
    throw Err("Expecting name of a macro", n->tok());
}

void Macros::process_autobits(Macuse & mu, Autobits & autobits)
{
    ///Autobits autobits(root);

    Unumber val = autobits.val(mu);
    string name = autobits.name();
    Pnode macdef = root->macdefs[name];

    if (!macdef)
    {
        // obtain bit macuse names
        if (mu.children.size() != 4) throw Err(LNFUN);
        string bit0 = autobits.getid(mu.children[2]);
        string bit1 = autobits.getid(mu.children[3]);

        Pnode md(new Macdef(mu.tok()));

        while (val != 0)
        {
            string bit = bit0;
            if (val.getbit(0)) bit = bit1;
            val >>= 1;

            Token tk(mu.tok());
            tk.s = bit;
            Macuse * u = new Macuse(tk, Pnode(new Labels(mu.tok())));
            md->addChild(Pnode(u));
        }

        root->macdefs[name] = md;
    }

    // remove arguments
    mu.children.pop_back();
    mu.children.pop_back();
    mu.children.pop_back();
}

Pnode Confinement::tsnum(Token tok, Cell c)
{
    auto xts = c.ts();
    Unumber t = xts.t;
    Unumber s = xts.s;
    Token tkt(tok, t);
    Token tks(tok, s);
    Pnode n_tkt(new Unum(tkt));
    Pnode n_tks(new Unum(tks));
    Pnode ts(new Tsnum(tkt, n_tkt, n_tks));
    return ts;
}

Pnode Confinement::item(Pnode ts)
{
    Token tok = ts->tok();

    Pnode cnst(new Cnst(tok, ts, Cnst::eTsnum));
    Pnode term(new Term(tok, cnst, Term::eCnst));

    Expr * r_expr = new Expr(tok);
    r_expr->typ = Expr::eTerm;
    r_expr->addChild(term);
    Pnode expr(r_expr);

    Item * r_item = new Item(tok);
    r_item->typ = Item::eExpr;
    r_item->addChild(expr);
    Pnode itm(r_item);

    return itm;
}


Nodes Confinement::table()
{
    Unumber N = root->comp->proc.N;
    Unumber N2 = root->comp->proc.N2;
    Token tok = mu.tok();

    if (N.iszero())
        throw Err("Macro _table requires N", tok);

    if (mu.children.size() != 7)
        throw Err("Macro _table requires 6 arguments ("
                  + bug::to_string(mu.children.size() - 1) + ")", tok);

    string type;
    {
        // sanity check
        get<Expr>(LNFUN, mu.children[1]);
        get<Term>(LNFUN, mu.children[1]->children[0]);

        Idn * id = get<Idn>(NOTHR, mu.children[1]->children[0]->children[0]);
        if (id)
            type = id->s;
    }

    Cell gr = get<Expr>(LNFUN, mu.children[2])->val();
    Cell p1 = get<Expr>(LNFUN, mu.children[3])->val();
    Cell c1 = get<Expr>(LNFUN, mu.children[4])->val();
    Cell p2 = get<Expr>(LNFUN, mu.children[5])->val();
    Cell c2 = get<Expr>(LNFUN, mu.children[6])->val();

    //if (!gr.ts().s.iszero()) throw Err("Argument must be open value: (" + gr.str() + ")", mu.children[2]->tok());
    if (!p1.ts().s.iszero()) throw Err("Argument must be open value: (" + p1.str() + ")", mu.children[3]->tok());
    if (!p2.ts().s.iszero()) throw Err("Argument must be open value: (" + p2.str() + ")", mu.children[5]->tok());
    Unumber grn = gr.x();
    Unumber p1n = p1.ts().t;
    Unumber p2n = p2.ts().t;

    Unumber gc = grn;
    std::vector<Cell> vm;

    int cntr = ORDMAX;
    while (--cntr > 0)
    {
        Unumber k = gc;
        k.pow(p1n, N2);
        k = k.mul(c1.x(), N2);

        Unumber v = gc;
        v.pow(p2n, N2);
        v = v.mul(c2.x(), N2);

        vm.push_back(Cell(k, Cell::X));
        vm.push_back(Cell(v, Cell::X));

        gc = gc.mul(grn, N2);

        if (gc == grn)
            break;
    }

    if (!cntr)
        throw Err("Table order is too high - increase constant ORDMAX ("
                  + bug::to_string(ORDMAX) + ")", tok);

    Instruction * pi = new Instruction(tok);

    pi->typ = Instruction::eData;

    if (type == "_map_")
        for (decltype(vm.size()) i = 0; i < vm.size(); i += 2)
        {
            Cell address = vm[i];
            Cell value = vm[i + 1];
            Pnode itm = item(tsnum(tok, value));

            Pnode labels(new Labels(tok));
            labels->addChild(tsnum(tok, address));

            Litem * r_litem = new Litem(tok, labels, itm, Litem::eItm);
            Pnode litem(r_litem);

            pi->addChild(litem);
        }
    else if (type == "_array_")
        for (auto i : vm)
        {
            Pnode itm = item(tsnum(tok, i));

            Pnode labels(new Labels(tok));
            Litem * r_litem = new Litem(tok, labels, itm, Litem::eItm);
            Pnode litem(r_litem);

            pi->addChild(litem);
        }
    else
        throw Err("Macro _table requires type: '_map_' or '_array_'", tok);

    Nodes r;
    r.push_back(Pnode(pi));
    return r;
}
