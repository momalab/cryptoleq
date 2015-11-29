// CryptoLeq Enhanced Assembler
// New York University at Abu Dhabi, MoMa Lab
// Copyright 2014-2015
// Author: Oleg Mazonka
// File: memory.h

#pragma once

#include <vector>
#include <map>
#include <unordered_map>

#include "../processor/memcell.h"

class MemoryTs
{
        using Tvalue = Unumber;
        using Svalue = Unumber;

        struct Section
        {
            Unumber start;
            Unumber end() { return start + v.size(); }
            std::vector<Cell> v;
            Section(Unumber s) : start(s) {}
        };

        using Sector = std::map<Tvalue, Section>;
        using Sectors = std::map<Svalue, Sector>;
        Sectors m;

    public:
        MemoryTs() {}
        void put(Cell place, Cell value);
        Cell & operator[](const Cell & k);
        string dump() const;
};

struct HashCell
{
    size_t operator()(const Cell & x) const
    {
        if ( Cell::based == Cell::X )
            return (size_t)x.x().to_ull();
        else
            return (size_t)x.ts().t.to_ull();
    }
};

using SectorHash = std::unordered_map<Cell, Cell, HashCell>;
using SectorOrdr = std::map<Cell, Cell>;

template <class T>
class MemoryMap
{
        T m;
    public:
        MemoryMap() {}
        void put(Cell place, Cell value);
        Cell & operator[](const Cell & k);
        string dump() const;
};

template <class T>
inline Cell & MemoryMap<T>::operator[](const Cell & k)
{
    //return m[k];
    auto j = m.find(k);
    if ( j == m.end() )
        throw "Memory access violation, address=" + k.str();
    return j->second;
}

template <class T>
inline void MemoryMap<T>::put(Cell place, Cell value)
{
    if (m.find(place) != m.end())
        throw "Memory overlap while loading: " + place.str();

    m[place] = value;
}

template <class T>
inline string MemoryMap<T>::dump() const
{
    MemoryTs ts;

    for (auto i : m)
        ts.put(i.first, i.second);

    return ts.dump();
}


#ifndef MEMDEF
#define MEMDEF 3
#endif

#if MEMDEF==1
using Memory = MemoryTs;
const char MemName[] = "1:Sectors";

#elif MEMDEF==2
using Memory = MemoryMap<SectorHash>;
const char MemName[] = "2:Hash";

#elif MEMDEF==3
using Memory = MemoryMap<SectorOrdr>;
const char MemName[] = "3:RB-Tree";

#else
#error MEMDEF bad value
#endif

