// / CryptoLeq / C++ prototype
// New York University at Abu Dhabi, MoMa Lab
// Copyright 2014-2015
// Author: Oleg Mazonka
// File: secure_ptr.cpp

#include "secure_ptr.h"

//**********************************************************************************
// delegations


// end of delegations

//**********************************************************************************
// definitions

template <Compiler & M>
secure_int<M> & secure_ptr<M>::operator[](const open_int<M> & i)
{
    auto j = data.find(i);
    if ( j == data.end() ) data[i] = secure_int<M>::zero;
    return data[i];
}


// end of definitions

//**********************************************************************************
// implementation


// end of implementation

//**********************************************************************************
// instantiation

template class secure_ptr<compiler>;

// end of instantiation
