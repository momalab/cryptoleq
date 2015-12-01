// CryptoLeq Processor Module
// New York University at Abu Dhabi, MoMa Lab
// Copyright 2014-2015
// Author: Oleg Mazonka
// File: compiler.cpp

#include <fstream>
#include <iostream>

#include "compiler.h"

void Compiler::next_rnd()
{
    if ( rndN.iszero() )
        throw "Rnd used, but not initialised: set random seed";

    rndN = rndN.mul(rndN, proc.N2); // for rnd need N, for rndN need N2
}

void Compiler::init_pragma() {}

void Compiler::init_pqkru()
{
    const char * name = "pqkru";
    std::ifstream in(name);
    if ( !in ) throw string() + "Cannot open " + name;

    in >> p >> q >> k >> rnd >> bit_guard;

    init(p * q);
}

void Compiler::init(Unumber n)
{
    Unumber pq = p * q;

    if ( !n.iszero() && !pq.iszero() && !(n == pq) )
        throw "Inconsistent P Q N";

    if ( !pq.iszero() )
        proc.init(pq);
    else
        proc.init(n);

    if ( proc.N.iszero() || pq.iszero() )
    {
        g = p1Nk1N = phim1 = km1 = phi = Nm1 = rndN = rnd = 0;
        return;
    }

    rnd = congruenceN(rnd);
    k = congruenceN(k);

    rndN = rnd;
    rndN.pow( proc.N, proc.N2 );

    phi = (p - 1) * (q - 1);

    if ( !isPrime(p) ) throw "P is not prime";
    if ( !isPrime(q) ) throw "Q is not prime";

    bool ik;

    if ( !rnd.iszero() )
    {
        ik = ma::invert(rndN, proc.N, &phim1); // test rnd
        if ( !ik ) throw "Cannot invert seed: " + rnd.str() + " ^N = " + rndN.str();
    }

    ik = ma::invert(phi, proc.N, &phim1); // inverting by N, not N2
    if ( !ik ) throw "Cannot invert phi";

    ik = ma::invert(k, proc.N, &km1);  // inverting by N, not N2
    if ( !ik ) throw "Cannot invert k";

    g = 1 + k * proc.N;

    ik = ma::invert(proc.N - phi, phi, &Nm1); // inverting N by phi
    if ( !ik ) throw "Cannot invert N";

    p1Nk1N = phim1.mul(km1, proc.N); // modulus is N, not N2

    if ( bit_guard < 1 || bit_guard > 100000 )
        throw "Bit guard (u) must be greater than 0";

    if ( proc.beta < bit_guard )
    {
        std::cout << "\nAttention: beta=" << proc.beta
                  << " is smaller than bit guard: " << bit_guard << '\n';
        std::cout << "N2-A2=" << (proc.N2 - proc.A2).str()
                  << " must be well above N=" << proc.N.str() << '\n';
        throw "Small range: select other N which is "
        "not slightly above power of 2.";
    }

}

void Compiler::show() const
{
    proc.show();
    std::cout << "Compiler:";
    std::cout << " p=" << p << " q=" << q;
    std::cout << " phi=" << phi <<  " k=" << k ;
    std::cout << " g=" << g << " Nm1=" << Nm1;
    std::cout << " p1Nk1N=" << p1Nk1N;
    std::cout << " rnd=" << rnd << " rndN=" << rndN << '\n';
}

Cell Compiler::encrypt(Unumber x, Unumber r)
{
    r.pow(proc.N, proc.N2 );
    return encryptRN(x, r);
}

Cell Compiler::encryptRN(Unumber ix, Unumber rN)
{
    //Unumber rN = random(); // M.random returns r^N, so no powering required
    Unumber gm = access_g();

    gm.pow( congruenceN(ix), proc.N2 );

    Unumber x = rN.mul(gm, proc.N2);

    return proc.x2ts(x);
}

void Compiler::init_pqkru(Unumber an, Unumber ap, Unumber aq,
                          Unumber ak, Unumber r, unsigned u)
{
    p = ap; q = aq;
    k = ak;
    rnd = r;
    bit_guard = u;

    if ( ( p.iszero() || q.iszero() ) && !rnd.iszero() )
        throw "Rnd is set but PQ is not";

    init(an);
}

Unumber Compiler::fkf() const
{
    if ( proc.N2.iszero() ) return proc.N2;
    Unumber Nphi = proc.N.mul(phi, proc.N2);
    Unumber pn = p1Nk1N.mul(sneak, proc.N2);
    return phi.mul(pn, Nphi);
}

Unumber Compiler::decrypt(Cell A, Unumber * R)
{
    Unumber x(fkf());

    Unumber mp1(1), a(A.x());

    while (x != 0)
    {
        if (x.getbit(0))
            mp1 = mp1.mul(a, proc.N2);

        a = a.mul(a, proc.N2);
        x >>= 1;
    }

    //return mp1;
    Unumber m = mp1 - 1;
    if (m % proc.N != 0)
        throw "Bad encrypted value " + A.x().str();

    m = m / proc.N;

    // m is found, now calculate r
    // first get r^N by r^N*(1+Nkm) * (1-Nkm)
    // (1-Nkm) = g^(N-m);

    if (!R)
        return m;

    Unumber r = access_g();

    r.pow(congruence(proc.N - m, proc.N), proc.N2);
    r = r.mul(A.x(), proc.N2);
    r.pow(access_Nm1(), proc.N);

    // verify

    Unumber rN = r;
    rN.pow(proc.N, proc.N2);
    Unumber y = access_g();
    y.pow(m, proc.N2);

    y = y.mul(rN, proc.N2);

    if (A.x() != y)
        throw "extract_rm - verification failed for " + A.x().str();

    *R = r;
    return m;
}

bool Compiler::isPrime(const Unumber & n)
{
    Unumber p = n - 1;

    unsigned max = MaxPrimeCheck;
    if ( p < Unumber(max) )
        max = (unsigned)p.to_ull();

    unsigned lowprimes[] = { 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43,
                             47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97, 101,
                             103, 107, 109, 113, 127, 131, 137, 139, 149, 151,
                             157, 163, 167, 173, 179, 181, 191, 193, 197, 199,
                             211, 223, 227, 229, 233, 239, 241, 251, 257, 263,
                             269, 271, 277, 281, 283, 293, 307, 311, 313, 317,
                             331, 337, 347, 349, 353, 359, 367, 373, 379, 383,
                             389, 397, 401, 409, 419, 421, 431, 433, 439, 443,
                             449, 457, 461, 463, 467, 479, 487, 491, 499, 503, 509
                           };

    unsigned lpsz = sizeof(lowprimes) / sizeof(lowprimes[0]);
    if ( lowprimes[lpsz - 1] < n )
        for ( unsigned i = 0; i < lpsz; i++ )
            if ( n % lowprimes[i] == 0 )
                return false;

    return isEuler(n, p, max);
}

bool Compiler::isEuler(const Unumber & m, const Unumber & p, unsigned max)
{
    Unumber one(1);

    for ( unsigned i = 2; i < max; i++ )
    {
        Unumber u(i);
        u.pow(p, m);
        if ( u != one )
            return false;

        //std::cout << 'x' << std::flush; // this is used in other places - cannot print
    }
    return true;
}
