// CryptoLeq Enhanced Assembler
// New York University at Abu Dhabi, MoMa Lab
// Copyright 2014-2015
// Author: Oleg Mazonka
// File: evltor.cpp

#include <iostream>

#include "utils.h"
#include "tokzer.h"

#include "evltor.h"
#include "macros.h"
#include "getptr.h"
#include "errex.h"
#include "memory.h"

void Evaluator::expand_items()
{
    Nodes & instructions = root->children;

    for (auto i : instructions)
    {
        Nodes old = i->children;
        i->children.clear();

        for (auto l : old )
        {
            Litem * pl = get<Litem>(LNFUN, l);

            if (pl->typ != Litem::eItm)
            {
                i->addChild(l);
                continue;
            }

            if (pl->children.size() != 2)
                throw Err("Internal error - Litem Itm!=2", l.get()->tok());

            Item * itm = get<Item>(NOTHR, pl->children[0]);

            if ( itm->typ == Item::eId )
            {
                Idn * id = get<Idn>(LNFUN, itm->children[0]);
                root->addarrsz(id->s, i);
                continue;
            }

            if ( itm->typ != Item::eStr && itm->typ != Item::eNum )
            {
                i->addChild(l);
                continue;
            }

            string s;

            if (itm->typ == Item::eStr)
                s = itm->s;

            else if (itm->typ == Item::eNum )
            {
                int sz = 0;
                Unum * u = get<Unum>(NOTHR, itm->children[0]);

                if (u)
                    sz = int(u->un.to_ull());

                else
                {
                    Expr * e = get<Expr>(LNFUN, itm->children[0]);

                    try
                    {
                        Cell v = e->val();
                        auto vts = v.ts();
                        if (!vts.s.iszero()) throw Err("Expr in [] evaluates to encrypted value", itm->tok());
                        sz = int(vts.t.to_ull());
                    }
                    catch (Err e)
                    {
                        throw Err("Only number or const expr allowed in []\nMore info: " + e.str(), itm->tok());
                    }

                }

                if ( sz < 1 || sz > 10000 )
                    throw Err("Invalid size (" + bug::to_string(sz)
                              + "), must be between 1 to 10000", itm->tok());

                s = string(sz, '\0');
            }

            else
                throw Err(LNFUN);

            Pnode labels = pl->children[1];
            Pnode empty(new Labels(itm->tok()));
            for (size_t j = 0; j < s.size(); j++)
            {
                Plitem nl = make_litem(s[j], j == 0 ? labels : empty
                                       , itm->tok(), itm->tilda, false);

                i->addChild(nl);
            }
        } // next litem
    } // next instruction
}


Plitem Evaluator::make_litem(char c, Pnode labels, Token tok, bool tilda, bool qmk)
{
    Cnst * cn;
    if ( qmk )
        cn = new Cnst(tok, Cnst::eQmk);
    else
    {
        cn = new Cnst(tok, Cnst::eChr);
        cn->s = string() + c;
    }

    Term * tm = new Term(tok, Pnode(cn), Term::eCnst);

    Expr * ex = new Expr(tok);
    ex->typ = Expr::eTerm;
    ex->addChild(Pnode(tm));

    Item * it = new Item(tok);
    it->tilda = tilda;
    it->typ = Item::eExpr;
    it->addChild(Pnode(ex));

    Plitem r(new Litem(tok, labels, Pnode(it), Litem::eItm));

    return r;
}

void Evaluator::move_defs()
{
    Nodes & instructions = root->children;

    for (auto i : instructions)
    {
        Nodes old = i->children;
        i->children.clear();

        for (auto l : old )
        {
            Litem * pl = get<Litem>(LNFUN, l);

            if (pl->typ != Litem::eDef)
                i->addChild(l);
            else
            {
                // check redefinition
                Idn * id = get<Idn>(LNFUN, pl->children[1]);
                auto j = root->find_pending(id->s);
                if ( j != root->names_pending.end() )
                    throw Err("Redefinition", pl->tok());

                // all good
                l->parent = root;
                root->names_pending.push_back(l);
            }
        } // next litem
    } // next instruction
}


