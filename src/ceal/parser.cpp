// CryptoLeq Enhanced Assembler
// New York University at Abu Dhabi, MoMa Lab
// Copyright 2014-2015
// Author: Oleg Mazonka
// File: parser.cpp

#include "parser.h"
#include "tokzer.h"
#include "getptr.h"
#include "errex.h"

Pnode Parser::program(Compiler * comp)
{
    root = new Root; // ok - returning Pnode
    root->comp = comp;

    if ( ts.empty() ) return Pnode(root);

    Nodes leftover_labels;
    while ( !ts[ip].is(Token::tEof) )
    {
        Pnode i = macro(leftover_labels);

        if (!i)
            i = instruction(leftover_labels);

        if (!i) throw "Internal error - bad instruction";

        if (get<Macdef>(NOTHR, i))
            root->addMacdef(i);

        else if (!i->children.empty()) // ignore empty instructions: empty lines and ;;;
            root->addChild(i);
    }

    return Pnode(root);
}

Pnode Parser::instruction(Nodes & leftover_labels)
{
    Instruction * i = new Instruction(ts[ip]);

    if ( ts[ip].is(Token::tTld) )
    {
        i->tilda = true;
        ++ip;
    }

    if ( ts[ip].is(Token::tDot) ) // note: no else if
    {
        i->typ = Instruction::eData;
        ++ip;
    }
    else if ( ts[ip].is(Token::tLcu) )
    {
        i->typ = Instruction::eCurl;
        ++ip;
    }

    while (1)
    {
        Pnode li = litem(leftover_labels);

        if ( !li )
        {
            if ( i->typ == Instruction::eCurl )
            {
                if ( ts[ip].is(Token::tRcu) ) break;
                throw Err("Expecting '}'", ts[ip]);
            }

            if ( ts[ip].isEos() ) break;
            throw Err("Unexpected token", ts[ip]);
        }

        i->addChild(li);
    }

    ++ip; // finished with ';', Eol, or '}'

    return Pnode(i);
}

Pnode Parser::macro(Nodes & leftover_labels)
{
    auto ip0 = ip;

    //read labels
    Pnode labels(new Labels(ts[ip]));
    while (1)
    {
        Pnode lab = label();
        if (!lab)
            break;

        labels->addChild(lab);
    }

    if (!ts[ip].is(Token::tMac))
    {
        ip = ip0;
        return Pnode();
    }

    if (ts[ip].s == "def")
    {
        if (!labels->children.empty() || !leftover_labels.empty())
            throw Err("Macro definition cannot follow labels", ts[ip]);

        return macrodef();
    }

    // this is Macuse
    for (auto i : leftover_labels)
        labels->addChild(i);

    leftover_labels.clear();

    Macuse * mu = new Macuse(ts[ip], labels);

    ++ip;

    // now read all arguments
    while (1)
    {
        Pnode p = expr();
        if (!p) break;
        mu->addChild(p);
    }

    if (ts[ip].isEos()) ++ip;
    else
        throw Err("Macro directive unexpected token", ts[ip]);

    return Pnode(mu);
}

Pnode Parser::macrodef()
{
    if (!ts[ip++].is(Token::tMac))
        throw Err(LNFUN);

    Macdef * md = new Macdef(ts[ip]); // getting the name

    Pnode i = id();
    if (!i) throw Err("Expecting macro name", ts[ip]);

    md->name = get<Idn>(LNFUN, i)->s;

    for (i = id(); i; i = id())
        md->argnams.push_back(i);

    if (!ts[ip].isEos())
    {
        if (!ts[ip++].is(Token::tCol))
            throw Err("Unexpected token in macro defintion", ts[ip - 1]);

        for (i = id(); i; i = id())
            md->globals.push_back(i);

        if (!ts[ip].isEos())
            throw Err("Error in macro definition", ts[ip]);
    }

    ++ip; // ip was Eos

    // at this point we have parsed macdef title - proceed with the body

    Nodes leftover_labels;
    while (1)
    {
        if (ts[ip].is(Token::tEof))
            throw Err("Unexpected end of file in macro definition", md->tok());

        i = macro(leftover_labels);

        if (!i)
            i = instruction(leftover_labels);

        if (!i) throw "Internal error - in macro def";

        Macuse * mu = get<Macuse>(NOTHR, i);

        if (mu && mu->name() == "end")
        {
            if (mu->children[0]->children.empty())
                break; // no need to create @end instruction

            Token t(mu->tok(), mu->tok().typk, "@end");
            Pnode end(new Macuse(t, mu->children[0]));
            md->addChild(end);
            break;
        }

        if (get<Macdef>(NOTHR, i))
            throw Err("Macro definition inside other definition", i->tok(), md->tok());

        else if (!i->children.empty()) // ignore empty instructions: empty lines and ;;;
            md->addChild(i);
    }

    return Pnode(md);
}

