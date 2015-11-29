// CryptoLeq Enhanced Assembler
// New York University at Abu Dhabi, MoMa Lab
// Copyright 2014-2015
// Author: Oleg Mazonka
// File: errex.cpp

#include "tokzer.h"
#include "getptr.h"
#include "errex.h"


namespace
{
string infile(int fidx)
{
    const std::vector<string> & v = Input_token_stream::include_files;

    if ( fidx < 0 || fidx > (int)v.size() )
        Err(LNFUN);

    if (!fidx) return "";
    return "'" + v[fidx - 1] + "' ";
}
}

string Err::lnpos_str(int ln, int ps, int fidx)
{
    std::ostringstream os;
    os << "(" << infile(fidx) << "line " << ln << ":" << ps << ") ";
    return os.str();
}

string Err::lntok_str(const Token & t)
{
    std::ostringstream os;

    os << "(" << infile(t.file_idx) << "line " << t.line
       << ":" << t.pos << tok_str(t) << ") ";

    return os.str();
}

string Err::tok_str(const Token & t)
{
    string s = t.str();

    if ( s.empty() )
        return s;

    for (auto & c : s)
        if (c < ' ' || c > '~') c = '.';

    return " '" + s + "'";
}


Err::Err(const string & s, int ln, int ps, int fidx)
{
    msg = lnpos_str(ln, ps, fidx) + s;
}

Err::Err(const string & s, const Token & t)
{
    msg = lntok_str(t) + s;
}

Err::Err(const string & s, const Token & t, const Token & u)
{
    msg = lntok_str(t) + lntok_str(u) + s;
}

Err::Err(int ln, const char * fn, const char * fl)
{
    std::ostringstream os;
    os << "Internal error (file='" << fl << "', fn='" << fn << "', line=" << ln << ")";
    msg = os.str();
}