void Evaluator::fill3()
{
    Nodes instructions = root->children;
    root->children.clear();

    for (auto i : instructions)
    {
        Instruction * pi = get<Instruction>(LNFUN, i);

        if (pi->children.empty()) continue;
        root->addChild(i);
        if (pi->typ != Instruction::eThree || pi->children.size() == 3) continue;

        if (pi->children.size() > 3)
            throw Err("Instruction cannot have more than 3 operands", i.get()->tok());

        Pnode empty(new Labels(i.get()->tok()));
        if (pi->children.size() < 2)
        {
            Pnode kid = pi->children[0]->children[0];
            Pnode li(new Litem(i.get()->tok(), empty, kid, Litem::eItm));
            pi->addChild(li);

            // kid thinks its parent is the newly created node
            // lets reset it back
            kid->parent = pi->children[0].get();
        }

        if (pi->children.size() < 3)
        {
            Plitem nl = make_litem('\0', empty
                                   , i.get()->tok(), false, true);

            i->addChild(nl);
        }

    } // next instruction
}

void Evaluator::set_address()
{
    Cell addr(0, 0);

    Nodes & instructions = root->children;

    for (auto i : instructions)
    {
        Instruction * pi = get<Instruction>(LNFUN, i);

        bool curl = false;
        Cell save_addr;
        if (pi->typ == Instruction::eCurl)
            try
            {
                curl = true;
                save_addr = addr;
                addr = root->comp->proc.x2ts(root->comp->random());
            }
            catch (const char * e) { throw Err(e, pi->tok()); }

        Nodes & lis = i->children;

        for (auto l : lis )
        {
            Litem * pl = get<Litem>(LNFUN, l);

            try
            {
                assign_address(pl, addr);
                next_address(addr);
            }
            catch (string e) { throw Err(e, pl->tok()); }

            // finished with Litem - safe to remove labels
            pl->children.pop_back();

        } // next litem

        // finished instruction, resore addr if curl
        if (curl)
            addr = save_addr;

    } // next instruction
}

void Evaluator::next_address(Cell & addr)
{
    Unumber old_index = addr.ts().t;
    ++addr;
    Unumber new_index = addr.ts().t;

    if (new_index < old_index)
    {
        --addr;
        throw "Address " + addr.str() + " reaches N limit";
    }
}

void Evaluator::assign_address(Litem * pl, Cell & addr)
{
    Nodes labels = pl->children[1]->children;

    // check for repetitions
    for (auto lab : labels)
    {
        Tsnum * ts = get<Tsnum>(NOTHR, lab);

        if (ts)
            continue;

        Idn * id = get<Idn>(LNFUN, lab);
        auto j = root->allLabs.find(id->s);
        if (j == root->allLabs.end())
        {
            root->allLabs[id->s] = lab;
            continue;
        }

        throw Err("Label has been defined", lab->tok(), j->second->tok());
    }

    // plan:
    // go through all labels
    // if a label is in name table
    //  y: try to evaluate the value
    //      n: if fail - error
    //      y: if ok reset addr to this value
    //  n: place label in the table with addr
    // assign litem addr
    // remove labels


    // first loop define the address
    bool addr_lab_defined = false;
    for (auto lab : labels)
    {
        Tsnum * ts = get<Tsnum>(NOTHR, lab);

        if (ts)
        {
            if (addr_lab_defined)
                throw Err("Address has already been defined", ts->tok());

            addr_lab_defined = true;
            addr = ts->val();
            continue;
        }

        Idn * id = get<Idn>(LNFUN, lab);
        Cell new_addr;
        bool name_defined = root->getvalue(id->s, new_addr, id->tok());

        if (addr_lab_defined)
        {
            if (name_defined)
                throw Err("Address defined ambiguously", id->tok());

            continue;
        }

        if (!name_defined)
            continue;

        addr_lab_defined = true;
        addr = new_addr;
    }

    // now addr is either defined by a label or sequential
    pl->address = addr;

    // now define label names
    for (auto lab : labels)
    {
        Tsnum * ts = get<Tsnum>(NOTHR, lab);

        if (ts)
            continue;

        Idn * id = get<Idn>(LNFUN, lab);
        Cell idval;
        bool name_defined = root->getvalue(id->s, idval, id->tok());

        if (!name_defined)
        {
            root->setvalue(id->s, addr);
            continue;
        }

        if (!(idval == addr))
            throw Err("Internal error - address mismatch", id->tok());
    }

}

