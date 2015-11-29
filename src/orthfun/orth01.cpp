// Orthfun research
// New York University at Abu Dhabi, MoMa Lab
// Copyright 2014-2015
// Author: Oleg Mazonka
// File: orth01.cpp

/*
   This file is a demo program to show how to
   encrypt and decrypt values
*/

#include <iostream>

#include "../processor/compiler.h"
#include "../ceal/errex.h"

void tmain(int ac, const char * av[])
{
    // Create compiler object
    Compiler comp;

    // initialize
    Unumber p(3), q(5);
    comp.init_pqkru(p * q, p, q, 1, 2, 1); // k=1, r=2, u=1

    // Once compiler is initialized, let Cell know about N
    Cell::setN(comp.proc.N);

    Cell e1 = comp.encrypt(3, 2);
    std::cout << e1.x().str() << '\t' << e1.ts().t.str() << '.' << e1.ts().s.str() << '\n';

    Unumber m, r;
    m = comp.decrypt(e1, &r);
    std::cout << m.str() << '\t' << r.str() << '\n';
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

