// / CryptoLeq / C++ prototype
// New York University at Abu Dhabi, MoMa Lab
// Copyright 2014-2015
// Author: Oleg Mazonka
// File: secure_int.cpp

#include "secure_int.h"

//**********************************************************************************
// consts definitions

template <Compiler & M> secure_int<M> secure_int<M>::B2;
template <Compiler & M> secure_int<M> secure_int<M>::one;
template <Compiler & M> secure_int<M> secure_int<M>::zero;

// end of consts definitions


//**********************************************************************************
// delegations

template <Compiler & M>
secure_int<M> secure_int<M>::operator+(const secure_int<M> & y) const
{
    secure_int<M> r(*this);
    r += y;
    return r;
}

template <Compiler & M>
secure_int<M> secure_int<M>::operator-(const secure_int<M> & y) const
{
    secure_int<M> r(*this);
    r -= y;
    return r;
}

template <Compiler & M>
secure_int<M> & secure_int<M>::operator+=(const secure_int<M> & y)
{
    base_int<M>::operator+=(y);
    return *this;
}

template <Compiler & M>
secure_int<M> & secure_int<M>::operator-=(const secure_int<M> & y)
{
    base_int<M>::operator-=(y);
    return *this;
}

template <Compiler & M>
secure_int<M> secure_int<M>::operator*(const secure_int<M> & y) const
{
    return secure_mul<M>(*this, y);
}

template <Compiler & M>
secure_int<M> secure_int<M>::secret_module(const secure_int & B) const
{
    return secure_mdl<M>(*this, B);
}

// end of delegations

//**********************************************************************************
// definitions

template <Compiler & M>
secure_int<M>::secure_int(Unumber x)
{
    if ( x == 0 ) throw "Operation with zero";
    x -= 1;
    base_int<M>::ts = Cell(x / M.proc.N, x % M.proc.N);
}

template <Compiler & M>
secure_int<M> secure_int<M>::absval() const
{
    const secure_int<M> & x = *this;
    secure_int<M> n = secure_int<M>::zero - x;
    return x.G(x) + n.G(n);
}

template <Compiler & M>
secure_int<M> secure_int<M>::equal(const secure_int<M> & y) const
{
    secure_int<M> a = *this - y;

    if (0)
    {
        return one - a.absval().G(one);
    }

    secure_int<M> b = secure_int<M>::zero - a;
    secure_int<M> ao = a.G(one);
    secure_int<M> bo = b.G(one);
    return one - (ao + bo);
}

template <Compiler & M>
secure_int<M> secure_int<M>::make_const(Unumber ix)
{
    secure_int<M> r;
    r.ts = M.encrypt(ix);
    return r;
}

template <Compiler & M>
void secure_int<M>::init_consts()
{
    zero = make_const(0);
    one = make_const(1);
    B2 = make_const(M.proc.B2);
}

template <Compiler & M>
std::string secure_int<M>::show() const
{
    Unumber r, m;

    extract_rm<M>(*this, r, m);
    return "[" + this->X().str() + "=" + r.str() + "^N*|1+Nk*"
           + m.str() + "] (" + base_int<M>::ts.str() + ")";
}

// end of definitions

//**********************************************************************************
// implementation


// end of implementation

//**********************************************************************************
// instantiation

template class secure_int<compiler>;

// end of instantiation
