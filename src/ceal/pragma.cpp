// CryptoLeq Enhanced Assembler
// New York University at Abu Dhabi, MoMa Lab
// Copyright 2014-2015
// Author: Oleg Mazonka
// File: pragma.cpp

#include <iostream>
#include <fstream>

#include "pragma.h"

void Pragma::read(std::istream & is)
{
    string s;

    for (; is >> s;)
    {
        std::istringstream in(s);
        string k, v;
        std::getline(in, k, '=');
        std::getline(in, v);

        if (0);
        else if ( k == "P" ) sp = v;
        else if ( k == "Q" ) sq = v;
        else if ( k == "u" ) su = v;
        else if ( k == "k" ) sk = v;
        else if ( k == "r" ) sr = v;
        else if ( k == "entry" ) entry = v;
        else if (k == "beta") ubeta = (int)Unumber(v, Unumber::Decimal).to_ull();

        else if ( k == "PQ" )
        {
            size_t i = v.find(".");
            if ( i == string::npos ) throw "PQ parameter must have form X.Y";
            sp = v.substr(0, i);
            sq = v.substr(i + 1);
        }
        else if ( k == "N" )
        {
            size_t i = v.find(".");
            if ( i == string::npos )
                n = Unumber(v, Unumber::Decimal);
            else
            {
                sp = v.substr(0, i);
                sq = v.substr(i + 1);
            }
        }

        else if ( k == "io" ) io = str2io(v);
        else if ( k == "cqtype" )
        {
            if ( v == "x" ) cqtype = Cqtype::X;
            else if ( v == "ts" ) cqtype  = Cqtype::Ts;
            else throw "bad cqtype: must be 'x' or 'ts'";
        }
        else if ( k == "id" ) {}
        else if (k == "ver") {}
        else if (k == "incdir") include_paths.push_back(v);
        else
        {
            other_values[k] = v;
            std::cout << "Warning: pragma key '" << k << "' unrecognized\n";
        }
    }
}

void Pragma::read(const string & line)
{
    std::istringstream is(line);
    read(is);
}

void Pragma::load_file(const string & file)
{
    std::ifstream in(file.c_str());
    if (!in) return;
    read(in);
}

void Pragma::load_dot_input(std::istream & in)
{
    int MAXLINES = 100;
    while (!!in)
    {
        string c;
        in >> c;
        if (c.empty()) continue;
        if (c[0] == '#')
        {
            std::getline(in, c);
            continue;
        }

        if (!--MAXLINES) break;

        if ( c != ".pragma" ) break;

        string line;
        std::getline(in, line);
        read(line);
    }
}

void Pragma::load_dot_input(const string & file)
{
    std::ifstream in(file.c_str());
    if (!in) return;
    load_dot_input(in);
}


unsigned Pragma::U() const
{
    if ( su.empty() )
        return 2;

    std::istringstream is(su);
    unsigned x = 0;
    is >> x;
    return x;
}

string Pragma::iostr() const
{
    if (0) {}
    else if ( io == Io::Ts ) return ""; // default
    else if ( io == Io::Text ) return "io=a";
    else if ( io == Io::X ) return "io=x";
    throw "Unknown io type";
}

Pragma::Io Pragma::str2io(const string & v)
{
    if (0) {}
    else if ( v == "ascii" || v == "a" ) return Io::Text;
    else if ( v == "ts" ) return Io::Ts;
    else if ( v == "x" ) return Io::X;
    throw "io must be 'ascii' ('a'), 'ts',  or 'x'";
}

