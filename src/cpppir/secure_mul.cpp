// / CryptoLeq / C++ prototype
// New York University at Abu Dhabi, MoMa Lab
// Copyright 2014-2015
// Author: Oleg Mazonka
// File: secure_mul.cpp

#include "secure_int.h"
#include "open_int.h"
#include "secure_ptr.h"

template <Compiler & M>
secure_int<M> d2p2(const secure_int<M> & x, const open_int<M> &)
{
    secure_int<M> p2 = secure_int<M>::one;
    secure_int<M> sum = secure_int<M>::zero;

    for ( open_int<M> i = open_int<M>::zero; i < open_int<M>::beta; i++ )
    {
        secure_int<M> y = (p2 + p2) - x;
        y = y.absval();
        y = y.G(secure_int<M>::one);
        y = (secure_int<M>::one - y).G(p2);
        sum += y;
        p2 += p2;
    }

    return sum;
}

template <Compiler & M> struct P2Table
{
    secure_ptr<M> tbl;
    P2Table();
    secure_int<M> & operator[](const open_int<M> & i) { return tbl[i]; }
};

template <Compiler & M>
P2Table<M>::P2Table(): tbl(open_int<M>::beta, M.random())
{
    secure_int<M> p2 = secure_int<M>::one;
    open_int<M> b1 = open_int<M>::beta - open_int<M>::one;

    for ( open_int<M> i = open_int<M>::zero; i < open_int<M>::beta; i++ )
    {
        tbl[b1 - i] = p2;
        p2 += p2;
    }
}

template <Compiler & M>
secure_int<M> d2p2tbl(const secure_int<M> & x, const open_int<M> & i)
{
    static P2Table<M> p2table;
    return p2table[i];
}

template <Compiler & M>
secure_int<M> d2(const secure_int<M> & x)
{
    secure_int<M> p2 = secure_int<M>::B2;
    secure_int<M> sum = secure_int<M>::zero;

    const bool VLDTE = false;

    for ( open_int<M> i = open_int<M>::zero; i < open_int<M>::beta; i++ )
    {
        secure_int<M> pk;
        if (VLDTE) pk = d2p2<M>(p2, i);
        p2 = d2p2tbl<M>(p2, i);

        if (VLDTE) // validate that d2p2==d2p2tbl
        {
            Unumber r1, m1, r2, m2;
            extract_rm<M>(pk, r1, m1);
            extract_rm<M>(p2, r2, m2);
            if ( m1 != m2 ) throw "d2p2tbl fail";
        }

        secure_int<M> y = sum + p2;
        y += y;
        y -= x;
        sum += (secure_int<M>::one - y).G(p2);
    }

    return sum;
}

template <Compiler & M>
secure_int<M> secure_mul(secure_int<M> x, secure_int<M> y)
{
    secure_int<M> sum = secure_int<M>::zero;
    for ( open_int<M> i = open_int<M>::zero; i < open_int<M>::beta1; i++ )
    {
        secure_int<M> z = d2<M>(x);
        secure_int<M> b = x - (z + z);
        sum += b.G(y);
        y += y;
        x = z;
    }

    return sum;
}

// ************************************************************************

template secure_int<compiler>
secure_mul<compiler>(secure_int<compiler>, secure_int<compiler>);


