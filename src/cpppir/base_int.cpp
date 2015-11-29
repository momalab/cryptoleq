// / CryptoLeq / C++ prototype
// New York University at Abu Dhabi, MoMa Lab
// Copyright 2014-2015
// Author: Oleg Mazonka
// File: base_int.cpp

#include "base_int.h"

//**********************************************************************************
// delegations


// end of delegations

//**********************************************************************************
// definitions

template <Compiler & M>
base_int<M> & base_int<M>::operator-=(const base_int<M> & A)
{
    BAm1(A);
    return *this;
}

template <Compiler & M>
base_int<M> & base_int<M>::operator+=(const base_int<M> & y)
{
    base_int<M> Am1;
    Am1.BAm1(y);
    return *this -= Am1;
}

template <Compiler & M>
void base_int<M>::BAm1(const base_int & A)
{
    ts = M.proc.BAm1(A.ts, ts);
}

// end of definitions

//**********************************************************************************
// implementation


// end of implementation

//**********************************************************************************
// instantiation

template class base_int<compiler>;

// end of instantiation