void Evaluator::resolve_names()
{
    // label names has already been resolved in address assignment
    {}

    // resolve array sizes
    root->resolve_arraysz();

    // resolve definitions
    root->resolve_pending();
}

void Evaluator::eval_expr()
{
    Nodes & instructions = root->children;

    for (auto i : instructions)
    {
        Nodes & lis = i->children;

        Instruction * instr = get<Instruction>(LNFUN, i); // need for tilda

        for (auto l : lis )
        {
            Litem * pl = get<Litem>(LNFUN, l);
            Item * pi = get<Item>(LNFUN, l->children[0]);

            if ( instr->tilda )
                pi->tilda = !pi->tilda;

            try
            {
                try
                {
                    if (pi->typ != Item::eExpr)
                        throw Err(LNFUN);

                    Expr * ex = get<Expr>(LNFUN, l->children[0]->children[0]);
                    pl->value = ex->val();

                    // test inversion
                    pl->value.invert();

                    if (pi->tilda)
                        pl->value = encrypt(pl->value);
                }
                catch (const char * e) { throw string(e); }
            }
            catch (string e) { throw Err(e, pi->tok()); }

            // finished with Litem - safe to remove item
            pl->children.pop_back();

        } // next litem
    } // next instruction
}

Cell Evaluator::encrypt(Cell x)
{
    if ( !root->comp->isrnd() )
        throw "Random seed has not been defined; "
        "either define it in pragma or use command line option";

    auto xts = x.ts();
    if ( !xts.s.iszero() )
        throw "Value " + root->comp->proc.str(x) + " is already encrypted";

    return root->comp->encrypt(xts.t);
}

void Evaluator::overlap_check(bool throwOnErr)
{
    Memory m;

    Nodes & instructions = root->children;

    for (auto i : instructions)
    {
        Nodes & lis = i->children;

        for (auto l : lis )
        {
            Litem * pl = get<Litem>(LNFUN, l);

            try
            {
                m.put(pl->address, pl->value);
            }
            catch (string e)
            {
                Err err("Memory overlap: " + pl->address.str(), pl->tok());
                if (throwOnErr)
                    throw err;
                else
                    std::cout << err.str() << '\n';
            }
            catch (...)
            {
                throw Err(LNFUN);
            }

        } // next litem
    } // next instruction
}

void Evaluator::subst_macros()
{
    int cntr = 0;
    Token gtok(0, 0);
    while (1)
    {
        if (++cntr > MAX_SUBST)
            throw Err("Too many macro substitutions: " + bug::to_string(MAX_SUBST) + ", possible recursion", gtok);

        bool weresubs = false;

        Nodes old = root->children;
        root->children.clear();

        Nodes leftovers;
        for (auto i : old)
        {
            Instruction * pin = get<Instruction>(NOTHR, i);
            if (pin)
            {
                for (auto j : leftovers)
                    i->children[0]->children[1]->children.push_back(j);

                leftovers.clear();

                root->addChild(i);
                continue;
            }

            Macuse * u = get<Macuse>(LNFUN, i);
            gtok = u->tok();

            if (u->name() == "@end")
            {
                for (auto j : u->children[0]->children)
                    leftovers.push_back(j);
                continue;
            }

            Nodes inject = Macros(root).process_macuse(*u);

            if (!inject.empty())
            {
                Pnode pn = inject.front();
                Instruction * pin = get<Instruction>(NOTHR, pn);
                if (pin)
                {
                    for (auto j : leftovers)
                        pn->children[0]->children[1]->children.push_back(j);
                }
                else
                {
                    Macuse * pu = get<Macuse>(LNFUN, pn);
                    for (auto j : leftovers)
                        pu->children[0]->children.push_back(j);
                }

                leftovers.clear();
            }

            for (auto j : inject)
                root->addChild(j);

            weresubs = true;
        }

        if (!weresubs)
        {
            if (leftovers.empty())
                break;

            if (root->children.empty())
                throw Err("Labels used in empty program");

            throw Err("Program finishes with label (see macro definition)", root->children.back()->tok());
        }
        // this can be improved later (one extra loop)
        // when expanding macro we can detect that no new macro introduced
    } // while
}

