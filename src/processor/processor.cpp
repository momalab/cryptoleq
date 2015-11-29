// CryptoLeq Processor Module
// New York University at Abu Dhabi, MoMa Lab
// Copyright 2014-2015
// Author: Oleg Mazonka
// File: processor.cpp

#include <fstream>
#include <iostream>

#include "processor.h"

void ProcessorU::setB2Beta(unsigned b)
{
    if (beta && b > beta)
    {
        std::cout << "Current beta " << beta << "; new value " << b << '\n';
        throw "Setting beta above predefined is forbidden; increase N";
    }

    if (N.iszero() && b > high_bit_posN / 2 )
    {
        std::cout << "Try setting beta " << b << ", but allowed value is " << (high_bit_posN / 2) << '\n';
        throw "Setting beta too high";
    }

    beta = b;
    B2 = (Unumber(1) << b);
}

void ProcessorU::init(Unumber n)
{
    N = n;
    if (N == 1)
        throw "N cannot be 1";

    N2 = A2 = B2 = beta = high_bit_posN = high_bit_posN2 = 0;

    Unumber x = N - 1; // if N==0 - Ok;
    // -1 does not matter for N!=2^n
    // but splits in half when N==2^n

    while (x != 0) { x >>= 1; high_bit_posN++; }
    high_bit_posN--;

    Xp1 = 2;
    Xp2 = Unumber(0) - 1;
    Xp2 >>= 1;

    if (N.iszero())
        return; // leave the rest undefined - 0's

    N = n;
    N2 = N * N;

    high_bit_posN2 = 0;
    x = N2;
    while ( x != 0 ) { x >>= 1; high_bit_posN2++; }
    high_bit_posN2--;

    unsigned nbit = 0;
    while ( (N >> ++nbit) != 1 );
    A2 = (Unumber(1) << nbit);

    Unumber M = N - A2;

    if (M.iszero())
    {
        if (high_bit_posN > 1)
            setB2Beta(high_bit_posN - 1);
        else
        {
            // leave beta and B2 undefined - 0's
        }
    }
    else
    {
        nbit = 0;
        while ((M >> ++nbit) > 1);

        setB2Beta(nbit);

        if (B2 * 2 < A2) {}
        else
            setB2Beta(nbit - 1);
    }

    Unumber tpmax = (Unumber(1) << high_bit_posN) - 1;
    Xp1 = N + 1;
    Xp2 = N * (tpmax + 1);
}

void ProcessorU::show() const
{
    std::cout << "Processor: N=" << N << " N2=" << N2;
    std::cout << " M=" << (N - A2) << " A2=" << A2;
    std::cout << " B2=" << B2 << " beta=" << beta;
    std::cout << " high_bit_posN=" << high_bit_posN << '\n';
}


bool ProcessorU::leq_u(const Unumber & x) const
{
    return x.iszero() || !!x.getbit( high_bit_posN );
}

bool ProcessorTS::leq_ts(const Cell & x) const
{
    return leq_u(x.ts().t);
}

bool ProcessorTS::leq_x(const Cell & z) const
{
    const Unumber & x = z.x();
    bool r =  x < Xp1 || Xp2 < x;

    if (0) // test
    {
        bool d = leq_ts(z);
        if (r != d)
            throw "Bad leq_x";
    }

    return r;
}

Cell ProcessorTS::BAm1(const Cell & A, const Cell & B) const
{
    if (N.iszero())
        return BAtt(A, B);

    if (0) // old impl
    {
        // Step 1 prepare old values
        Unumber x = B.x();
        Unumber Ax = A.x();

        // Step 2 do old operation
        Unumber xm1, a(Ax);
        bool k = ma::invert(a, N2, &xm1);
        if ( !k )
        {
            Unumber g = ma::gcd(a, x);
            if ( g == 1 ) return Cell();
            x /= g;
            a /= g;
            bool k = ma::invert(a, N2, &xm1);
            if ( !k ) return Cell();
        }
        x = x.mul(xm1, N2);

        // Step 3 convert old values to Ts
        x -= 1;
        Unumber t, s;
        s = x.div(N, t);

        return Cell(t, s);
    }
    else
    {
        if (0)
        {
            Cell ai = A.invert();
            Unumber x = B.x().mul(ai.x(), N2);
            return Cell(x, Cell::X);
        }
        else
            return A.invert() * B;
    }
}

Cell ProcessorTS::BAtt(const Cell & A, const Cell & B) const
{
    Unumber at, bt, as, bs;
    zeroExtract(A, at, as);
    zeroExtract(B, bt, bs);

    if (as != bs)
        throw "Illegal instruction";

    return Cell(bt - at, as);
}

void ProcessorTS::zeroExtract(const Cell & c, Unumber & t, Unumber & s) const
{
    auto ts = c.ts();
    t = ts.t;
    s = ts.s;
}

string ProcessorTS::str(const Cell & x) const
{
    if ( !N.iszero() )
        return x.str();

    auto ts = x.ts();
    if (ts.s.iszero() && ts.t.iszero())
        return "0";

    auto xx = x.x();
    if (!leq_u(xx))
        return x.str();

    Unumber mx = Unumber(1) - xx;
    return "-" + mx.str();
}

Unumber congruence(Unumber x, const Unumber & n)
{
    if ( n.iszero() ) return x;

    Unumber nn(n);
    nn <<= 2;
    if (nn > x)
    {
        while (n < x) x -= n;
        return x;
    }

    x.divABRQ(n, &nn, 0);
    return nn;
}