Pnode Parser::litem(Nodes & leftover_labels)
{
    auto i0 = ip;
    auto ip0 = ip;

    // first check Def: id = expr
    Pnode nid = id();
    if (ts[ip].is(Token::tEql))
    {
        ++ip;
        Pnode pex = expr();

        if (!pex)
        {
            if ( ts[ip].is(Token::tTld))
                throw Err("Invalid tilde - use $enc() instead", ts[ip]);

            throw Err("Bad expression", ts[ip]);
        }

        return Pnode(new Litem(ts[ip0], nid, pex, Litem::eDef));
    }

    // not Def
    ip = ip0;

    //read labels
    Pnode labels(new Labels(ts[ip]));
    while (1)
    {
        Pnode lab = label();
        if ( !lab )
            break;

        labels->addChild(lab);

        if (ts[ip].is(Token::tEol) || ts[ip].is(Token::tSmc))
        {
            // labels are read but left hanging - add them for the next instruction
            for (auto i : labels->children)
                leftover_labels.push_back(i);
            return Pnode();
        }

        ip0 = ip;
    }

    Pnode itm = item();

    if (!itm)
    {
        ip = i0;
        return Pnode();
    }

    {
        Item * pitm = get<Item>(NOTHR, itm);
        if (pitm && pitm->typ == Item::eId)
        {
            Token e(0, 0);
            if (!labels->children.empty()) e = labels->children[0]->tok();
            if (!leftover_labels.empty()) e = leftover_labels[0]->tok();
            if (!e.is(Token::tNull))
                throw Err("Labels cannot be used in front of array size variables", e, pitm->tok());
        }
    }

    // Litem is good - move all leftovers
    for (auto i : leftover_labels)
        labels->addChild(i);

    leftover_labels.clear();

    return Pnode(new Litem(ts[ip0], labels, itm, Litem::eItm));
}

Pnode Parser::item()
{

    auto i0 = ip;
    bool tld = false;
    if ( ts[ip].is(Token::tTld) )
    {
        tld = true;
        ++ip;
    }

    if ( ts[ip].is(Token::tLsq) )
    {
        Item * i = new Item(ts[ip]);
        i->tilda = tld;

        auto i1 = ++ip;
        Pnode n = id();
        if ( n )
        {
            i->typ = Item::eId;
            if (tld) throw Err("Tilde cannot be used with array size variables", ts[i0]);
        }
        else
        {
            ip = i1;
            ///n = unumber();
            n = expr();
            i->typ = Item::eNum;
            if (!n) throw Err("Bad expr in []", ts[i1]);
        }

        if ( !ts[ip++].is(Token::tRsq) )
            throw Err("Expecting ']'", ts[ip - 1]);

        i->addChild(n);
        return Pnode(i);
    }

    Pitem j(new Item(ts[ip]));
    j->tilda = tld;

    if (ts[ip].is(Token::tStr))
    {
        j->typ = Item::eStr;
        ip++;
        return Pnode(j);
    }

    Pnode ex = expr();

    if ( !ex )
    {
        ip = i0;
        return Pnode();
    }

    j->typ = Item::eExpr;
    j->addChild(ex);
    return Pnode(j);
}

Pnode Parser::id()
{
    if (!ts[ip].is(Token::tIdt))
        return Pnode();
    return Pnode(new Idn(ts[ip++]));
}

