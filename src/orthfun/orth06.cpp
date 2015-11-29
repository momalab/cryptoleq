// Orthfun research
// New York University at Abu Dhabi, MoMa Lab
// Copyright 2014-2015
// Author: Oleg Mazonka
// File: orth06.cpp

/*
        Cell folding r^N2
*/

#include <iostream>
#include <set>

#include "../processor/compiler.h"
#include "../ceal/errex.h"

Unumber calca(Unumber k, Unumber N, Unumber Mod)
{
    Unumber x = k.mul(2, N);
    Unumber k2 = Cell::inv(x, Mod);
    return congruence(k2 * (N - 1), Mod);
}

void fold(const Compiler & comp, Cell & z, Unumber base);

void tmain(int ac, const char * av[])
{
    // Create compiler object
    Compiler comp;

    // initialize
    Unumber p(7), q(11);
    Unumber N(p * q);
    Unumber N2(N * N);
    Unumber k = 5;
    Unumber R = 3;
    comp.init_pqkru(N, p, q, k, R, 1); // u=1
    Unumber rN = R; R.pow(comp.proc.N, comp.proc.N2);

    // Once compiler is initialized, let Cell know about N
    Cell::setN(comp.proc.N);

    std::set<Cell> zcntr;
    Unumber a = 2;
    for ( int i = 0; i < N; i++ )
    {
        Unumber r = a;
        r.pow(i, N2);

        // testing R
        Unumber z = r; z.pow(N, N2); z.pow(N, N2);
        Cell cz(0, 0);
        try { cz = comp.proc.x2ts(z); }
        catch (...) { continue; }

        bool lz1 = comp.proc.leq(cz);

        Cell c0 = cz;
        fold(comp, cz, z);

        if ( zcntr.find(cz) != zcntr.end() ) continue;
        zcntr.insert(cz);

        bool lz2 = comp.proc.leq(cz);

        // Output
        std::cout << zcntr.size() << '\t' << i
                  << '\t' << c0.x().str() << '\t' << lz1
                  << '\t' << cz.x().str() << '\t' << lz2
                  << '\n';
    }

}


void fold(const Compiler & comp, Cell & z, Unumber base)
{
    //Cell a = comp.proc.x2ts(base);
    Cell a = comp.proc.x2ts(2);
    Cell t1 = comp.proc.x2ts(3);
    Cell t2 = comp.proc.x2ts(5);

    auto leq = [&comp](Cell x, Cell t) -> bool
    {
        Cell y = comp.proc.BAm1(t, x);
        return comp.proc.leq(y);
    };

    while (1)
    {
        while (comp.proc.leq(z))
            z = comp.proc.BAm1(a, z);

        if (leq(z, t1)) break;

        z = comp.proc.BAm1(t1, z);
    }
}



int main(int ac, const char * av[])
try
{
    try { tmain(ac, av); }
    catch (string e) { throw; }
    catch (char const * e) { throw string(e); }
    catch (Err e) { throw e.str(); }
    catch (...) { throw string("unknown"); }
}
catch (string e)
{
    std::cout << "Error: " << e << '\n';
}


