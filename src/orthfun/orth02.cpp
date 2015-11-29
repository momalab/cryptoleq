// Orthfun research
// New York University at Abu Dhabi, MoMa Lab
// Copyright 2014-2015
// Author: Oleg Mazonka
// File: orth02.cpp

/*
   Building a table of TS on MR dimension
*/

#include <iostream>

#include "../processor/compiler.h"
#include "../ceal/errex.h"

void outrm(int m, int r, string t, string s, bool ok, bool leq)
{
    string a = ".";
    if ( ok )
    {
        a[0] = ' ';
        if ( leq ) a[0] = 'X';
    }

    std::cout << a;
}

void outrmX(int m, int r, string t, string s, bool ok, bool leq)
{
    std::cout << m << '\t' << r << '\t' << t << '\t'
              << s << '\t' << ok << '\t' << leq << '\n';
}

void nextr()
{
    std::cout << '\n';
}

void tmain(int ac, const char * av[])
{
    // Create compiler object
    Compiler comp;

    // initialize
    Unumber p(7), q(11);
    Unumber N(p * q);
    comp.init_pqkru(N, p, q, 1, 2, 1); // k=1, r=2, u=1

    // Once compiler is initialized, let Cell know about N
    Cell::setN(comp.proc.N);

    for ( int r = 0; r < N; r++ )
    {
        for ( int m = 0; m < N; m++ )
        {
            Unumber t(0), s(0);
            Cell e;
            bool ok = false;
            try
            {
                e = comp.encrypt(m, r);
                t = e.ts().t;
                s = e.ts().s;
                ok = true;
            }
            catch (...) {}

            outrm(m, r, t.str(), s.str(), ok, comp.proc.leq(e) );
        }
        nextr();
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


