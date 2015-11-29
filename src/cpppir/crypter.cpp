// CryptoLeq, Encryption Tool
// New York University at Abu Dhabi, MoMa Lab
// Copyright 2014-2015
// Author: Oleg Mazonka
// File: crypter.cpp

#include <iostream>
#include <fstream>
#include <string>

#include "secure_types.h"

const bool SHOWR = true;

Compiler compiler;

typedef secure_int<compiler> sec_int;
typedef open_int<compiler> opn_int;
typedef secure_ptr<compiler> sec_ptr;

string enc(Unumber x);
string dec(Unumber x);
string cor(Unumber x);
string und(Unumber x);

int main(int ac, char * av[]) try
{
    compiler.init_pqkru();
    Cell::setN(compiler.proc.N);

    open_int<compiler>::init_consts();
    secure_int<compiler>::init_consts();

    if ( ac < 3 )
    {
        std::cout << "Usage:\n";
        std::cout << "       crypter {enc|dec|cor|und} {input|-} {output|-}\n";
        std::cout << "       crypter {enc|dec|cor|und} value\n";
        std::cout << "       actions: encrypt, decrypt, decorate, undecorate\n";
        return 1;
    }

    string (*f)(Unumber) = nullptr;
    if ( string(av[1]) == "enc" ) f = enc;
    if ( string(av[1]) == "dec" ) f = dec;
    if ( string(av[1]) == "cor" ) f = cor;
    if ( string(av[1]) == "und" ) f = und;

    if ( f == nullptr ) throw "Must be one of: enc, dec, cor, or und";

    if ( ac == 3 )
    {
        std::cout << f(Unumber(av[2], Unumber::Decimal)) << '\n';
        return 0;
    }

    bool sin = ( string (av[2]) == "-" );
    std::ifstream in;
    if ( !sin )
    {
        in.open(av[2]);
        if ( !in ) throw "Cannot open to read " + string(av[2]);
    }

    bool son = ( string (av[3]) == "-" );
    std::ofstream on;
    if ( !son )
    {
        on.open(av[3]);
        if ( !on ) throw "Cannot open to write " + string(av[3]);
    }

    void process(std::istream & in, std::ostream & on, string (*f)(Unumber));
    process(sin ? std::cin : in, son ? std::cout : on, f);
}
catch (const char * e) { std::cout << "Error: " << e << "\n"; }
catch (std::string e) { std::cout << "Error: " << e << "\n"; }
catch (...) { std::cout << "Exception thrown\n"; }

void process(std::istream & in, std::ostream & on, string (*f)(Unumber))
{
    for (string s; in >> s;)
    {
        Unumber x(s, Unumber::Decimal);
        on << f(x) << '\n';
    }
}

string enc(Unumber x)
{
    return sec_int::make_const(x).str();
}

string dec(Unumber x)
{
    Unumber r, m;
    extract_rm<compiler>(x, r, m);
    return m.str() + (SHOWR ? ("\t(r=" + r.str() + ")") : "");
}

string cor(Unumber ix)
{
    while ( compiler.proc.N2 < ix ) ix -= compiler.proc.N2;
    Unumber x = compiler.proc.N + 1;
    x.pow( ix, compiler.proc.N2 );
    return x.str();
}

string und(Unumber x)
{
    Unumber y = x - 1;
    if ( y % compiler.proc.N != 0 ) throw "undecorate - bad value";

    // verify
    y = y / compiler.proc.N;
    if ( x != compiler.proc.N * y + 1 ) throw "undecorate - verification failed";

    return y.str();
}