Pnode Parser::expr()
{
    auto ip0 = ip;
    Pnode ltm = term();
    if (!ltm)
        return Pnode();

    Expr * ex = nullptr;
    Expr::Typ typ = Expr::eTerm;

    while (1)
    {
        if (ts[ip].is(Token::tPls))
        {
            typ = Expr::ePlus;
            ++ip;
        }
        else if (ts[ip].is(Token::tMin))
        {
            typ = Expr::eMinus;
            ++ip;
        }
        else
        {
            if (ex)
                return Pnode(ex);

            Expr * r = new Expr(ts[ip0]);
            r->addChild(ltm);
            return Pnode(r);
        }

        Pnode rtm = term();
        if (!rtm)
            throw Err("Expecting expression", ts[ip]);

        if (!ex)
        {
            ex = new Expr(ts[ip]);
            ex->typ = typ;
            ex->addChild(ltm);
            ex->addChild(rtm);
        }
        else
        {
            Expr * nx = new Expr(ts[ip]);
            nx->typ = typ;
            nx->addChild(Pnode(ex));
            nx->addChild(rtm);
            ex = nx;
        }
    }

    throw Err(LNFUN);
}

Pnode Parser::term()
{
    auto i0 = ip;
    if (ts[ip].is(Token::tMin))
    {
        ++ip;
        Pnode tm = term();
        if (!tm)
        {
            ip = i0;
            return Pnode();
        }
        return Pnode(new Term(ts[i0], tm, Term::eMinus));
    }

    Pnode x = cnst();
    if (x)
        return Pnode(new Term(ts[i0], x, Term::eCnst));

    x = id();
    if (x)
        return Pnode(new Term(ts[i0], x, Term::eId));

    if (ts[ip].is(Token::tDol))
    {
        ++ip;
        x = id();
        if (!x)
            throw Err("Expecting function name", x->tok());

        if (!ts[ip].is(Token::tLbr))
            return Pnode(new Term(ts[i0], x, Term::eFunc));

        Nodes as = args();
        return Pnode(new Term(ts[i0], x, as, Term::eFunc));
    }

    if (ts[ip].is(Token::tLbr))
    {
        ++ip;
        x = expr();
        if (!x)
            throw Err("Expecting expression", ts[ip]);

        if (!ts[ip++].is(Token::tRbr))
            throw Err("Expecting ')'", ts[ip - 1]);

        return Pnode(new Term(ts[i0], x, Term::eExpr));
    }

    return Pnode();
}

Pnode Parser::cnst()
{
    if (ts[ip].is(Token::tQmk))
        return Pnode(new Cnst(ts[ip++], Cnst::eQmk));

    auto i0 = ip;
    Pnode x = tsnumber();
    if (x)
        return Pnode(new Cnst(ts[i0], x, Cnst::eTsnum));

    if (ts[ip].is(Token::tChr))
        return Pnode(new Cnst(ts[ip++], Cnst::eChr));

    return Pnode();
}

Pnode Parser::tsnumber()
{
    Pnode t = unumber();
    if (!t)
        return Pnode();

    if (!ts[ip].is(Token::tDot) )
        return Pnode(new Tsnum(t->tok(), t));

    ++ip;
    Pnode s = unumber();
    if (!s)
        throw Err("Expecting number", ts[ip]);

    return Pnode(new Tsnum(t->tok(), t, s));
}

Pnode Parser::unumber()
{
    if (!ts[ip].is(Token::tNum))
        return Pnode();

    return Pnode(new Unum(ts[ip++]));
}

Pnode Parser::label()
{
    auto i0 = ip;
    Pnode pn = id();
    if ( !pn )
    {
        pn = tsnumber();
        if ( !pn )
            return Pnode();
    }

    if ( !ts[ip].is(Token::tCol) )
    {
        ip = i0;
        return Pnode();
    }

    ip++;
    return pn;
}

Nodes Parser::args()
{
    Nodes r;

    if (!ts[ip].is(Token::tLbr)) throw Err(LNFUN);

    ++ip;

    while (!ts[ip].is(Token::tRbr))
    {
        Pnode ex = expr();

        if (!ex)
            throw Err("Expecting ')' or expression in function call", ts[ip]);

        r.push_back(ex);

        if (ts[ip].is(Token::tCma))
        {
            ++ip;
            continue;
        }
    }

    ++ip; // tRbr

    return r;
}
