// / CryptoLeq / C++ prototype
// New York University at Abu Dhabi, MoMa Lab
// Copyright 2014-2015
// Author: Oleg Mazonka
// File: open_int.cpp

#include <sstream>

#include "open_int.h"

template <Compiler & M> bool open_int<M>::inited;

template <Compiler & M> open_int<M> open_int<M>::B2(0);
template <Compiler & M> open_int<M> open_int<M>::zero(0);
template <Compiler & M> open_int<M> open_int<M>::one(0);
template <Compiler & M> open_int<M> open_int<M>::two(0);
template <Compiler & M> open_int<M> open_int<M>::beta1(0);
template <Compiler & M> open_int<M> open_int<M>::beta(0);


//**********************************************************************************
// delegations

template <Compiler & M>
open_int<M> open_int<M>::operator+(const open_int<M> & y) const
{
    open_int<M> r(*this);
    r += y;
    return r;
}

template <Compiler & M>
open_int<M> & open_int<M>::operator+=(const open_int<M> & y)
{
    base_int<M>::operator+=(y);
    return *this;
}

template <Compiler & M>
open_int<M> open_int<M>::operator-(const open_int<M> & y) const
{
    open_int<M> r(*this);
    r -= y;
    return r;
}

template <Compiler & M>
open_int<M> & open_int<M>::operator-=(const open_int<M> & y)
{
    base_int<M>::operator-=(y);
    return *this;
}

template <Compiler & M>
open_int<M> open_int<M>::operator*(const open_int<M> & y) const
{
    open_int<M> r(*this);
    r *= y;
    return r;
}

// end of delegations

//**********************************************************************************
// definitions


template <Compiler & M>
void open_int<M>::open_int_init(Unumber ix)
{
    base_int<M>::ts = Cell(base_int<M>::congruenceN(ix), 0);

    if ( inited )
    {
        std::ostringstream os; os << ix;
        throw "open_int dynamic initialization " + os.str();
    }
}

template <Compiler & M>
bool open_int<M>::operator<(const open_int & y) const
{
    return (y - *this).gt0();
}

template <Compiler & M>
open_int<M> & open_int<M>::operator*=(const open_int<M> & y)
{
    return *this = open_mul<M>(*this, y);
}

template <Compiler & M>
std::string open_int<M>::show() const
{
    auto z = base_int<M>::ts.ts();
    if ( z.s != 0 ) throw "open_int<M>::show - bad value";

    return "[" + this->X().str() + "=1+N*" + z.t.str() + "]";
}

// end of definitions

//**********************************************************************************
// implementation

template <Compiler & M>
void open_int<M>::init_consts()
{
    zero = open_int<M>(0);
    one = open_int<M>(1);
    two = open_int<M>(2);
    B2 = open_int<M>(M.proc.B2);
    beta1 = open_int<M>(M.proc.beta + 1);
    beta = open_int<M>(M.proc.beta);

    inited = true;
}

// end of implementation

//**********************************************************************************
// instantiation

template class open_int<compiler>;

// end of instantiation
