// CryptoLeq Enhanced Assembler
// New York University at Abu Dhabi, MoMa Lab
// Copyright 2014-2015
// Author: Oleg Mazonka
// File: stat.h

#pragma once

#include <string>
#include <vector>

#include "../processor/memcell.h"

#include "nodes.h"

using std::string;

struct Stat
{
    string filename;

    struct Entry
    {
        string name;
        Unumber val;
        Cell addr;
        unsigned long long counter = 0;

        Entry(string n) : name(n) {}
        Entry(Unumber n) : val(n) {}
    };

    std::vector<Entry> entries;
    bool addrinited = false;

    unsigned long long cntr_io = 0;
    unsigned long long cntr_opn = 0;
    unsigned long long cntr_sec = 0;
    unsigned long long cntr_mix = 0;

    void init(string file);
    void addEntry(string nm);
    string dumpEntries() const;
    void output() const;
    void output_e() const;
    void addrinit();
    void populate_addr(const Root & root);

    void ip(const Cell & i);
    void io() { ++cntr_io; }
    void instr(const Cell & a, const Cell & b);

};
