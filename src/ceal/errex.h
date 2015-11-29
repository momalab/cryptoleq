// CryptoLeq Enhanced Assembler
// New York University at Abu Dhabi, MoMa Lab
// Copyright 2014-2015
// Author: Oleg Mazonka
// File: errex.h

#pragma once

#include <string>
#include <sstream> // bug see below

#include "nodes.h"

using std::string;


class Err
{
        string msg;

        static string tok_str(const Token &);
        static string lnpos_str(int ln, int ps, int fidx);
        static string lntok_str(const Token &);

    public:

        Err(const string & s, int ln, int ps, int fidx);
        Err(const string & s, const Token &);
        Err(const string & s, const Token &, const Token &);
        Err(const string & s) : msg(s) {}
        Err(int ln, const char * fn, const char * fl);

        string str() const { return msg; }
};

namespace bug // as of today gcc 4.9.2 cygwin does not have to_string
{
template <class T> inline string to_string(T x)
{ std::ostringstream os; os << x; return os.str(); }
} // bug

