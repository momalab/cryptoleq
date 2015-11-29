// / CryptoLeq / C++ prototype
// New York University at Abu Dhabi, MoMa Lab
// Copyright 2014-2015
// Author: Oleg Mazonka
// File: open_int.h

#ifndef OPEN_INT_H__
#define OPEN_INT_H__

#include "base_int.h"

template <Compiler & M>
class open_int : public base_int<M>
{
        static bool inited;
        void open_int_init(Unumber x);

        template <Compiler & N> friend open_int<N>
        open_mul(open_int<N> x, open_int<N> y);

    public:
        // constants
        static void init_consts();
        static open_int B2;
        static open_int zero;
        static open_int one;
        static open_int two;
        static open_int beta1;
        static open_int beta;

    public:
        explicit open_int(Unumber x) { open_int_init(x); }
        explicit open_int(unsigned x) { open_int_init(x); }

        bool operator<(const open_int & y) const;
        open_int operator++(int) { open_int t(*this); *this += one; return t; }
        open_int & operator++() { return *this += one; }

        //open_int operator*(const open_int & y) const;
        open_int operator+(const open_int & y) const;
        open_int<M> & operator+=(const open_int<M> & y);

        open_int operator-(const open_int & y) const;
        open_int<M> & operator-=(const open_int<M> & y);

        open_int operator*(const open_int & y) const;
        open_int<M> & operator*=(const open_int<M> & y);

        std::string show() const;
};

template <Compiler & N> open_int<N> open_mul(open_int<N> x, open_int<N> y);

#endif
