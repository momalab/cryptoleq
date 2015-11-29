// CryptoLeq Enhanced Assembler
// New York University at Abu Dhabi, MoMa Lab
// Copyright 2014-2015
// Author: Oleg Mazonka
// File: utils.cpp

#include <fstream>
#include <ctime>

#include "utils.h"

Unumber make_unumber(string v, Unumber def)
{
    if ( v.empty() )
        return def;

    if ( v == "time" )
        return Unumber(std::time(0));

    return Unumber(v, Unumber::Decimal);
}

string print_char(char c)
{
    string sc;
    if ( c >= ' ' && c <= '~' )
        return sc += c;

    std::ostringstream os;
    os << std::hex << int(c);
    sc = os.str();
    if ( sc.size() < 2 ) sc = "0" + sc;
    sc = "\\x" + sc;
    return sc;
}

Cell str2ts(const string & s)
{
    auto i = s.find('.');
    if ( i == string::npos )
    {
        Unumber x(s, Unumber::Decimal);
        return Cell(x, 0);
    }

    Unumber xt(s.substr(0, i), Unumber::Decimal);
    Unumber xs(s.substr(i + 1), Unumber::Decimal);

    return Cell(xt, xs);
}

string file2str(const string & file)
{
    const int MAX_FILE_SIZE = 100000000;

    std::ifstream in(file.c_str(), std::ios::binary);

    if ( !in )
        return "";

    string r;

    in.seekg(0, std::ios::end);

    size_t sz = size_t(in.tellg());

    if ( sz > MAX_FILE_SIZE )
        throw "too big file " + file;

    r.reserve( sz );
    in.seekg(0, std::ios::beg);

    r.assign( std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>() );

    return r;
}

bool isfile(const string & file)
{
    std::ifstream in(file.c_str());
    return !!in;
}


#ifdef _WIN32
#include <direct.h>
#define GetCurrentDir _getcwd
#else
#include <unistd.h>
#define GetCurrentDir getcwd
#endif

string cwd()
{
    char ccp[1000];
    ccp[0] = '\0';
    void * p = GetCurrentDir(ccp, sizeof(ccp));
    if ( p == nullptr ) throw "getcwd returns failure";
    return ccp;
}
