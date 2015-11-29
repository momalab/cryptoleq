// / CryptoLeq / C++ prototype
// New York University at Abu Dhabi, MoMa Lab
// Copyright 2014-2015
// Author: Oleg Mazonka
// File: secure_int.h

#ifndef SECURE_INT_H__
#define SECURE_INT_H__

#include "base_int.h"


template <Compiler & M>
class secure_int : public base_int<M>
{
    protected:
        // returns re-encrypted B G(A,B)=(A>0)*B'
        secure_int secret_module(const secure_int & B) const;

        template <Compiler & N> friend secure_int<N>
        secure_mul(secure_int<N> x, secure_int<N> y);

        template <Compiler & N> friend secure_int<N>
        secure_mdl(secure_int<N> x, secure_int<N> y);

        template <Compiler & N> friend secure_int<N>
        secure_mp1(secure_int<N> x);

        template <Compiler & N> friend void
        extract_rm(secure_int<N> x, Unumber & r, Unumber & m);

    public:
        secure_int G(const secure_int & B) const { return secret_module(B); }

        static void init_consts();

        // constants
        static secure_int B2;
        static secure_int zero;
        static secure_int one;

    public:
        secure_int(): base_int<M>() {} // Unit value - open encrypted zero
        static secure_int make_const(Unumber x);
        secure_int(Unumber x);

        secure_int absval() const;
        secure_int equal(const secure_int &) const;
        secure_int operator*(const secure_int & y) const;
        secure_int operator+(const secure_int & y) const;
        secure_int operator-(const secure_int & y) const;
        secure_int & operator+=(const secure_int & y);
        secure_int & operator-=(const secure_int & y);

        std::string show() const;
};

template <Compiler & N> secure_int<N> secure_mul(secure_int<N> x, secure_int<N> y);
template <Compiler & N> secure_int<N> secure_mdl(secure_int<N> x, secure_int<N> y);
template <Compiler & N> void extract_rm(secure_int<N> x, Unumber & r, Unumber & m);

#endif
