// CryptoLeq Enhanced Assembler
// New York University at Abu Dhabi, MoMa Lab
// Copyright 2014-2015
// Author: Oleg Mazonka
// File: cloner.cpp

#include <sstream>

#include "nodes.h"
#include "tokzer.h"
#include "utils.h"
#include "getptr.h"
#include "errex.h"

Pnode Node::baseclone(Pnode p) const
{
    p->parent = parent;

    for (auto i : children)
        p->addChild(i->clone());

    return p;
}

Pnode Root::clone() const { throw Err(LNFUN); }

Pnode Instruction::clone() const
{
    Instruction * r = new Instruction(tok());
    r->tilda = tilda;
    r->typ = typ;
    return baseclone(Pnode(r));
}

Pnode Litem::clone() const
{
    Pnode itm = children[0]->clone();
    Pnode lab = children[1]->clone();

    Litem * r = new Litem(tok(), lab, itm, typ);

    r->address = address;
    r->value = value;

    return Pnode(r);
}

Pnode Item::clone() const
{
    Item * r = new Item(tok());
    r->tilda = tilda;
    r->typ = typ;

    return baseclone(Pnode(r));
}

Pnode Expr::clone() const
{
    Expr * r = new Expr(tok());
    r->typ = typ;
    return baseclone(Pnode(r));
}

Pnode Cnst::clone() const
{
    Cnst * r = new Cnst(tok(), typ);
    return baseclone(Pnode(r));
}

Pnode Term::clone() const
{
    Term * r = new Term(tok(), children[0], typ);
    r->children.clear();
    return baseclone(Pnode(r));
}

Pnode Tsnum::clone() const
{
    Tsnum * r = new Tsnum(tok(), children[0]);
    r->children.clear();
    return baseclone(Pnode(r));
}

Pnode Macuse::clone() const
{
    Macuse * r = new Macuse(tok(), children[0]);
    r->children.clear();
    return baseclone(Pnode(r));
}

Pnode Macdef::clone() const
{
    Macdef * r = new Macdef(tok());
    r->name = name;
    r->argnams = argnams;
    r->globals = globals;
    // no copy of table because it will be constructed
    return baseclone(Pnode(r));
}

