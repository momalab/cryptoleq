// CryptoLeq Processor Module
// New York University at Abu Dhabi, MoMa Lab
// Copyright 2014-2015
// Author: Oleg Mazonka
// File: processor.h

#ifndef PROCESSOR_H__
#define PROCESSOR_H__

#include <string>

#include "../unumber/unumberg.h"

#include "memcell.h"

class ProcessorU
{
    public:
        ProcessorU(): N(0) {}
        ProcessorU(Unumber n): N(0) { init(n); }

        Unumber N, N2; // N2=N^2

        unsigned beta; // B2 = 2^beta, negative bit position
        Unumber A2, B2; // see bit layout doc
        Unumber Xp1, Xp2; // for leq X test

        unsigned high_bit_posN;
        unsigned high_bit_posN2;

        void init(Unumber n);
        void setB2Beta(unsigned beta);
        void show() const;

    protected:
        bool leq_u(const Unumber & x) const;

};

class ProcessorTS : public ProcessorU
{
        void zeroExtract(const Cell & c, Unumber & t, Unumber & s) const;
        bool leq_ts(const Cell & x) const;
        bool leq_x(const Cell & x) const;

    public:
        ProcessorTS(): ProcessorU() {}
        ProcessorTS(Unumber n): ProcessorU(n) {}

        Cell BAm1(const Cell & A, const Cell & B) const;
        Cell BAtt(const Cell & A, const Cell & B) const;

        bool leq(const Cell & z) const
        {
            return Cell::based == Cell::X ? leq_x(z) : leq_ts(z);
        }

        string str(const Cell & x) const;

        Cell x2ts(const Unumber & x) const { return Cell(x, Cell::X); }
};

Unumber congruence(Unumber x, const Unumber & n);

#endif
