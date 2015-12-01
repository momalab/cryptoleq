// CryptoLeq Processor Module
// New York University at Abu Dhabi, MoMa Lab
// Copyright 2014-2015
// Author: Oleg Mazonka
// File: compiler.h

#ifndef COMPILER_H__
#define COMPILER_H__

#include "processor.h"

class Compiler
{
    public:
        ProcessorTS proc;

    private:
        Unumber p, q, phi, phim1; // phim1=phi^-1, km1=k^-1
        Unumber k, km1, g, rndN; // g=1+kN, rndN(seeded and ^N)
        Unumber p1Nk1N; // phim1*km1 mod N
        Unumber Nm1; // N*Nm1=1 mod phi

    public:
        Unumber sneak;

    private:
        // not used values
        Unumber rnd;
        unsigned bit_guard;

        void next_rnd();

        void init(Unumber n);

        static const unsigned MaxPrimeCheck = 70;
        static bool isPrime(const Unumber & n);
        static bool isEuler(const Unumber & m, const Unumber & phi, unsigned max);

        Cell encryptRN(Unumber x, Unumber rN);

    public:
        Compiler(): rndN(0), sneak(1) {}

        void init_pqkru();
        void init_pragma();
        void init_pqkru(Unumber an, Unumber ap, Unumber aq,
                        Unumber ak, Unumber r, unsigned u);

        void show() const;

        bool isrnd() const { return !rnd.iszero(); }
        Unumber random() { next_rnd(); return rndN; }
        Unumber access_g() const { return g; }
        Unumber access_phi() const { return phi; }
        Unumber access_p1Nk1N() const { return p1Nk1N; }
        Unumber access_Nm1() const { return Nm1; }
        Unumber fkf() const;

        Unumber congruenceN(Unumber ix) { return congruence(ix, proc.N); }
        Unumber congruenceN2(Unumber ix) { return congruence(ix, proc.N2); }

        Cell encrypt(Unumber x) { return encryptRN(x, random()); }
        Cell encrypt(Unumber x, Unumber R);

        Unumber decrypt(Cell c, Unumber * R);

        Unumber peek_rndN() const { return rndN; }
};


#endif
