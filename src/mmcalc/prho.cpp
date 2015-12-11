// CryptoLeq Processor Module
// New York University at Abu Dhabi, MoMa Lab
// Copyright 2014-2015
// Author: Oleg Mazonka
// File: prho.cpp

#include <iostream>
#include <sstream>
#include <fstream>
#include <map>
#include <string>
#include <algorithm>
#include <string>
#include <vector>

#include "mmcalc.h"

using std::string;

Unumber gcd(Unumber y, Unumber x);

std::vector<string> factorize(const string & s)
{
    Unumber us(s, Unumber::Decimal);
    std::vector<Unumber> factorize(Unumber);
    std::vector<Unumber> ur = factorize(us);

    std::vector<string> r;
    for ( size_t i = 0; i < ur.size(); i++ )
        r.push_back(ur[i].str());

    return r;
}

string factorone(const string & s)
{
    Unumber us(s, Unumber::Decimal);
    Unumber factorone(Unumber);
    Unumber ur = factorone(us);
    return ur.str();
}

std::vector<Unumber> factorize(Unumber n)
{
    std::vector<Unumber> r;

    if ( mmc::isPrime(n) ) { r.push_back(n); return r; }

    if ( n.getbit(0) == 0 )
    {
        std::vector<Unumber> r = factorize(n / 2);
        r.push_back(2);
        std::sort(r.begin(), r.end());
        return r;
    }

    Unumber init = 1;

tryinit:
    init += 1;

    Unumber x(init), y(init), d(1);

    for (int i = 0; d == 1 && i < 1000000; i++ )
    {
        Unumber g(Unumber, Unumber);
        x = g(x, n);
        y = g(g(y, n), n);

        Unumber xy;
        if ( x < y ) xy = y - x;
        else xy = x - y;

        d = gcd(xy, n);
        if ( d == 1 ) d = gcd(x, n);
    }

    if ( d == n ) goto tryinit;

    Unumber d2 = n / d;

    std::vector<Unumber> r1 = factorize(d);
    std::vector<Unumber> r2 = factorize(d2);

    for ( size_t i = 0; i < r1.size(); i++ ) r.push_back(r1[i]);
    for ( size_t i = 0; i < r2.size(); i++ ) r.push_back(r2[i]);

    std::sort(r.begin(), r.end());
    return r;

}

Unumber factorone(Unumber n)
{
    if ( mmc::isPrime(n) ) return n;

    if ( n.getbit(0) == 0 ) return 2;

    Unumber init = 1;

tryinit:
    init += 1;

    Unumber x(init), y(init), d(1);

    for (int i = 0; d == 1 && i < 1000000; i++ )
    {
        Unumber g(Unumber, Unumber);
        x = g(x, n);
        y = g(g(y, n), n);

        Unumber xy;
        if ( x < y ) xy = y - x;
        else xy = x - y;

        d = gcd(xy, n);
        if ( d == 1 ) d = gcd(x, n);
    }

    if ( d == n ) goto tryinit;

    return d;
}

Unumber g(Unumber x, Unumber m)
{
    x = x.mul(x, m) + 1;
    while ( !(x < m) ) x -= m;
    return x;
}

Unumber gcd(Unumber y, Unumber x)
{
    if ( y < x ) x.swap(y);
    if ( x == 0 ) return y;
    Unumber k;
    for (;;)
    {
        k = y % x;
        if ( k == 0 ) return x;
        x.swap(y);
        x.swap(k);
    }

    return k;
}
