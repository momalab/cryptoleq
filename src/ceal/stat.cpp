// CryptoLeq Enhanced Assembler
// New York University at Abu Dhabi, MoMa Lab
// Copyright 2014-2015
// Author: Oleg Mazonka
// File: stat.cpp

#include <fstream>
#include <sstream>
#include <iostream>

#include "getptr.h"
#include "errex.h"
#include "stat.h"

void Stat::init(string file)
{
    filename = file;

    {
        std::ifstream in(filename.c_str());
        if (in)
        {
            string ln;
            std::getline(in, ln, '{');
            std::getline(in, ln, '}');
            std::istringstream is(ln);

            for ( string s; is >> s; )
                addEntry(s);
        }
    }

    std::ofstream of(filename.c_str());

    if ( !of )
        throw Err("Cannot open file for writing [" + filename + "]");

    of << dumpEntries();
    of << "ceal has started and has not finished working...\n";
}

string Stat::dumpEntries() const
{
    string r;
    r += "IP watch list {";
    for ( auto i : entries )
        if ( i.name.empty() )
            r += " " + i.val.str();
        else
            r += " " + i.name;

    r += " }\n";

    return r;
}

void Stat::addEntry(string nm)
{
    if ( nm.empty() ) throw Err(LNFUN);

    if ( std::isdigit(nm[0]) )
        entries.push_back(Entry(Unumber(nm, Unumber::Decimal)));
    else
        entries.push_back(Entry(nm));
}

void Stat::output() const
{
    std::ofstream of(filename.c_str());

    if ( !of )
        throw Err("Cannot open file for writing [" + filename + "]");

    of << dumpEntries();

    if ( entries.empty() ) {}
    else
    {
        of << "\nIP pass counters\n";
        of << "================\n";

        for ( auto i : entries )
        {
            static Cell und = minusone();
            string saddr = "undefined";

            if ( i.addr == und ) {}
            else
                saddr = i.addr.x().str();

            string s = i.name + "[" + saddr + "]";
            while ( s.size() < 20 ) s = s + ' ';

            of << s << "= " << i.counter << '\n';
        }
    }

    of << "\nInstruction stat\n";
    of << "================\n";

    of << "Input/Output   = " << cntr_io << '\n';
    of << "Open           = " << cntr_opn << '\n';
    of << "Secure         = " << cntr_sec << '\n';
    of << "Mixed          = " << cntr_mix << '\n';
    of << "----------------\n";
    of << "Total          = " << (cntr_mix + cntr_sec + cntr_opn + cntr_io) << '\n';
}

void Stat::ip(const Cell & p)
{
    for ( auto & i : entries )
    {
        if ( i.addr == p )
            ++i.counter;
    }
}

void Stat::instr(const Cell & a, const Cell & b)
{
    const Unumber & sa = a.ts().s;
    const Unumber & sb = b.ts().s;

    if ( sa.iszero() )
    {
        if ( sb.iszero() )
            ++cntr_opn;
        else
            ++cntr_mix;
    }
    else
    {
        if ( sb.iszero() )
            ++cntr_mix;
        else
            ++cntr_sec;
    }
}

void Stat::output_e() const
{
    if (filename.empty())
        return;

    std::ofstream of(filename.c_str());

    if ( !of )
        throw Err("Cannot open file for writing [" + filename + "]");

    of << dumpEntries();

    if ( entries.empty() ) {}
    else
    {
        of << "\n= No emulation =\n";
        of << "\nIP pass counters\n";
        of << "================\n";

        for ( auto i : entries )
        {
            static Cell und = minusone();
            string saddr = "undefined";

            if ( i.addr == und ) {}
            else
                saddr = i.addr.x().str();

            string s = i.name;
            while ( s.size() < 10 ) s = s + ' ';

            s += string(i.name.empty() ? " " : "=") + " [" + saddr + "]";

            of << s << '\n';
        }
    }
}

void Stat::addrinit()
{
    if (addrinited)
        return;

    // initialize values
    for ( auto & i : entries )
    {
        if (i.name.empty())
            i.addr = Cell(i.val, Cell::X);
        else
            i.addr = minusone();
    }

    addrinited = true;
}

void Stat::populate_addr(const Root & root)
{
    addrinit();

    // initialize values
    const auto & dic = root.names_defined;

    // assign addresses
    for (auto & i : entries)
    {
        if (i.name.empty()) continue;
        auto j = dic.find(i.name);

        if (j == dic.end())
        {
            std::cout << "Warning: Stat name '" << i.name << "' is not defined\n";
            continue;
        }

        i.addr = j->second;
    }
}
