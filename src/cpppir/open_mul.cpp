// / CryptoLeq / C++ prototype
// New York University at Abu Dhabi, MoMa Lab
// Copyright 2014-2015
// Author: Oleg Mazonka
// File: open_mul.cpp

#include "open_int.h"

template <Compiler & M>
open_int<M> absval(open_int<M> x)
{
    if ( x.gt0() ) return x;
    return open_int<M>::zero - x;
}


template <Compiler & M>
open_int<M> d2p2(const open_int<M> & x)
{
    open_int<M> p2 = open_int<M>::one;

    open_int<M> sum = open_int<M>::zero;

    for ( open_int<M> i = open_int<M>::zero; i < open_int<M>::beta; i++ )
    {
        open_int<M> y = (p2 + p2) - x;
        y = absval<M>(y);

        if ( !y.gt0() )
            sum += p2;

        p2 += p2;
    }

    return sum;
}

template <Compiler & M>
open_int<M> d2(const open_int<M> & x)
{
    open_int<M> p2 = open_int<M>::B2;
    open_int<M> sum(open_int<M>::zero);

    for ( open_int<M> i = open_int<M>::zero; i < open_int<M>::beta; i++ )
    {
        p2 = d2p2<M>(p2);
        open_int<M> y = sum + p2;
        y += y;
        y -= x;
        if ( (open_int<M>::one - y).gt0() ) sum += p2;
    }

    return sum;
}

template <Compiler & M>
open_int<M> open_mul(open_int<M> x, open_int<M> y)
{
    if ( y < x )
        std::swap(x, y);

    if ( !x.gt0() )
        return open_int<M>::zero;;

    open_int<M> sum = open_int<M>::zero;

    while ( x.gt0() )
    {
        open_int<M> z = d2<M>(x);
        open_int<M> b = x - (z + z);
        if ( b.gt0() ) sum += y;
        y += y;
        x = z;
    }

    return sum;
}

template open_int<compiler>
open_mul<compiler>(open_int<compiler>, open_int<compiler>);


