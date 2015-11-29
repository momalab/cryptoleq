// CryptoLeq Enhanced Assembler
// New York University at Abu Dhabi, MoMa Lab
// Copyright 2014-2015
// Author: Oleg Mazonka
// File: memory.cpp


#include "memory.h"

void MemoryTs::put(Cell place, Cell value)
{
    auto pts = place.ts();
    const Unumber & t = pts.t;

    Sector & sector = m[pts.s];
    auto i = sector.upper_bound(pts.t);

    if ( i == sector.begin() )
    {
        sector.insert(std::pair<Tvalue, Section>(t, Section(t)));
        i = sector.begin();
    }
    else
        --i;

    if ( i->second.end() == t )
    {
        i->second.v.push_back(value);
        return;
    }

    if ( t < i->second.end() )
        throw "Memory overlap while loading: " + place.str();

    auto k = sector.insert(std::pair<Tvalue, Section>(t, Section(t)));
    i = k.first;
    i->second.v.push_back(value);

    // check overlap with the next section (safe to delete later)
    if (0)
    {
        if (++i == sector.end())
            return; // ok

        if (k.first->second.end() < i->second.start) {}
        else // this error is never reachable
            throw "Memory overlap while loading: " + k.first->second.end().str();
    }
}

Cell & MemoryTs::operator[](const Cell & place)
{
    auto pts = place.ts();
    const Unumber & t = pts.t;

    Sector & sector = m[pts.s];
    auto i = sector.upper_bound(pts.t);

    if (i == sector.begin()) goto bad;
    --i;

    {
        auto & j = i->second;
        if (t < j.end()) {}
        else goto bad;

        size_t idx = (size_t)(t - j.start).to_ull();
        return j.v[idx];
    }

bad:
    throw "Memory access violation, address=" + place.str();
}

string MemoryTs::dump() const
{
    string r;

    for (auto i : m)
    {
        for (auto j : i.second)
        {
            r += j.first.str() + "." + i.first.str() + ":\t";

            for (auto k : j.second.v)
                r += " " + k.str();

            r += '\n';
        }
    }

    return r;
}

