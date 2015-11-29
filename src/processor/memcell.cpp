// CryptoLeq Processor Module
// New York University at Abu Dhabi, MoMa Lab
// Copyright 2014-2015
// Author: Oleg Mazonka
// File: memcell.cpp

#include "memcell.h"

Unumber CellCommon::N = 0;
Unumber CellCommon::N2 = 0;

namespace ma {bool invert(const Unumber & x, const Unumber & mod, Unumber * xm1);}
namespace ma {Unumber gcd(const Unumber & x, const Unumber & y);}

// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = //

bool CellCommon::inv(const Unumber & x, const Unumber & n, Unumber * xm1)
{
    if (!n.iszero())
        return ma::invert(x, n, xm1);

    *xm1 = Unumber(2) - x;
    return true;
}

Unumber CellCommon::inv(const Unumber & x, const Unumber & n)
{
    Unumber r;
    bool k = inv(x, n, &r);

    if (!k)
        throw "Cannot invert " + x.str();

    return r;
}

Unumber CellCommon::toX(Unumber t, Unumber s)
{
    if (N.iszero())
    {
        static Unumber fakeN = fake();
        return t + fakeN * s + 1;
    }

    return N * t + s + 1;
}

Unumber CellCommon::fake()
{
    Unumber m1 = Unumber(0) - 1;
    int k = 0;
    while (!m1.iszero())
    {
        k++;
        m1 >>= 1;
    }

    k >>= 1;
    Unumber r = (Unumber(1) << k);
    return r;
}

// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = //

CellTs CellTs::fromX(Unumber x)
{
    if (N.iszero()) // 0 is valid -1 value
    {
        static Unumber fN = fake();
        x -= 1;
        Unumber negx = Unumber(0) - x;
        if (!(negx < x))
            return CellTs(x % fN, x / fN);
        else
            return CellTs(Unumber(0) - negx % fN, negx / fN);
    }

    if (x.iszero())
        throw "Operation with zero";

    x -= 1;
    return CellTs(x / N, x % N);
}

string CellTs::str() const
{
    return v.t.str() + (v.s.iszero() ? "" : "." + v.s.str());
}

// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = //

string CellX::str() const
{
    auto v = ts();
    return v.t.str() + (v.s.iszero() ? "" : "." + v.s.str());
}

CellCommon::TsVal CellX::ts() const
{
    Unumber x(z);
    x -= 1;

    Unumber t, s;

    if (N.iszero())
    {
        static Unumber fakeN = fake();
        Unumber negx = Unumber(0) - x;

        if ( !(negx < x) )
            t = x.div(fakeN, s);
        else
        {
            t = negx.div(fakeN, s);
            t = Unumber(0) - t;
        }
    }
    else
        s = x.div(N, t);

    return TsVal(t, s);
}

Cell minusone_calc()
{
    const Unumber & N = CellCommon::getN();
    Unumber m1u;
    if (N != 0)
        m1u = N - 1;
    else
        m1u = Unumber(0) - 1;

    return Cell(m1u, Cell::TS);
}

Cell minusone()
{
    static Cell m1 = minusone_calc();
    return m1;
}

// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = //

