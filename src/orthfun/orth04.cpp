// Orthfun research
// New York University at Abu Dhabi, MoMa Lab
// Copyright 2014-2015
// Author: Oleg Mazonka
// File: orth04.cpp

/*
        Same as 03 but for latex
*/

#include <iostream>

#include "../processor/compiler.h"
#include "../ceal/errex.h"


void nextr() { std::cout << " \\\\ \\hline\n"; }
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

void tmain(int ac, const char * av[])
{
    // Create compiler object
    Compiler comp;

    // initialize
    Unumber p(11), q(7);
    Unumber N(p * q);
    comp.init_pqkru(N, p, q, 1, 2, 1); // k=1, r=2, u=1

    // Once compiler is initialized, let Cell know about N
    Cell::setN(comp.proc.N);

    Unumber r, r0 = 2; // set base
    int n = 2; // order

    while (1)
    {
        r = r0;
        r.pow(n, N);
        if ( r == r0 ) break;
        n++;
    }

    std::cout << "{";
    for ( int i = 0; i <= n; i++ )
        std::cout << (i == 1 ? "|" : "") << "|c";
    std::cout << "|}\n";

    std::cout << "\n";
    for ( int i = 0; i < n; i++ )
        std::cout << "&" << (i % 10);
    std::cout << "\\\\ \\hline\n\n";

    for ( int m = 0; m < N; m++ )
    {
        std::cout << m;
        for ( int i = 0; i < n; i++ )
        {
            Unumber t(0), s(0);
            Cell e;
            bool ok = false;
            try
            {
                r = r0;
                r.pow(i, N);
                e = comp.encrypt(m, r);
                t = e.ts().t;
                s = e.ts().s;
                ok = true;
            }
            catch (...) {}

            std::cout << '&';
            outrm(m, r.to_ull(), t.str(), s.str(), ok, comp.proc.leq(e) );
        }
        nextr();
    }

    std::cout << "n=" << n << '\n';
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


