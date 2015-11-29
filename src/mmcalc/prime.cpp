// CryptoLeq Processor Module
// New York University at Abu Dhabi, MoMa Lab
// Copyright 2014-2015
// Author: Oleg Mazonka
// File: prime.cpp

#include <iostream>
#include <sstream>
#include <fstream>
#include <map>
#include <string>
#include <algorithm>
#include <string>
#include <vector>

#include "mmcalc.h"

const unsigned MaxPrimeCheck = 70;

bool isEuler(const Unumber & m, const Unumber & p, unsigned max);

bool mmc::isPrime(const Unumber & n)
{
    Unumber p = n - 1;

    unsigned max = MaxPrimeCheck;
    if ( p < Unumber(max) )
        max = (unsigned)p.to_ull();

    unsigned lowprimes[] = { 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43,
                             47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97, 101,
                             103, 107, 109, 113, 127, 131, 137, 139, 149, 151,
                             157, 163, 167, 173, 179, 181, 191, 193, 197, 199,
                             211, 223, 227,
                             229, 233, 239, 241, 251, 257, 263, 269, 271, 277,
                             281, 283, 293, 307, 311, 313, 317, 331, 337, 347,
                             349, 353, 359,
                             367, 373, 379, 383, 389, 397, 401, 409, 419, 421,
                             431, 433, 439, 443, 449, 457, 461, 463, 467, 479,
                             487, 491, 499, 503, 509
                           };

    unsigned lpsz = sizeof(lowprimes) / sizeof(lowprimes[0]);
    if ( lowprimes[lpsz - 1] < n )
        for ( unsigned i = 0; i < lpsz; i++ )
            if ( n % lowprimes[i] == 0 )
                return false;

    return isEuler(n, p, max);
}

bool isEuler(const Unumber & m, const Unumber & p, unsigned max)
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

