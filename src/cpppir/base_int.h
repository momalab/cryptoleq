// / CryptoLeq / C++ prototype
// New York University at Abu Dhabi, MoMa Lab
// Copyright 2014-2015
// Author: Oleg Mazonka
// File: base_int.h

#ifndef BASE_INT_H__
#define BASE_INT_H__


#include <string>

#include "../processor/compiler.h"

template <Compiler & M>
class base_int
{
    protected:
        Cell ts;

    protected:
        void BAm1(const base_int & A);

        base_int() {} // ts is set to 0.0

        static Unumber congruenceN(Unumber ix) { return M.congruenceN(ix); }
        static Unumber congruenceN2(Unumber x) { return M.congruenceN2(x); }

        base_int<M> & operator+=(const base_int<M> & y);
        base_int<M> & operator-=(const base_int<M> & y);

    public:
        bool gt0() const { return !M.proc.leq(ts); }

        Unumber X() const { return ts.x(); }

        std::string str() const { return X().str(); }
        std::string show() const { throw "base_int show"; }
};

extern Compiler compiler;

#endif
