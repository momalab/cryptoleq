// CryptoLeq Processor Module
// New York University at Abu Dhabi, MoMa Lab
// Copyright 2014-2015
// Author: Oleg Mazonka
// File: mmcalc.h

#include <istream>

#include "../unumber/unumberg.h"

namespace mmc
{
extern bool interactive;
extern std::istream * gin;
void mmcf();
bool isPrime(const Unumber & n);
} //mmc
